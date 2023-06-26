// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PluginBuilder/PluginBuilderGlobals.h"

namespace PluginBuilder
{
	/**
	 * A dataset used to process UAT batch file.
	 */
	struct PLUGINBUILDER_API FUATBatchFileParams
	{
	public:
		// The name of the plugin to build.
		FString PluginName;

		// The friendly name of the plugin to build.
		FString PluginFriendlyName;

		// Whether to use friendly names in plugin output files, editor menus, etc.
		bool bUseFriendlyName = true;
		
		// The version name of the plugin to build.
		FString PluginVersionName;
		
		// The path to the .uplugin file for the plugin you want to build.
		FString UPluginFile;

		// The path of the directory where pre-built plugins, zipped plugins, etc. are output.
		TOptional<FString> OutputDirectoryPath;

		// Whether to stop the packaging process as soon as the cancel button is pressed during packaging.
		bool bStopPackagingProcessImmediately = false;

	public:
		// Returns the name of the plugin formatted according to the value of bUseFriendlyName.
		FString GetPluginNameInSpecifiedFormat() const;
	};
	
	/**
	 * A dataset used to process plugin build.
	 */
	struct PLUGINBUILDER_API FBuildPluginParams
	{
	public:
		// The list of target platforms to build the plugin.
		TArray<FString> TargetPlatforms;
		
		// Whether to handle older versions that do not use the Rocket.txt file.
		bool bRocket = false;

		// Whether to create a subfolder in the output built plugins folder.
		bool bCreateSubFolder = false;

		// Whether to judge the header inclusion of the plug-in code strictly.
		bool bStrictIncludes = false;
	};

	/**
	 * A dataset used to process plugin zip up.
	 */
	struct PLUGINBUILDER_API FZipUpPluginParams
	{
	public:
		// Whether the plugin to build uses the content folder.
		bool bCanPluginContainContent = false;
		
		// Whether to put the zip files into a single folder. If false will use a per engine folder for each zip file.
		bool bOutputAllZipFilesToSingleFolder = false;

		// Whether the zip folder should keep the binaries folder. Marketplace submissions expect the binaries folder to be deleted.
		bool bKeepBinariesFolder = false;

		// Whether to keep the properties of uplugin that are deleted when outputting from UAT even after outputting.
		bool bKeepUPluginProperties = false;

		// The compressibility strength. Specify from 0 to 9.
		uint8 CompressionLevel = 0;

	public:
		// Returns whether the format is acceptable for submission to the marketplace.
		bool IsFormatExpectedByMarketplace() const;
	};
	
	/**
	 * A dataset used to process plugin packages.
	 */
	struct PLUGINBUILDER_API FPackagePluginParams
	{
	public:
		// The list of engine versions to build the plugin.
		TArray<FString> EngineVersions;
		
		// The dataset used to process UAT batch file.
		FUATBatchFileParams UATBatchFileParams;
		
		// The dataset used to process plugin build.
		TOptional<FBuildPluginParams> BuildPluginParams;

		// The dataset used to process plugin zip up.
		TOptional<FZipUpPluginParams> ZipUpPluginParams;

#if UE_5_00_OR_LATER
		// Whether to change the output log filter to show only log categories for this plugin when starting the package process.
		bool bShowOnlyLogsFromThisPluginWhenPackageProcessStarts = false;
#endif

	public:
		// Creates a parameter set from the values set in the editor preferences.
		static bool MakeDefault(FPackagePluginParams& Default);
		static bool MakeFromPluginFriendlyName(const FName& PluginFriendlyName, FPackagePluginParams& Params);

		// Returns whether the parameters is valid to start package plugin task.
		// Returns true if the specified plugin exists and all engine versions are installed.
		bool IsValid() const;
	};
}
