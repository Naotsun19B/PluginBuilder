// Copyright 2022-2026 Naotsun. All Rights Reserved.

#include "PluginBuilder/CommandActions/PluginBuilderCommandActions.h"
#include "PluginBuilder/Utilities/PluginPackager.h"
#include "PluginBuilder/Utilities/PluginBuilderEditorSettings.h"
#include "PluginBuilder/Utilities/PluginBuilderPackagingSettings.h"
#include "PluginBuilder/Utilities/OneDriveSettings.h"
#include "PluginBuilder/CloudStorages/CloudStorageManager.h"
#include "PluginBuilder/CloudStorages/ICloudStorageProvider.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Framework/Application/SlateApplication.h"
#include "HAL/PlatformFileManager.h"
#include "HAL/FileManager.h"
#include "Misc/Paths.h"

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

	void FPluginBuilderCommandActions::SetConflictBehaviorIgnore()
	{
		auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		Settings.ConflictBehavior = EOneDriveConflictBehavior::Ignore;
		Settings.SaveConfig();
	}

	bool FPluginBuilderCommandActions::GetConflictBehaviorIgnoreState()
	{
		return (GetSettings<UPluginBuilderPackagingSettings>().ConflictBehavior == EOneDriveConflictBehavior::Ignore);
	}

	void FPluginBuilderCommandActions::BuildPluginOnly()
	{
		FPluginPackager::StartBuildOnlyTask();
	}

	bool FPluginBuilderCommandActions::CanBuildPluginOnly()
	{
		const auto& Settings = GetSettings<UPluginBuilderPackagingSettings>();
		return (
			!FPluginPackager::IsPackagePluginTaskRunning() &&
			Settings.IsReadyToStartPackagePluginTask()
		);
	}

	void FPluginBuilderCommandActions::ZipPluginOnly()
	{
		FPluginPackager::StartZipOnlyTask();
	}

	bool FPluginBuilderCommandActions::CanZipPluginOnly()
	{
		const auto& PackagingSettings = GetSettings<UPluginBuilderPackagingSettings>();
		if (FPluginPackager::IsPackagePluginTaskRunning() || !PackagingSettings.IsReadyToStartPackagePluginTask())
		{
			return false;
		}
		const auto& EditorSettings = GetSettings<UPluginBuilderEditorSettings>();
		if (!EditorSettings.bSelectOutputDirectoryManually && !EditorSettings.OutputDirectoryPath.Path.IsEmpty())
		{
			const FString BuiltPluginsPath = (EditorSettings.OutputDirectoryPath.Path / TEXT("BuiltPlugins"));
			return FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*BuiltPluginsPath);
		}
		return true;
	}

	void FPluginBuilderCommandActions::UploadToCloud()
	{
		const auto& PackagingSettings = GetSettings<UPluginBuilderPackagingSettings>();
		const auto& EditorSettings = GetSettings<UPluginBuilderEditorSettings>();

		FString PackagedPluginsPath;
		if (!EditorSettings.bSelectOutputDirectoryManually && !EditorSettings.OutputDirectoryPath.Path.IsEmpty())
		{
			PackagedPluginsPath = (EditorSettings.OutputDirectoryPath.Path / TEXT("PackagedPlugins"));
		}

		if (!FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*PackagedPluginsPath))
		{
			IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
			if (DesktopPlatform == nullptr)
			{
				return;
			}

			FString SelectedPath;
			if (!DesktopPlatform->OpenDirectoryDialog(
				FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
				TEXT("Select PackagedPlugins Directory"),
				FPaths::ProjectDir(),
				SelectedPath
			))
			{
				return;
			}
			PackagedPluginsPath = SelectedPath;
		}

		TArray<FString> ZipFilePaths;
		IFileManager::Get().FindFilesRecursive(ZipFilePaths, *PackagedPluginsPath, TEXT("*.zip"), true, false);

		if (ZipFilePaths.Num() == 0)
		{
			UE_LOG(LogPluginBuilder, Warning, TEXT("Cloud Storage upload: No zip files found in %s"), *PackagedPluginsPath);
			return;
		}

		FString PluginName;
		if (PackagingSettings.SelectedBuildTarget.IsSet())
		{
			PluginName = (EditorSettings.bUseFriendlyName ?
				PackagingSettings.SelectedBuildTarget->GetPluginFriendlyName() :
				PackagingSettings.SelectedBuildTarget->GetPluginName());
		}

		FPluginPackager::StartUploadOnlyTask(ZipFilePaths, PackagedPluginsPath, PluginName, PackagingSettings.bGetShareUrls);
	}

	bool FPluginBuilderCommandActions::CanUploadToCloud()
	{
		if (FPluginPackager::IsPackagePluginTaskRunning() || !IsCloudStorageAuthenticated())
		{
			return false;
		}
		const auto& EditorSettings = GetSettings<UPluginBuilderEditorSettings>();
		if (!EditorSettings.bSelectOutputDirectoryManually && !EditorSettings.OutputDirectoryPath.Path.IsEmpty())
		{
			const FString PackagedPluginsPath = (EditorSettings.OutputDirectoryPath.Path / TEXT("PackagedPlugins"));
			return FPlatformFileManager::Get().GetPlatformFile().DirectoryExists(*PackagedPluginsPath);
		}
		return true;
	}
}
