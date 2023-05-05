// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace PluginBuilder
{
	/**
	 * A dataset used to process plugin build.
	 */
	struct PLUGINBUILDER_API FBuildPluginParams
	{
	public:
		// The name of the plugin to build.
		FString PluginName;
		
		// The version name of the plugin to build.
		FString PluginVersionName;

		// Whether the plugin to build uses the content folder.
		bool bCanPluginContainContent = false;

		// The path to the .uplugin file for the plugin you want to build.
		FString UPluginFile;
		
		// A list of engine versions to build the plugin.
		TArray<FString> EngineVersions;

		// The path of the directory where pre-built plugins, zipped plugins, etc. are output.
		TOptional<FString> OutputDirectoryPath;
		
		// A list of target platforms to build the plugin.
		TArray<FString> TargetPlatforms;
		
		// Whether to handle older versions that do not use the Rocket.txt file.
		bool bRocket = false;

		// Whether to create a subfolder in the output built plugins folder.
		bool bCreateSubFolder = false;

		// Whether to judge the header inclusion of the plug-in code strictly.
		bool bStrictIncludes = false;

		// Whether to create a zip file that contains only the files we need after the build.
		bool bZipUp = false;

		// Whether to stop the packaging process as soon as the cancel button is pressed during packaging.
		bool bStopPackagingProcessImmediately = false;
	};
	
	/**
	 * A dataset used to process plugin packages.
	 */
	struct PLUGINBUILDER_API FPackagePluginParams
	{
	public:
		// A list of engine versions to build the plugin.
		TArray<FString> EngineVersions;

		// A dataset used to process plugin build.
		FBuildPluginParams BuildPluginParams;

	public:
		// Creates a parameter set from the values set in the editor preferences.
		static bool MakeDefault(FPackagePluginParams& Default);
	};
}
