// Copyright 2022-2025 Naotsun. All Rights Reserved.

#include "PluginBuilder/CommandActions/PluginBuilderCommandActions.h"
#include "PluginBuilder/Utilities/PluginPackager.h"
#include "PluginBuilder/Utilities/PluginBuilderEditorSettings.h"
#include "PluginBuilder/Utilities/PluginBuilderBuildConfigurationSettings.h"

namespace PluginBuilder
{
	void FPluginBuilderCommandActions::BuildPlugin()
	{
		FPluginPackager::StartPackagePluginTask();
	}

	bool FPluginBuilderCommandActions::CanBuildPlugin()
	{
		const auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		return (
			!FPluginPackager::IsPackagePluginTaskRunning() &&
			Settings.IsReadyToStartPackagePluginTask()
		);
	}
	
	void FPluginBuilderCommandActions::ToggleNoHostPlatform()
	{
		auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		Settings.bNoHostPlatform = !Settings.bNoHostPlatform;
	}

	bool FPluginBuilderCommandActions::GetNoHostPlatformState()
	{
		const auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		return Settings.bNoHostPlatform;
	}
	
	void FPluginBuilderCommandActions::ToggleRocket()
	{
		auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		Settings.bRocket = !Settings.bRocket;
	}

	bool FPluginBuilderCommandActions::GetRocketState()
	{
		const auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		return Settings.bRocket;
	}

	void FPluginBuilderCommandActions::ToggleCreateSubFolder()
	{
		auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		Settings.bCreateSubFolder = !Settings.bCreateSubFolder;
	}

	bool FPluginBuilderCommandActions::GetCreateSubFolderState()
	{
		const auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		return Settings.bCreateSubFolder;
	}

	void FPluginBuilderCommandActions::ToggleStrictIncludes()
	{
		auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		Settings.bStrictIncludes = !Settings.bStrictIncludes;
	}

	bool FPluginBuilderCommandActions::GetStrictIncludesState()
	{
		const auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		return Settings.bStrictIncludes;
	}

	void FPluginBuilderCommandActions::ToggleUnversioned()
	{
		auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		Settings.bUnversioned = !Settings.bUnversioned;
	}

	bool FPluginBuilderCommandActions::GetUnversionedState()
	{
		const auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		return Settings.bUnversioned;
	}

	void FPluginBuilderCommandActions::ToggleZipUp()
	{
		auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		Settings.bZipUp = !Settings.bZipUp;
	}

	bool FPluginBuilderCommandActions::GetZipUpState()
	{
		const auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		return Settings.bZipUp;
	}

	void FPluginBuilderCommandActions::ToggleOutputAllZipFilesToSingleFolder()
	{
		auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		Settings.bOutputAllZipFilesToSingleFolder = !Settings.bOutputAllZipFilesToSingleFolder;
	}

	bool FPluginBuilderCommandActions::GetOutputAllZipFilesToSingleFolderState()
	{
		const auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		return Settings.bOutputAllZipFilesToSingleFolder;
	}

	void FPluginBuilderCommandActions::ToggleKeepBinariesFolder()
	{
		auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		Settings.bKeepBinariesFolder = !Settings.bKeepBinariesFolder;
	}

	bool FPluginBuilderCommandActions::GetKeepBinariesFolderState()
	{
		const auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		return Settings.bKeepBinariesFolder;
	}

	void FPluginBuilderCommandActions::ToggleKeepUPluginProperties()
	{
		auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		Settings.bKeepUPluginProperties = !Settings.bKeepUPluginProperties;
	}

	bool FPluginBuilderCommandActions::GetKeepUPluginPropertiesState()
	{
		const auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		return Settings.bKeepUPluginProperties;
	}

	void FPluginBuilderCommandActions::OpenBuildSettings()
	{
		OpenSettings<UPluginBuilderEditorSettings>();
	}
}
