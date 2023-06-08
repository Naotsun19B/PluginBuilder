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
		UPluginBuilderSettings::ModifyProperties(
			[](UPluginBuilderSettings& Settings)
			{
				Settings.bRocket = !Settings.bRocket;
			}
		);
	}

	bool FPluginBuilderCommandActions::GetRocketState()
	{
		return UPluginBuilderSettings::Get().bRocket;
	}

	void FPluginBuilderCommandActions::ToggleCreateSubFolder()
	{
		UPluginBuilderSettings::ModifyProperties(
			[](UPluginBuilderSettings& Settings)
			{
				Settings.bCreateSubFolder = !Settings.bCreateSubFolder;
			}
		);
	}

	bool FPluginBuilderCommandActions::GetCreateSubFolderState()
	{
		return UPluginBuilderSettings::Get().bCreateSubFolder;
	}

	void FPluginBuilderCommandActions::ToggleStrictIncludes()
	{
		UPluginBuilderSettings::ModifyProperties(
			[](UPluginBuilderSettings& Settings)
			{
				Settings.bStrictIncludes = !Settings.bStrictIncludes;
			}
		);
	}

	bool FPluginBuilderCommandActions::GetStrictIncludesState()
	{
		return UPluginBuilderSettings::Get().bStrictIncludes;
	}

	void FPluginBuilderCommandActions::ToggleZipUp()
	{
		UPluginBuilderSettings::ModifyProperties(
			[](UPluginBuilderSettings& Settings)
			{
				Settings.bZipUp = !Settings.bZipUp;
			}
		);
	}

	bool FPluginBuilderCommandActions::GetZipUpState()
	{
		return UPluginBuilderSettings::Get().bZipUp;
	}

	void FPluginBuilderCommandActions::ToggleOutputAllZipFilesToSingleFolder()
	{
		UPluginBuilderSettings::ModifyProperties(
			[](UPluginBuilderSettings& Settings)
			{
				Settings.bOutputAllZipFilesToSingleFolder = !Settings.bOutputAllZipFilesToSingleFolder;
			}
		);
	}

	bool FPluginBuilderCommandActions::GetOutputAllZipFilesToSingleFolderState()
	{
		return UPluginBuilderSettings::Get().bOutputAllZipFilesToSingleFolder;
	}

	void FPluginBuilderCommandActions::ToggleKeepBinariesFolder()
	{
		UPluginBuilderSettings::ModifyProperties(
			[](UPluginBuilderSettings& Settings)
			{
				Settings.bKeepBinariesFolder = !Settings.bKeepBinariesFolder;
			}
		);
	}

	bool FPluginBuilderCommandActions::GetKeepBinariesFolderState()
	{
		return UPluginBuilderSettings::Get().bKeepBinariesFolder;
	}

	void FPluginBuilderCommandActions::ToggleKeepUPluginProperties()
	{
		UPluginBuilderSettings::ModifyProperties(
			[](UPluginBuilderSettings& Settings)
			{
				Settings.bKeepUPluginProperties = !Settings.bKeepUPluginProperties;
			}
		);
	}

	bool FPluginBuilderCommandActions::GetKeepUPluginPropertiesState()
	{
		return UPluginBuilderSettings::Get().bKeepUPluginProperties;
	}

	void FPluginBuilderCommandActions::OpenBuildSettings()
	{
		UPluginBuilderSettings::OpenSettings();
	}
}
