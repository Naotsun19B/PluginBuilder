// Copyright 2022-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace PluginBuilder
{
	/**
	 * A class that defines the function executed from the shortcut key.
	 */
	class PLUGINBUILDER_API FPluginBuilderCommandActions
	{
	public:
		// Builds and packages the plugin based on the set build configuration.
		static void BuildPlugin();
		static bool CanBuildPlugin();

		// Whether to prevent editor platform compilation on the host.
		static void ToggleNoHostPlatform();
		static bool GetNoHostPlatformState();
		
		// Whether to handle older versions that do not use the Rocket.txt file.
		static void ToggleRocket();
		static bool GetRocketState();

		// Whether to create a subfolder in the output built plugins folder.
		static void ToggleCreateSubFolder();
		static bool GetCreateSubFolderState();

		// Whether to judge the header inclusion of the plugin code strictly.
		static void ToggleStrictIncludes();
		static bool GetStrictIncludesState();
		
		// Whether to embed the engine version to be built into the uplugin file.
		static void ToggleUnversioned();
		static bool GetUnversionedState();

		// Whether to create a zip file that contains only the files we need after the build.
		static void ToggleZipUp();
		static bool GetZipUpState();
		
		// Whether to put the zip files into a single folder.
		// If false will use a per engine folder for each zip file.
		static void ToggleOutputAllZipFilesToSingleFolder();
		static bool GetOutputAllZipFilesToSingleFolderState();

		// Whether the zip folder should keep the binaries folder.
		// Marketplace submissions expect the binaries folder to be deleted.
		static void ToggleKeepBinariesFolder();
		static bool GetKeepBinariesFolderState();

		// Whether to keep the properties of uplugin that are deleted when outputting from UAT even after outputting.
		// Examples of properties to delete by UAT: IsBetaVersion, IsExperimentalVersion, EnabledByDefault, etc.
		// Marketplace submissions expect to use the uplugin file output by UAT.
		static void ToggleKeepUPluginProperties();
		static bool GetKeepUPluginPropertiesState();
		
		// Whether to append the engine version to the zip file name.
		static void ToggleAppendEngineVersionToZipFileName();
		static bool GetAppendEngineVersionToZipFileNameState();

		// Opens the settings for Plugin Builder.
		static void OpenBuildSettings();

		// Whether to automatically upload packaged zip files to cloud storage after the zip step.
		static void ToggleAutoUploadZipFiles();
		static bool GetAutoUploadZipFilesState();

		// Whether to retrieve a share URL for each uploaded file.
		static void ToggleGetShareUrls();
		static bool GetGetShareUrlsState();

		// Opens the cloud storage provider settings page.
		static void OpenCloudStorageSettings();

		// Returns whether the current cloud storage provider is authenticated.
		static bool IsCloudStorageAuthenticated();

		// Returns whether the Get Share URLs toggle can be activated
		// (requires both authentication and Auto Upload to be enabled).
		static bool CanToggleGetShareUrls();

		// Sets conflict behavior to Replace: overwrite the existing file.
		static void SetConflictBehaviorReplace();
		static bool GetConflictBehaviorReplaceState();

		// Sets conflict behavior to Rename: keep both files by renaming the upload.
		static void SetConflictBehaviorRename();
		static bool GetConflictBehaviorRenameState();

		// Sets conflict behavior to Fail: abort if the file already exists.
		static void SetConflictBehaviorFail();
		static bool GetConflictBehaviorFailState();

		// Sets conflict behavior to Ignore: skip upload if file exists and retrieve a share URL for it.
		static void SetConflictBehaviorIgnore();
		static bool GetConflictBehaviorIgnoreState();

		// Runs only the build step (no zip, no upload).
		static void BuildPluginOnly();
		static bool CanBuildPluginOnly();

		// Runs only the zip step against existing build output.
		static void ZipPluginOnly();
		static bool CanZipPluginOnly();

		// Uploads existing zip files from the PackagedPlugins folder to cloud storage.
		static void UploadToCloud();
		static bool CanUploadToCloud();
	};
}
