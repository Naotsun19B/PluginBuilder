// Copyright 2022-2025 Naotsun. All Rights Reserved.

#include "PluginBuilder/Types/HostPlatforms.h"
#include "PluginBuilder/Utilities/PluginBuilderBuildConfigurationSettings.h"
#include "PluginBuilder/CommandActions/PluginBuilderCommandActions.h"
#include "Interfaces/ITargetPlatform.h"

namespace PluginBuilder
{
	namespace HostPlatforms
	{
		static FHostPlatforms& GetInstance()
		{
			static TUniquePtr<FHostPlatforms> Instance;
			if (!Instance.IsValid())
			{
				Instance = MakeUnique<FHostPlatforms>();
			}

			return *Instance.Get();
		}
		
		static bool IsHostPlatform(const ITargetPlatform& Platform)
		{
			return Platform.HasEditorOnlyData();
		}
	}

	FHostPlatforms::FHostPlatforms()
		: FPlatformsBase(FFilterPlatform::CreateStatic(&HostPlatforms::IsHostPlatform))
	{
	}

	TArray<FHostPlatforms::FPlatform> FHostPlatforms::GetHostPlatformNames(const bool bWithRefresh /* = true */)
	{
		return HostPlatforms::GetInstance().GetPlatformNames(bWithRefresh);
	}

	void FHostPlatforms::RefreshHostPlatformNames()
	{
		return HostPlatforms::GetInstance().RefreshPlatformNames();
	}

	void FHostPlatforms::ToggleHostPlatform(const FPlatform HostPlatform)
	{
		auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		if (Settings.HostPlatforms.Contains(HostPlatform.UBTPlatformName))
		{
			Settings.HostPlatforms.Remove(HostPlatform.UBTPlatformName);
		}
		else
		{
			Settings.HostPlatforms.Add(HostPlatform.UBTPlatformName);
		}
	}

	bool FHostPlatforms::GetHostPlatformState(const FPlatform HostPlatform)
	{
		const auto& Settings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		return Settings.HostPlatforms.Contains(HostPlatform.UBTPlatformName);
	}

	void FHostPlatforms::ToggleNoHostPlatform()
	{
		FPluginBuilderCommandActions::ToggleNoHostPlatform();
	}

	bool FHostPlatforms::GetNoHostPlatformState()
	{
		return FPluginBuilderCommandActions::GetNoHostPlatformState();
	}
}
