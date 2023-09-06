// Copyright 2022-2023 Naotsun. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class PluginBuilder : ModuleRules
{
	public PluginBuilder(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
#if UE_5_2_OR_LATER
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
#endif
		
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
	}
}
