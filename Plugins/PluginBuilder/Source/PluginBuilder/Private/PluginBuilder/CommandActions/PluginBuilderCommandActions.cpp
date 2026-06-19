// Copyright 2022-2026 Naotsun. All Rights Reserved.

#include "PluginBuilder/CommandActions/PluginBuilderCommandActions.h"
#include "PluginBuilder/Utilities/PluginPackager.h"
#include "PluginBuilder/Utilities/PluginBuilderEditorSettings.h"
#include "PluginBuilder/Utilities/PluginBuilderPackagingSettings.h"
#include "PluginBuilder/Utilities/OneDriveSettings.h"
#include "PluginBuilder/CloudStorages/CloudStorageManager.h"
#include "PluginBuilder/CloudStorages/ICloudStorageProvider.h"

namespace PluginBuilder
{
	void FPluginBuilderCommandActions::BuildPlugin()
	{
		FPluginPackager::StartPackagePluginTask();
	}

	bool FPluginBuilderCommandActions::CanBuildPlugin()
	{
		const auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		return (
			!FPluginPackager::IsPackagePluginTaskRunning() &&
			Settings.IsReadyToStartPackagePluginTask()
		);
	}
	
	void FPluginBuilderCommandActions::ToggleNoHostPlatform()
	{
		auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		Settings.bNoHostPlatform = !Settings.bNoHostPlatform;
	}

	bool FPluginBuilderCommandActions::GetNoHostPlatformState()
	{
		const auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		return Settings.bNoHostPlatform;
	}
	
	void FPluginBuilderCommandActions::ToggleRocket()
	{
		auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		Settings.bRocket = !Settings.bRocket;
	}

	bool FPluginBuilderCommandActions::GetRocketState()
	{
		const auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		return Settings.bRocket;
	}

	void FPluginBuilderCommandActions::ToggleCreateSubFolder()
	{
		auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		Settings.bCreateSubFolder = !Settings.bCreateSubFolder;
	}

	bool FPluginBuilderCommandActions::GetCreateSubFolderState()
	{
		const auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		return Settings.bCreateSubFolder;
	}

	void FPluginBuilderCommandActions::ToggleStrictIncludes()
	{
		auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		Settings.bStrictIncludes = !Settings.bStrictIncludes;
	}

	bool FPluginBuilderCommandActions::GetStrictIncludesState()
	{
		const auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		return Settings.bStrictIncludes;
	}

	void FPluginBuilderCommandActions::ToggleUnversioned()
	{
		auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		Settings.bUnversioned = !Settings.bUnversioned;
	}

	bool FPluginBuilderCommandActions::GetUnversionedState()
	{
		const auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		return Settings.bUnversioned;
	}

	void FPluginBuilderCommandActions::ToggleZipUp()
	{
		auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		Settings.bZipUp = !Settings.bZipUp;
	}

	bool FPluginBuilderCommandActions::GetZipUpState()
	{
		const auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		return Settings.bZipUp;
	}

	void FPluginBuilderCommandActions::ToggleOutputAllZipFilesToSingleFolder()
	{
		auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		Settings.bOutputAllZipFilesToSingleFolder = !Settings.bOutputAllZipFilesToSingleFolder;
	}

	bool FPluginBuilderCommandActions::GetOutputAllZipFilesToSingleFolderState()
	{
		const auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		return Settings.bOutputAllZipFilesToSingleFolder;
	}

	void FPluginBuilderCommandActions::ToggleKeepBinariesFolder()
	{
		auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		Settings.bKeepBinariesFolder = !Settings.bKeepBinariesFolder;
	}

	bool FPluginBuilderCommandActions::GetKeepBinariesFolderState()
	{
		const auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		return Settings.bKeepBinariesFolder;
	}

	void FPluginBuilderCommandActions::ToggleKeepUPluginProperties()
	{
		auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		Settings.bKeepUPluginProperties = !Settings.bKeepUPluginProperties;
	}

	bool FPluginBuilderCommandActions::GetKeepUPluginPropertiesState()
	{
		const auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		return Settings.bKeepUPluginProperties;
	}

	void FPluginBuilderCommandActions::ToggleAppendEngineVersionToZipFileName()
	{
		auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		Settings.bAppendEngineVersionToZipFileName = !Settings.bAppendEngineVersionToZipFileName;
	}

	bool FPluginBuilderCommandActions::GetAppendEngineVersionToZipFileNameState()
	{
		const auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		return Settings.bAppendEngineVersionToZipFileName;
	}

	void FPluginBuilderCommandActions::OpenBuildSettings()
	{
		OpenSettings<UPluginBuilderEditorSettings>();
	}

	void FPluginBuilderCommandActions::ToggleAutoUploadZipFiles()
	{
		auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		Settings.bAutoUploadAfterZip = !Settings.bAutoUploadAfterZip;
	}

	bool FPluginBuilderCommandActions::GetAutoUploadZipFilesState()
	{
		const auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		return Settings.bAutoUploadAfterZip;
	}

	void FPluginBuilderCommandActions::ToggleGetShareUrls()
	{
		auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		Settings.bGetShareUrls = !Settings.bGetShareUrls;
	}

	bool FPluginBuilderCommandActions::GetGetShareUrlsState()
	{
		const auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		return Settings.bGetShareUrls;
	}

	void FPluginBuilderCommandActions::OpenCloudStorageSettings()
	{
		OpenSettings<UOneDriveSettings>();
	}

	bool FPluginBuilderCommandActions::IsCloudStorageAuthenticated()
	{
		const TSharedPtr<ICloudStorageProvider> Provider = FCloudStorageManager::GetCurrentProvider();
		return (Provider.IsValid() && Provider->IsAuthenticated());
	}

	bool FPluginBuilderCommandActions::CanToggleGetShareUrls()
	{
		return (IsCloudStorageAuthenticated() && GetAutoUploadZipFilesState());
	}

	void FPluginBuilderCommandActions::SetConflictBehaviorReplace()
	{
		auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		Settings.ConflictBehavior = EOneDriveConflictBehavior::Replace;
		Settings.SaveConfig();
	}

	bool FPluginBuilderCommandActions::GetConflictBehaviorReplaceState()
	{
		return (GetSettings<UPluginBuilderPackagingSettings>().ConflictBehavior == EOneDriveConflictBehavior::Replace);
	}

	void FPluginBuilderCommandActions::SetConflictBehaviorRename()
	{
		auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		Settings.ConflictBehavior = EOneDriveConflictBehavior::Rename;
		Settings.SaveConfig();
	}

	bool FPluginBuilderCommandActions::GetConflictBehaviorRenameState()
	{
		return (GetSettings<UPluginBuilderPackagingSettings>().ConflictBehavior == EOneDriveConflictBehavior::Rename);
	}

	void FPluginBuilderCommandActions::SetConflictBehaviorFail()
	{
		auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		Settings.ConflictBehavior = EOneDriveConflictBehavior::Fail;
		Settings.SaveConfig();
	}

	bool FPluginBuilderCommandActions::GetConflictBehaviorFailState()
	{
		return (GetSettings<UPluginBuilderPackagingSettings>().ConflictBehavior == EOneDriveConflictBehavior::Fail);
	}
}
