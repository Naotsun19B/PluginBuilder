// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class SNotificationItem;

namespace PluginBuilder
{
	class FPackagePluginTask;
	
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
		
		// Opens the settings for Plugin Builder.
		static void OpenBuildSettings();
	};
}
