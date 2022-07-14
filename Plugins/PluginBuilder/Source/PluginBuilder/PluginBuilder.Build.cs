// Copyright 2022 Naotsun. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class PluginBuilder : ModuleRules
{
	public PluginBuilder(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
			}
		);
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"InputCore",
				"Engine",
				"UnrealEd",
				"Slate",
				"SlateCore",
				"EditorStyle",
				"MainFrame",
				"Settings",
				"Projects",
				"ToolMenus",
				"TargetPlatform",
				"DesktopPlatform",
			}
		);
		
		// To use version macros.
		PublicIncludePaths.AddRange(
			new string[]
			{
				Path.Combine(EngineDirectory, "Source", "Runtime", "Launch", "Resources"),
			}
		);
	}
}
