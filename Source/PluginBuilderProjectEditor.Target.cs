// Copyright 2022-2025 Naotsun. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class PluginBuilderProjectEditorTarget : TargetRules
{
	public PluginBuilderProjectEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
#if UE_5_2_OR_LATER
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
#endif
		ExtraModuleNames.AddRange( new string[] { "PluginBuilderProject" } );
	}
}
