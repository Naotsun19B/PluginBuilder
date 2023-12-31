// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace PluginBuilder
{
	/**
	 * A class that manages the name of the platform to be built.
	 */
	class PLUGINBUILDER_API FPlatformsBase
	{
	public:
		// A structure that summarizes platform names in UBT and ini formats respectively.
		struct PLUGINBUILDER_API FPlatform
		{
		public:
			// The name of the platform covered by UBT.
			FString UBTPlatformName;

			// The name of the platform covered by ini.
			FString IniPlatformName;

			// The identifier to group similar platforms together, such as "Mobile" and "Console".
			FName PlatformGroupName;
		};

	protected:
		// Define the event that filters available platforms when collecting.
		DECLARE_DELEGATE_RetVal_OneParam(bool, FFilterPlatform, const ITargetPlatform& /* Platform */);
		
	protected:
		// Constructor.
		explicit FPlatformsBase(const FFilterPlatform& InFilterPlatform);
		
		// Returns a list of available platform names.
		TArray<FPlatform> GetPlatformNames(const bool bWithRefresh);

		// Collects available platform names from ITargetPlatformManagerModule.
		void RefreshPlatformNames();

	private:
		// The list of available platform names.
		TArray<FPlatform> PlatformNames;
		
		// The filter used when collecting available platforms.
		FFilterPlatform FilterPlatform;
	};
}
