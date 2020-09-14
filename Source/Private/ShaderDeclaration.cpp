#include "ShaderActor.h"
#include "ShaderDeclaration.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"

//These are needed to actually implement the constant buffers so they are available inside our shader
//They also need to be unique over the entire solution since they can in fact be accessed from any shader
IMPLEMENT_UNIFORM_BUFFER_STRUCT(FPixelShaderConstantParameters, TEXT("PSConstant"))
IMPLEMENT_UNIFORM_BUFFER_STRUCT(FPixelShaderVariableParameters, TEXT("PSVariable"))

FPixelShaderDeclaration::FPixelShaderDeclaration(const
        ShaderMetaType::CompiledShaderInitializerType& Initializer)
    : FGlobalShader(Initializer) {
    //This call is what lets the shader system know that the surface OutputSurface is going to be available in the shader. The second parameter is the name it will be known by in the shader
    TextureParameter.Bind(Initializer.ParameterMap,
                          TEXT("TextureParameter"));  //The text parameter here is the name of the parameter in the shader
}

void FPixelShaderDeclaration::SetUniformBuffers(FRHICommandList& RHICmdList,
        FPixelShaderConstantParameters& ConstantParameters,
        FPixelShaderVariableParameters& VariableParameters) {
    FPixelShaderConstantParametersRef ConstantParametersBuffer;
    FPixelShaderVariableParametersRef VariableParametersBuffer;

    ConstantParametersBuffer =
        FPixelShaderConstantParametersRef::CreateUniformBufferImmediate(
            ConstantParameters, UniformBuffer_SingleFrame);
    VariableParametersBuffer =
        FPixelShaderVariableParametersRef::CreateUniformBufferImmediate(
            VariableParameters, UniformBuffer_SingleDraw);

    SetUniformBufferParameter(RHICmdList, GetPixelShader(),
                              GetUniformBufferParameter<FPixelShaderConstantParameters>(),
                              ConstantParametersBuffer);
    SetUniformBufferParameter(RHICmdList, GetPixelShader(),
                              GetUniformBufferParameter<FPixelShaderVariableParameters>(),
                              VariableParametersBuffer);
}

void FPixelShaderDeclaration::SetSurfaces(FRHICommandList& RHICmdList,
        FShaderResourceViewRHIRef TextureParameterSRV) {
    FPixelShaderRHIParamRef PixelShaderRHI = GetPixelShader();

    if (TextureParameter.IsBound()) { //This actually sets the shader resource view to the texture parameter in the shader :)
        RHICmdList.SetShaderResourceViewParameter(PixelShaderRHI,
                TextureParameter.GetBaseIndex(), TextureParameterSRV);
    }
}

void FPixelShaderDeclaration::UnbindBuffers(FRHICommandList& RHICmdList) {
    FPixelShaderRHIParamRef PixelShaderRHI = GetPixelShader();

    if (TextureParameter.IsBound()) {
        RHICmdList.SetShaderResourceViewParameter(PixelShaderRHI,
                TextureParameter.GetBaseIndex(), FShaderResourceViewRHIParamRef());
    }
}

//This is what will instantiate the shader into the engine from the engine/Shaders folder
//从引擎shader文件夹载入ShaderExample.usf文件(在项目启动时加载)
IMPLEMENT_SHADER_TYPE(, FVertexShaderExample, TEXT("ShaderExample"),
                      TEXT("MainVertexShader"), SF_Vertex);
IMPLEMENT_SHADER_TYPE(, FPixelShaderDeclaration, TEXT("ShaderExample"),
                      TEXT("MainPixelShader"), SF_Pixel);