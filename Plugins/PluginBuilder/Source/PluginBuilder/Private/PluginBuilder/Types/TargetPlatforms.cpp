// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/Types/TargetPlatforms.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "Interfaces/ITargetPlatformManagerModule.h"
#include "Interfaces/ITargetPlatform.h"
#if UE_5_00_OR_LATER
#include "Styling/AppStyle.h"
#include "Misc/DataDrivenPlatformInfoRegistry.h"
#else
#include "EditorStyleSet.h"
#include "PlatformInfo.h"
#endif

namespace PluginBuilder
{
	TArray<FTargetPlatforms::FTargetPlatform> FTargetPlatforms::GetPlatformNames(bool bWithRefresh)
	{
		if (bWithRefresh)
		{
			RefreshPlatformNames();
		}
		
		return PlatformNames;
	}

	void FTargetPlatforms::RefreshPlatformNames()
	{
		ITargetPlatformManagerModule& TargetPlatformManagerRef = GetTargetPlatformManagerRef();
		const TArray<ITargetPlatform*>& TargetPlatforms = TargetPlatformManagerRef.GetTargetPlatforms();
		PlatformNames.Reset(TargetPlatforms.Num());
		for (const ITargetPlatform* TargetPlatform : TargetPlatforms)
		{
			if (TargetPlatform == nullptr)
			{
				continue;
			}

			FTargetPlatform PlatformName;
			{
#if UE_5_00_OR_LATER
				const FDataDrivenPlatformInfo& PlatformInfo = TargetPlatform->GetPlatformInfo();
				PlatformName.UBTPlatformName = PlatformInfo.UBTPlatformString;
				PlatformName.IniPlatformName = PlatformInfo.IniPlatformName.ToString();
				PlatformName.PlatformGroupName = PlatformInfo.PlatformGroupName;
#else
				const PlatformInfo::FPlatformInfo& PlatformInfo = TargetPlatform->GetPlatformInfo();
				PlatformInfo.UBTTargetId.ToString(PlatformName.UBTPlatformName);
				PlatformName.IniPlatformName = PlatformInfo.IniPlatformName;
				PlatformName.PlatformGroupName = PlatformInfo.PlatformGroupName;
#endif
				if (PlatformInfo.PlatformSubMenu != NAME_None)
				{
					PlatformName.IniPlatformName = PlatformInfo.PlatformSubMenu.ToString();
				}
			}
			
			if (PlatformName.IniPlatformName.IsEmpty())
			{
				continue;
			}

			const FString& UBTPlatformName = PlatformName.UBTPlatformName;
			const auto EqualsUBTPlatformName = [&UBTPlatformName](const FTargetPlatform& PlatformName) -> bool
			{
				return PlatformName.UBTPlatformName.Equals(UBTPlatformName);
			};
			if (PlatformNames.ContainsByPredicate(EqualsUBTPlatformName))
			{
				continue;
			}
			
			PlatformNames.Add(PlatformName);
		}
	}

	void FTargetPlatforms::ToggleTargetPlatform(const FTargetPlatform TargetPlatform)
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

	bool FTargetPlatforms::GetTargetPlatformState(const FTargetPlatform TargetPlatform)
	{
		return UPluginBuilderSettings::Get().TargetPlatforms.Contains(TargetPlatform.UBTPlatformName);
	}

	TArray<FTargetPlatforms::FTargetPlatform> FTargetPlatforms::PlatformNames;
}
