// Copyright 2016-2017 STORM IN A TEACUP, S.R.L. All Rights Reserved.
#include "CoreUObject.h"
#include "Engine.h"
#include "ShaderActorPrivatePCH.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "ShaderActorModule"

//IMPLEMENT_MODULE(param1,param2)
//param1:class define in ShaderInPluginModule.h witch Inherited from IModuleInterface
//param2:class define in ShaderActor.Build.cs
IMPLEMENT_MODULE(ShaderActorModule, ShaderActor)


void ShaderActorModule::StartupModule()
{
	//TEXT("ShaderActor") is defined in ShaderActor.uproject
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("ShaderActor"))->GetBaseDir(), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/ShaderInPugin"), PluginShaderDir);
}

void ShaderActorModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE