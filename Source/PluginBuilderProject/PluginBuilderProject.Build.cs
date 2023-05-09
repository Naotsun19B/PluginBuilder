// Copyright 2022-2023 Naotsun. All Rights Reserved.

using UnrealBuildTool;

public class PluginBuilderProject : ModuleRules
{
	public PluginBuilderProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
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
