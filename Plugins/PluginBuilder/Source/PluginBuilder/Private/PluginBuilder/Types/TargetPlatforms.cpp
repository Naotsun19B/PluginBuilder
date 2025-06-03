// Copyright 2022-2025 Naotsun. All Rights Reserved.

#include "PluginBuilder/Types/TargetPlatforms.h"
#include "PluginBuilder/Utilities/PluginBuilderBuildConfigurationSettings.h"

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
		const auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		return Settings.TargetPlatforms.Contains(TargetPlatform.UBTPlatformName);
	}
}
