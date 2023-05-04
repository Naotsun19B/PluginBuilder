// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace PluginBuilder
{
	/**
	 * A class that manages the name of the platform to be built.
	 */
	class PLUGINBUILDER_API FPlatformNames
	{
	public:
		// A structure that summarizes platform names in UBT and ini formats respectively.
		struct FPlatformName
		{
		public:
			// The name of the platform covered by UBT.
			FString UBTPlatformName;

			// The name of the platform covered by ini.
			FString IniPlatformName;
		};

	public:
		// Returns a list of available platform names.
		static TArray<FPlatformName> GetPlatformNames(bool bWithRefresh = true);

		// Collects available platform names from ITargetPlatformManagerModule.
		static void RefreshPlatformNames();
		
		// Toggles selection state the specified target platform.
		static void ToggleTargetPlatform(const FPlatformName TargetPlatform);

		// Returns whether the specified target platform is selected.
		static bool GetTargetPlatformState(const FPlatformName TargetPlatform);

	private:
		// A list of available platform names.
		static TArray<FPlatformName> PlatformNames;
	};
}
