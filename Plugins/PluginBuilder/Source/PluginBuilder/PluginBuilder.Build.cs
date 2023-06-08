// Copyright 2022-2023 Naotsun. All Rights Reserved.

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

		if (Target.Version.MajorVersion >= 5)
		{
			PrivateDependencyModuleNames.Add("OutputLog");
		}
		
		// To use version macros.
		PublicIncludePaths.AddRange(
			new string[]
			{
				Path.Combine(EngineDirectory, "Source", "Runtime", "Launch", "Resources"),
			}
		);
	}
}
