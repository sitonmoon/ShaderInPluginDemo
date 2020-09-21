#include "ShaderActorPrivatePCH.h"
#include "ShaderActor.h"
#include "Public/PipelineStateCache.h"

//--UE4.16
//IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, ShaderActor, "ShaderActor");
//TGlobalResource<FTextureVertexDeclaration> GTextureVertexDeclaration;
//--UE4.23
TGlobalResource<FSimpleScreenVertexBuffer> GSimpleScreenVertexBuffer;

AShaderActor::AShaderActor(class FObjectInitializer const &ObjectInitializer): Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	StartColor = FColor::Green;

	VariableParameters = FPixelShaderVariableParameters();

	bMustRegenerateSRV = false;
	bIsPixelShaderExecuting = false;
	bIsUnloading = false;

	CurrentTexture = NULL;
	TextureParameterSRV = NULL;
}

AShaderActor::~AShaderActor()
{
	bIsUnloading = true; 
}


void AShaderActor::BeginPlay()
{
	Super::BeginPlay();

	FeatureLevel = GetWorld()->Scene->GetFeatureLevel();

	ConstantParameters = FPixelShaderConstantParameters();
	ConstantParameters.StartColor = FVector4(StartColor.R / 255.0, StartColor.G / 255.0, StartColor.B / 255.0, StartColor.A / 255.0);

	UStaticMeshComponent* StaticMeshComponent = (UStaticMeshComponent*)this->GetRootComponent();
	StaticMeshComponent->SetMaterial(0, MaterialToApply);
	UMaterialInstanceDynamic* pMID = StaticMeshComponent->CreateAndSetMaterialInstanceDynamic(0);
	if (pMID)
	{	
		pMID->SetTextureParameterValue("InputTexture", RenderTarget);
	}
	
}

void AShaderActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (RenderTarget)
	{
		EndColorBuildup = FMath::Clamp(EndColorBuildup + DeltaSeconds *
			EndColorBuildupDirection, 0.0f, 1.0f);

		if (EndColorBuildup >= 1.0 || EndColorBuildup <= 0) {
			EndColorBuildupDirection *= -1;
		}

		ExecutePixelShader(FVector4(EndColorBuildup, 0, 0, 1), 0.1f);
	}

}


void AShaderActor::ExecutePixelShader(FVector4 EndColor, float TextureParameterBlendFactor)
{
	if (bIsUnloading ) //Skip this execution round if we are already executing
	{
		return;
	}

	//Now set our runtime parameters!
	VariableParameters.EndColor = EndColor;
	VariableParameters.TextureParameterBlendFactor = TextureParameterBlendFactor;

	
	/*//--UE4.16
	ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
		FPixelShaderRunner,
		AShaderActor*, PixelShader, this,
		{
			PixelShader->ExecutePixelShaderInternal();
		}
	);
	*/
	//--UE4.23
	ENQUEUE_RENDER_COMMAND(FPixelShaderRunner)([this](
		FRHICommandListImmediate& RHICmdList)
		{
			this->ExecutePixelShaderInternal();
		}
	);
}

void AShaderActor::ExecutePixelShaderInternal()
{
	check(IsInRenderingThread());

	if (bIsUnloading) //If we are about to unload, so just clean up the SRV :)
	{
		if (NULL != TextureParameterSRV)
		{
			TextureParameterSRV.SafeRelease();
			TextureParameterSRV = NULL;
		}

		return;
	}

	FRHICommandListImmediate& RHICmdList = GRHICommandList.GetImmediateCommandList();

	//This is where the magic happens
	CurrentTexture = RenderTarget->GetRenderTargetResource()->GetRenderTargetTexture();
	
	//--UE4.16
	//SetRenderTarget(RHICmdList, CurrentTexture, FTextureRHIRef());
	//--UE4.23
	FRHIRenderPassInfo RPInfo(CurrentTexture, ERenderTargetActions::Load_Store);
	RHICmdList.BeginRenderPass(RPInfo, TEXT("ShaderActor"));

	TShaderMapRef<FVertexShaderExample> VertexShader(GetGlobalShaderMap(FeatureLevel));
	TShaderMapRef<FPixelShaderDeclaration> PixelShader(GetGlobalShaderMap(FeatureLevel));

	FGraphicsPipelineStateInitializer GraphicsPSOInit;
	RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);
	GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
	GraphicsPSOInit.BlendState = TStaticBlendState<>::GetRHI();
	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
	GraphicsPSOInit.PrimitiveType = PT_TriangleStrip;
	GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GFilterVertexDeclaration.VertexDeclarationRHI;
	GraphicsPSOInit.BoundShaderState.VertexShaderRHI = GETSAFERHISHADER_VERTEX(*VertexShader);
	GraphicsPSOInit.BoundShaderState.PixelShaderRHI = GETSAFERHISHADER_PIXEL(*PixelShader);
	SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

	PixelShader->SetSurfaces(RHICmdList, TextureParameterSRV);
	PixelShader->SetUniformBuffers(RHICmdList, ConstantParameters, VariableParameters);

	//--UE4.16
	//// Draw a fullscreen quad that we can run our pixel shader on
	//FTextureVertex Vertices[4];
	//Vertices[0].Position = FVector4(-1.0f, 1.0f, 0, 1.0f);
	//Vertices[1].Position = FVector4(1.0f, 1.0f, 0, 1.0f);
	//Vertices[2].Position = FVector4(-1.0f, -1.0f, 0, 1.0f);
	//Vertices[3].Position = FVector4(1.0f, -1.0f, 0, 1.0f);
	//Vertices[0].UV = FVector2D(0, 0);
	//Vertices[1].UV = FVector2D(1, 0);
	//Vertices[2].UV = FVector2D(0, 1);
	//Vertices[3].UV = FVector2D(1, 1);
	//DrawPrimitiveUP(RHICmdList, PT_TriangleStrip, 2, Vertices, sizeof(Vertices[0]));
	//--UE4.23
	//RHICmdList.SetViewport(0, 0, 0.0f, CurrentTexture->GetSizeX(), CurrentTexture->GetSizeY(), 1.0f);
	RHICmdList.SetStreamSource(0, GSimpleScreenVertexBuffer.VertexBufferRHI, 0);
	RHICmdList.DrawPrimitive(0, 2, 1);

	RHICmdList.EndRenderPass();
}