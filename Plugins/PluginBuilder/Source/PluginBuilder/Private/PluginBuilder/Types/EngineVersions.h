// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace PluginBuilder
{
	/**
	 * A class that gets the installed engine version and directory path from the Windows registry.
	 */
	class PLUGINBUILDER_API FEngineVersions
	{
	public:
		// Engine version information that can be obtained from the registry.
		struct PLUGINBUILDER_API FEngineVersion
		{
		public:
			// The installed engine version string.
			FString VersionName;

			// The path to the root directory of the installed engine.
			FString InstalledDirectory;
			
			// RunUAT.bat file path for the installed engine.
			FString UATBatchFile;

			// Kind of like UE4 or UE5 or a custom version.
			FString MajorVersionName;
		};
		
	public:
		// Returns a list of installed engine version information.
		static TArray<FEngineVersion> GetEngineVersions(const bool bWithRefresh = true);

		// Collects engine version information from the Windows registry.
		static void RefreshEngineVersions();

		// Returns a list of installed engine version names.
		static TArray<FString> GetVersionNames();

		// Searches for the RunUAT.bat file path from the version name.
		static bool FindUATBatchFileByVersionName(const FString& VersionName, FString& UATBatchFile, const bool bWithRefresh = true);

		// Toggles selection state the specified engine version.
		static void ToggleEngineVersion(const FEngineVersion EngineVersion);

		// Returns whether the specified engine version is selected.
		static bool GetEngineVersionState(const FEngineVersion EngineVersion);
		
	private:
		// A list of installed engine version information.
		static TArray<FEngineVersion> EngineVersions;
	};
}
