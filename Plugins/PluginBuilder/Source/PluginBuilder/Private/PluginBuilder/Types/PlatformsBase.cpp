// Copyright 2022-2025 Naotsun. All Rights Reserved.

#include "PluginBuilder/Types/PlatformsBase.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "Interfaces/ITargetPlatformManagerModule.h"
#include "Interfaces/ITargetPlatform.h"
#include "Interfaces/IProjectManager.h"
#include "Misc/CoreMisc.h"
#if UE_5_00_OR_LATER
#include "Misc/DataDrivenPlatformInfoRegistry.h"
#include "Interfaces/ITurnkeySupportModule.h"
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
		
		return Platforms;
	}

	void FPlatformsBase::RefreshPlatformNames()
	{
		check(FilterPlatform.IsBound());

		static const bool bCodeBasedProject = []() -> bool
		{
			FProjectStatus ProjectStatus;
			if (!IProjectManager::Get().QueryStatusForCurrentProject(ProjectStatus))
			{
				return false;
			}

			return ProjectStatus.bCodeBasedProject;
		}();
		
		ITargetPlatformManagerModule& TargetPlatformManagerRef = GetTargetPlatformManagerRef();
		const TArray<ITargetPlatform*>& PlatformsBase = TargetPlatformManagerRef.GetTargetPlatforms();
		Platforms.Reset(PlatformsBase.Num());
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

			FPlatform Platform;
			{
#if UE_5_00_OR_LATER
				const FDataDrivenPlatformInfo& PlatformInfo = TargetPlatform->GetPlatformInfo();
				Platform.UBTPlatformName = PlatformInfo.UBTPlatformString;
				Platform.IniPlatformName = PlatformInfo.IniPlatformName.ToString();

				auto Test = ITurnkeySupportModule::Get().GetSdkInfo(*Platform.IniPlatformName, false);
				const ETurnkeyPlatformSdkStatus Status = ITurnkeySupportModule::Get().GetSdkInfo(*Platform.IniPlatformName, false).Status;
				switch (Status)
				{
				case ETurnkeyPlatformSdkStatus::OutOfDate:
				case ETurnkeyPlatformSdkStatus::NoSdk:
					Platform.IconStyleName = TEXT("Icons.Warning");
					break;

				case ETurnkeyPlatformSdkStatus::Error:
					Platform.IconStyleName = TEXT("Icons.Error");
					break;

				case ETurnkeyPlatformSdkStatus::Unknown:
					Platform.IconStyleName = TEXT("Icons.Help");
					break;

				default:
					Platform.IconStyleName = PlatformInfo.GetIconStyleName(EPlatformIconSize::Normal);
					Platform.bIsAvailable = true;
					break;
				}
#else
				const PlatformInfo::FPlatformInfo& PlatformInfo = TargetPlatform->GetPlatformInfo();
				Platform.UBTPlatformName = PlatformInfo.UBTTargetId.ToString();
				Platform.IniPlatformName = PlatformInfo.IniPlatformName;
				
				const PlatformInfo::EPlatformSDKStatus SDKStatus = PlatformInfo.SDKStatus;
				switch (SDKStatus)
				{
				case PlatformInfo::EPlatformSDKStatus::Installed:
					Platform.IconStyleName = PlatformInfo.GetIconStyleName(PlatformInfo::EPlatformIconSize::Normal);
					Platform.bIsAvailable = true;
					break;
					
				case PlatformInfo::EPlatformSDKStatus::NotInstalled:
					Platform.IconStyleName = TEXT("Icons.Warning");
					break;
					
				case PlatformInfo::EPlatformSDKStatus::Unknown:
				default:
					Platform.IconStyleName = TEXT("Icons.Error");
					break;
				}
#endif
				
				Platform.PlatformGroupName = PlatformInfo.PlatformGroupName;
				if (PlatformInfo.PlatformSubMenu != NAME_None)
				{
					Platform.IniPlatformName = PlatformInfo.PlatformSubMenu.ToString();
				}
			}
			
			if (Platform.IniPlatformName.IsEmpty())
			{
				continue;
			}

			const FString& UBTPlatformName = Platform.UBTPlatformName;
			const auto EqualsUBTPlatformName = [&UBTPlatformName](const FPlatform& PlatformName) -> bool
			{
				return PlatformName.UBTPlatformName.Equals(UBTPlatformName);
			};
			if (Platforms.ContainsByPredicate(EqualsUBTPlatformName))
			{
				continue;
			}
				
			Platforms.Add(Platform);
		}
	}

	bool FPlatformsBase::IsAvailablePlatform(const FString& PlatformName) const
	{
		const FPlatform* FoundPlatform = Platforms.FindByPredicate(
			[&](const FPlatform& Platform) -> bool
			{
				return (Platform.UBTPlatformName == PlatformName);
			}
		);
		if (FoundPlatform == nullptr)
		{
			return false;
		}

		return FoundPlatform->bIsAvailable;
	}
}
