// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/CommandActions/PluginBuilderCommandActions.h"
#include "PluginBuilder/Utilities/PluginPackager.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"

namespace PluginBuilder
{
	void FPluginBuilderCommandActions::BuildPlugin()
	{
		FPluginPackager::StartPackagePluginTask();
	}

	bool FPluginBuilderCommandActions::CanBuildPlugin()
	{
		return (
			!FPluginPackager::IsPackagePluginTaskRunning() &&
			UPluginBuilderSettings::Get().IsReadyToStartPackagePluginTask()
		);
	}
	
	void FPluginBuilderCommandActions::ToggleRocket()
	{
		auto& Settings = UPluginBuilderSettings::Get();
		Settings.bRocket = !Settings.bRocket;
		Settings.SaveConfig();
	}

	bool FPluginBuilderCommandActions::GetRocketState()
	{
		return UPluginBuilderSettings::Get().bRocket;
	}

	void FPluginBuilderCommandActions::ToggleCreateSubFolder()
	{
		auto& Settings = UPluginBuilderSettings::Get();
		Settings.bCreateSubFolder = !Settings.bCreateSubFolder;
		Settings.SaveConfig();
	}

	bool FPluginBuilderCommandActions::GetCreateSubFolderState()
	{
		return UPluginBuilderSettings::Get().bCreateSubFolder;
	}

	void FPluginBuilderCommandActions::ToggleStrictIncludes()
	{
		auto& Settings = UPluginBuilderSettings::Get();
		Settings.bStrictIncludes = !Settings.bStrictIncludes;
		Settings.SaveConfig();
	}

	bool FPluginBuilderCommandActions::GetStrictIncludesState()
	{
		return UPluginBuilderSettings::Get().bStrictIncludes;
	}

	void FPluginBuilderCommandActions::ToggleZipUp()
	{
		auto& Settings = UPluginBuilderSettings::Get();
    	Settings.bZipUp = !Settings.bZipUp;
    	Settings.SaveConfig();
	}

	bool FPluginBuilderCommandActions::GetZipUpState()
	{
		return UPluginBuilderSettings::Get().bZipUp;
	}

	void FPluginBuilderCommandActions::OpenBuildSettings()
	{
		UPluginBuilderSettings::OpenSettings();
	}
}
