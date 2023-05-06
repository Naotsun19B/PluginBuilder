// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace PluginBuilder
{
	/**
	 * A class that manages the name of the platform to be built.
	 */
	class PLUGINBUILDER_API FTargetPlatforms
	{
	public:
		// A structure that summarizes platform names in UBT and ini formats respectively.
		struct PLUGINBUILDER_API FTargetPlatform
		{
		public:
			// A name of the platform covered by UBT.
			FString UBTPlatformName;

			// A name of the platform covered by ini.
			FString IniPlatformName;

			// A identifier to group similar platforms together, such as "Mobile" and "Console".
			FName PlatformGroupName;
		};

	public:
		// Returns a list of available platform names.
		static TArray<FTargetPlatform> GetPlatformNames(bool bWithRefresh = true);

		// Collects available platform names from ITargetPlatformManagerModule.
		static void RefreshPlatformNames();
		
		// Toggles selection state the specified target platform.
		static void ToggleTargetPlatform(const FTargetPlatform TargetPlatform);

		// Returns whether the specified target platform is selected.
		static bool GetTargetPlatformState(const FTargetPlatform TargetPlatform);

	private:
		// A list of available platform names.
		static TArray<FTargetPlatform> PlatformNames;
	};
}
