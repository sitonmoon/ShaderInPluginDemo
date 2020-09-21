#pragma once

#include "Engine.h"
#include "Engine/EngineTypes.h"
#include "GameFramework/Actor.h"
#include "Private/ShaderDeclaration.h"
#include "ShaderActor.generated.h"

UCLASS(BlueprintType, Blueprintable)
class SHADERACTOR_API AShaderActor: public AActor
{
	GENERATED_BODY()
public:
	AShaderActor(class FObjectInitializer const &ObjectInitializer);
	~AShaderActor();
	
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ShaderActor)
		FColor StartColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ShaderActor)
		UMaterialInterface* MaterialToApply; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ShaderActor)
		UTextureRenderTarget2D* RenderTarget; 

	void ExecutePixelShader(FVector4 EndColor, float TextureParameterBlendFactor);

	/************************************************************************/
	/* Only execute this from the render thread!!!                          */
	/************************************************************************/
	void ExecutePixelShaderInternal();


private:
	bool bIsPixelShaderExecuting;
	bool bMustRegenerateSRV;
	bool bIsUnloading;

	float EndColorBuildup = 0.0f;
	float EndColorBuildupDirection = 1.0f;

	FPixelShaderConstantParameters ConstantParameters;
	FPixelShaderVariableParameters VariableParameters;
	ERHIFeatureLevel::Type FeatureLevel;

	/** Main texture */
	FTexture2DRHIRef CurrentTexture;

	/** Since we are only reading from the resource, we do not need a UAV; an SRV is sufficient */
	FShaderResourceViewRHIRef TextureParameterSRV;
};