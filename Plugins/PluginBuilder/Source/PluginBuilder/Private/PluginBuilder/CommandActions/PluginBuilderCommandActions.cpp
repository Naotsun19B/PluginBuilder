// Copyright 2022 Naotsun. All Rights Reserved.

#include "PluginBuilder/CommandActions/PluginBuilderCommandActions.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"

namespace PluginBuilder
{
	void FPluginBuilderCommandActions::BuildPlugin()
	{
	}

	bool FPluginBuilderCommandActions::CanBuildPlugin()
	{
		const auto& Settings = UPluginBuilderSettings::Get();
		
		return (
			Settings.SelectedBuildTarget.IsSet() &&
			(Settings.EngineVersions.Num() > 0)
		);
	}

	void FPluginBuilderCommandActions::OpenBuildSettings()
	{
		UPluginBuilderSettings::OpenSettings();
	}
}
