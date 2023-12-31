// Copyright 2022-2024 Naotsun. All Rights Reserved.

using UnrealBuildTool;

public class PluginBuilderProject : ModuleRules
{
	public PluginBuilderProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
#if UE_5_2_OR_LATER
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
#endif
	
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core", 
				"CoreUObject", 
				"Engine", 
				"InputCore",
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"PluginBuilder",
			}
		);
	}
}
