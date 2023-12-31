// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/Types/HostPlatforms.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "PluginBuilder/CommandActions/PluginBuilderCommandActions.h"

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
		UPluginBuilderSettings::ModifyProperties(
			[&HostPlatform](UPluginBuilderSettings& Settings)
			{
				if (Settings.HostPlatforms.Contains(HostPlatform.UBTPlatformName))
				{
					Settings.HostPlatforms.Remove(HostPlatform.UBTPlatformName);
				}
				else
				{
					Settings.HostPlatforms.Add(HostPlatform.UBTPlatformName);
				}
			}
		);
	}

	bool FHostPlatforms::GetHostPlatformState(const FPlatform HostPlatform)
	{
		return UPluginBuilderSettings::Get().HostPlatforms.Contains(HostPlatform.UBTPlatformName);
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
