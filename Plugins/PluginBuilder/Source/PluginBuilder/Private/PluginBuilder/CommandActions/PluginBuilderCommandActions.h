// Copyright 2022-2023 Naotsun. All Rights Reserved.

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

		// Whether to handle older versions that do not use the Rocket.txt file.
		static void ToggleRocket();
		static bool GetRocketState();

		// Whether to create a subfolder in the output built plugins folder.
		static void ToggleCreateSubFolder();
		static bool GetCreateSubFolderState();

		// Whether to judge the header inclusion of the plugin code strictly.
		static void ToggleStrictIncludes();
		static bool GetStrictIncludesState();

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
		
		// Opens the settings for Plugin Builder.
		static void OpenBuildSettings();
	};
}
