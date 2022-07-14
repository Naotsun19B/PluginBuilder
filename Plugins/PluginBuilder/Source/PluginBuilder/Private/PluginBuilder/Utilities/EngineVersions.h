// Copyright 2022 Naotsun. All Rights Reserved.

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
		struct FEngineVersion
		{
		public:
			// The installed engine version string.
			FString VersionName;

			// The path to the root directory of the installed engine.
			FString InstalledDirectory;
			
			// RunUAT.bat file path for the installed engine.
			FString UATBatchFile;
		};
		
	public:
		// Returns a list of installed engine version information.
		static TArray<FEngineVersion> GetEngineVersions(bool bWithRefresh = true);

		// Collect engine version information from the Windows registry.
		static void RefreshEngineVersions();

		// Returns a list of installed engine version names.
		static TArray<FString> GetVersionNames();

		// Search for the RunUAT.bat file path from the version name.
		static bool FindUATBatchFileByVersionName(const FString& VersionName, FString& UATBatchFile);
		
	private:
		// A list of installed engine version information.
		static TArray<FEngineVersion> EngineVersions;
	};
}
