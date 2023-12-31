// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/Types/PlatformsBase.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "Interfaces/ITargetPlatformManagerModule.h"
#include "Interfaces/ITargetPlatform.h"
#if UE_5_00_OR_LATER
#include "Misc/DataDrivenPlatformInfoRegistry.h"
#else
#include "PlatformInfo.h"
#endif

namespace PluginBuilder
{
	FPlatformsBase::FPlatformsBase(const FFilterPlatform& InFilterPlatform)
		: FilterPlatform(InFilterPlatform)
	{
	}

	TArray<FPlatformsBase::FPlatform> FPlatformsBase::GetPlatformNames(const bool bWithRefresh)
	{
		if (bWithRefresh)
		{
			RefreshPlatformNames();
		}
		
		return PlatformNames;
	}

	void FPlatformsBase::RefreshPlatformNames()
	{
		check(FilterPlatform.IsBound());
		
		ITargetPlatformManagerModule& TargetPlatformManagerRef = GetTargetPlatformManagerRef();
		const TArray<ITargetPlatform*>& PlatformsBase = TargetPlatformManagerRef.GetTargetPlatforms();
		PlatformNames.Reset(PlatformsBase.Num());
		for (const auto* TargetPlatform : PlatformsBase)
		{
			if (TargetPlatform == nullptr)
			{
				continue;
			}

			if (!FilterPlatform.Execute(*TargetPlatform))
			{
				continue;
			}

			FPlatform PlatformName;
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
			const auto EqualsUBTPlatformName = [&UBTPlatformName](const FPlatform& PlatformName) -> bool
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
}
