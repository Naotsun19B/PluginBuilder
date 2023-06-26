// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/Types/PackagePluginParams.h"
#include "PluginBuilder/Types/BuildTargets.h"
#include "PluginBuilder/Types/EngineVersions.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"

namespace PluginBuilder
{
	FString FUATBatchFileParams::GetPluginNameInSpecifiedFormat() const
	{
		return (bUseFriendlyName ? PluginFriendlyName : PluginName);
	}

	bool FZipUpPluginParams::IsFormatExpectedByMarketplace() const
	{
		return (!bKeepBinariesFolder && !bKeepUPluginProperties);
	}

	bool FPackagePluginParams::MakeDefault(FPackagePluginParams& Default)
	{
		const auto& Settings = UPluginBuilderSettings::Get();
		if (!Settings.IsReadyToStartPackagePluginTask())
		{
			return false;
		}

		const FBuildTargets::FBuildTarget& BuildTarget = Settings.SelectedBuildTarget.GetValue();
		
		FUATBatchFileParams UATBatchFileParams;
		{
			UATBatchFileParams.PluginName = BuildTarget.GetPluginName();
			UATBatchFileParams.PluginFriendlyName = BuildTarget.GetPluginFriendlyName();
			UATBatchFileParams.bUseFriendlyName = Settings.bUseFriendlyName;
			UATBatchFileParams.PluginVersionName = BuildTarget.GetPluginVersionName();
			UATBatchFileParams.UPluginFile = BuildTarget.GetUPluginFile();
			if (!Settings.bSelectOutputDirectoryManually)
			{
				UATBatchFileParams.OutputDirectoryPath = Settings.OutputDirectoryPath.Path;
			}
			UATBatchFileParams.bStopPackagingProcessImmediately = Settings.bStopPackagingProcessImmediately;
		}
		
		FBuildPluginParams BuildPluginParams;
		{
			BuildPluginParams.TargetPlatforms = Settings.TargetPlatforms;
			BuildPluginParams.bRocket = Settings.bRocket;
			BuildPluginParams.bCreateSubFolder = Settings.bCreateSubFolder;
			BuildPluginParams.bStrictIncludes = Settings.bStrictIncludes;
		}

		FZipUpPluginParams ZipUpPluginParams;
		{
			ZipUpPluginParams.bCanPluginContainContent = BuildTarget.CanPluginContainContent();
			ZipUpPluginParams.bOutputAllZipFilesToSingleFolder = Settings.bOutputAllZipFilesToSingleFolder;
			ZipUpPluginParams.bKeepBinariesFolder = Settings.bKeepBinariesFolder;
			ZipUpPluginParams.bKeepUPluginProperties = Settings.bKeepUPluginProperties;
			ZipUpPluginParams.CompressionLevel = Settings.CompressionLevel;
		}

		Default.EngineVersions = Settings.EngineVersions;
		Default.UATBatchFileParams = UATBatchFileParams;
		Default.BuildPluginParams = BuildPluginParams;
		if (Settings.bZipUp)
		{
			Default.ZipUpPluginParams = ZipUpPluginParams;
		}
#if UE_5_00_OR_LATER
		Default.bShowOnlyLogsFromThisPluginWhenPackageProcessStarts = Settings.bShowOnlyLogsFromThisPluginWhenPackageProcessStarts;
#endif

		return true;
	}

	bool FPackagePluginParams::MakeFromPluginFriendlyName(const FName& PluginFriendlyName, FPackagePluginParams& Params)
	{
		const TArray<FBuildTargets::FBuildTarget>& BuildTargets = FBuildTargets::GetFilteredBuildTargets();
		const FBuildTargets::FBuildTarget* FoundBuildTarget = BuildTargets.FindByPredicate(
			[&PluginFriendlyName](const FBuildTargets::FBuildTarget& BuildTarget) -> bool
			{
				return PluginFriendlyName.IsEqual(*BuildTarget.GetPluginFriendlyName());
			}
		);
		if (FoundBuildTarget == nullptr)
		{
			return false;
		}

		Params.UATBatchFileParams.PluginName = FoundBuildTarget->GetPluginName();
		Params.UATBatchFileParams.PluginFriendlyName = FoundBuildTarget->GetPluginFriendlyName();
		Params.UATBatchFileParams.PluginVersionName = FoundBuildTarget->GetPluginVersionName();
		Params.UATBatchFileParams.UPluginFile = FoundBuildTarget->GetUPluginFile();
		if (Params.ZipUpPluginParams.IsSet())
		{
			Params.ZipUpPluginParams.GetValue().bCanPluginContainContent = FoundBuildTarget->CanPluginContainContent();
		}

		return true;
	}

	bool FPackagePluginParams::IsValid() const
	{
		// Whether the plugin exists.
		{
			auto Predicate = [&](const FBuildTargets::FBuildTarget& BuildTarget) -> bool
			{
				return UATBatchFileParams.PluginFriendlyName.Equals(BuildTarget.GetPluginFriendlyName());
			};
		
			const TArray<FBuildTargets::FBuildTarget>& BuildTargets = FBuildTargets::GetFilteredBuildTargets();
			if (!BuildTargets.ContainsByPredicate(Predicate))
			{
				return false;
			}
		}
		// Whether a valid engine versions are specified.
		{
			const TArray<FString>& InstalledVersionNames = FEngineVersions::GetVersionNames();
			for (const auto& EngineVersion : EngineVersions)
			{
				auto Predicate = [&EngineVersion](const FString& InstalledVersionName) -> bool
				{
					return EngineVersion.Equals(InstalledVersionName);
				};

				if (!InstalledVersionNames.ContainsByPredicate(Predicate))
				{
					return false;
				}
			}
		}

		return true;
	}
}
