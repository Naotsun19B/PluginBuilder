// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/Types/PlatformNames.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "Interfaces/ITargetPlatformManagerModule.h"
#include "Interfaces/ITargetPlatform.h"
#if UE_5_00_OR_LATER
#include "Styling/AppStyle.h"
#include "Misc/DataDrivenPlatformInfoRegistry.h"
#else
#include "EditorStyle.h"
#include "PlatformInfo.h"
#endif

namespace PluginBuilder
{
	TArray<FPlatformNames::FPlatformName> FPlatformNames::GetPlatformNames(bool bWithRefresh)
	{
		if (bWithRefresh)
		{
			RefreshPlatformNames();
		}
		
		return PlatformNames;
	}

	void FPlatformNames::RefreshPlatformNames()
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

			FString UBTPlatformName;
			FString IniPlatformName;
			{
#if UE_5_00_OR_LATER
				const FDataDrivenPlatformInfo& PlatformInfo = TargetPlatform->GetPlatformInfo();
				UBTPlatformName = PlatformInfo.UBTPlatformString;
				IniPlatformName = PlatformInfo.IniPlatformName.ToString();
#else
				const PlatformInfo::FPlatformInfo& PlatformInfo = TargetPlatform->GetPlatformInfo();
				PlatformInfo.UBTTargetId.ToString(UBTPlatformName);
				IniPlatformName = PlatformInfo.IniPlatformName;
#endif
				if (PlatformInfo.PlatformSubMenu != NAME_None)
				{
					IniPlatformName = PlatformInfo.PlatformSubMenu.ToString();
				}
			}
					
			if (IniPlatformName.IsEmpty())
			{
				continue;
			}

			const auto EqualsUBTPlatformName = [&UBTPlatformName](const FPlatformName& PlatformName) -> bool
			{
				return PlatformName.UBTPlatformName.Equals(UBTPlatformName);
			};
			if (PlatformNames.ContainsByPredicate(EqualsUBTPlatformName))
			{
				continue;
			}

			FPlatformName PlatformName;
			PlatformName.UBTPlatformName = UBTPlatformName;
			PlatformName.IniPlatformName = IniPlatformName;
			PlatformNames.Add(PlatformName);
		}
	}

	void FPlatformNames::ToggleTargetPlatform(const FPlatformName TargetPlatform)
	{
		auto& Settings = UPluginBuilderSettings::Get();
		
		if (Settings.TargetPlatforms.Contains(TargetPlatform.UBTPlatformName))
		{
			Settings.TargetPlatforms.Remove(TargetPlatform.UBTPlatformName);
		}
		else
		{
			Settings.TargetPlatforms.Add(TargetPlatform.UBTPlatformName);
		}

		Settings.SaveConfig();
	}

	bool FPlatformNames::GetTargetPlatformState(const FPlatformName TargetPlatform)
	{
		return UPluginBuilderSettings::Get().TargetPlatforms.Contains(TargetPlatform.UBTPlatformName);
	}

	TArray<FPlatformNames::FPlatformName> FPlatformNames::PlatformNames;
}
