// Copyright 2022-2025 Naotsun. All Rights Reserved.

#include "PluginBuilder/Types/TargetPlatforms.h"
#include "PluginBuilder/Utilities/PluginBuilderBuildConfigurationSettings.h"
#include "PluginBuilder/PluginBuilderGlobals.h"

namespace PluginBuilder
{
	namespace TargetPlatforms
	{
		static FTargetPlatforms& GetInstance()
		{
			static TUniquePtr<FTargetPlatforms> Instance;
			if (!Instance.IsValid())
			{
				Instance = MakeUnique<FTargetPlatforms>();
			}

			return *Instance.Get();
		}
		
		static bool PassEverything(const ITargetPlatform& Platform)
		{
			return true;
		}
	}

	FTargetPlatforms::FTargetPlatforms()
		: FPlatformsBase(FFilterPlatform::CreateStatic(&TargetPlatforms::PassEverything))
	{
	}

	TArray<FTargetPlatforms::FPlatform> FTargetPlatforms::GetTargetPlatformNames(const bool bWithRefresh /* = true */)
	{
		return TargetPlatforms::GetInstance().GetPlatformNames(bWithRefresh);
	}

	void FTargetPlatforms::RefreshTargetPlatformNames()
	{
		return TargetPlatforms::GetInstance().RefreshPlatformNames();
	}

	bool FTargetPlatforms::IsAvailableTargetPlatform(const FString& TargetPlatformName)
	{
		return TargetPlatforms::GetInstance().IsAvailablePlatform(TargetPlatformName);
	}

	void FTargetPlatforms::LogAvailableTargetPlatformNames()
	{
		UE_LOG(LogPluginBuilder, Log, TEXT("==================== Available Target Platforms ==================="));

		const TArray<FPlatform>& TargetPlatforms = GetTargetPlatformNames();
		for (const auto& TargetPlatform : TargetPlatforms)
		{
			if (!TargetPlatform.bIsAvailable)
			{
				continue;
			}
				
			UE_LOG(LogPluginBuilder, Log, TEXT("%s (%s)"), *TargetPlatform.UBTPlatformName, *TargetPlatform.PlatformGroupName.ToString());
		}
	}

	void FTargetPlatforms::ToggleTargetPlatform(const FPlatform TargetPlatform)
	{
		auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		if (Settings.TargetPlatforms.Contains(TargetPlatform.UBTPlatformName))
		{
			Settings.TargetPlatforms.Remove(TargetPlatform.UBTPlatformName);
		}
		else
		{
			Settings.TargetPlatforms.Add(TargetPlatform.UBTPlatformName);
		}
	}

	bool FTargetPlatforms::GetTargetPlatformState(const FPlatform TargetPlatform)
	{
		if (!TargetPlatform.bIsAvailable)
		{
			return false;
		}
		
		const auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		return Settings.TargetPlatforms.Contains(TargetPlatform.UBTPlatformName);
	}
}
