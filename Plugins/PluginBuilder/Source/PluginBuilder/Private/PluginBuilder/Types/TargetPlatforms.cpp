// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/Types/TargetPlatforms.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"

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
		UPluginBuilderSettings::ModifyProperties(
			[&TargetPlatform](UPluginBuilderSettings& Settings)
			{
				if (Settings.TargetPlatforms.Contains(TargetPlatform.UBTPlatformName))
				{
					Settings.TargetPlatforms.Remove(TargetPlatform.UBTPlatformName);
				}
				else
				{
					Settings.TargetPlatforms.Add(TargetPlatform.UBTPlatformName);
				}
			}
		);
	}

	bool FTargetPlatforms::GetTargetPlatformState(const FPlatform TargetPlatform)
	{
		return UPluginBuilderSettings::Get().TargetPlatforms.Contains(TargetPlatform.UBTPlatformName);
	}
}
