
using UnrealBuildTool;

public class ShaderActor : ModuleRules
{
    public ShaderActor(ReadOnlyTargetRules Target) : base(Target)
    {   
        // This prevents UBT from complaining when it finds module PCH before class header
        PCHUsage = PCHUsageMode.NoSharedPCHs;

        PrivatePCHHeaderFile = "Private/ShaderActorPrivatePCH.h";

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                    "Core",
                    "CoreUObject",
                    "InputCore",
                    "Engine",
                    "RenderCore",
                    //"ShaderCore",
                    "RHI"
            }
            );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "Projects",
				// ... add private dependencies that you statically link with here ...	
			}
            );
    }
}
