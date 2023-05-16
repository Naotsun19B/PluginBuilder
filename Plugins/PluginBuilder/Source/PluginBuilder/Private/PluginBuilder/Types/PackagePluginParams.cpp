// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/Types/PackagePluginParams.h"
#include "PluginBuilder/Types/BuildTargets.h"
#include "PluginBuilder/Types/EngineVersions.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"

namespace PluginBuilder
{
	bool FPackagePluginParams::MakeDefault(FPackagePluginParams& Default)
	{
		const auto& Settings = UPluginBuilderSettings::Get();
		if (!Settings.IsReadyToStartPackagePluginTask())
		{
			return false;
		}

		Default.EngineVersions = Settings.EngineVersions;

		FBuildPluginParams BuildPluginParams;
		{
			const FBuildTargets::FBuildTarget& BuildTarget = Settings.SelectedBuildTarget.GetValue();
			BuildPluginParams.PluginName = BuildTarget.GetPluginName();
			BuildPluginParams.PluginFriendlyName = BuildTarget.GetPluginFriendlyName();
			BuildPluginParams.PluginVersionName = BuildTarget.GetPluginVersionName();
			BuildPluginParams.bUseFriendlyName = Settings.bUseFriendlyName;
			BuildPluginParams.bCanPluginContainContent = BuildTarget.CanPluginContainContent();
			BuildPluginParams.UPluginFile = BuildTarget.GetUPluginFile();
			BuildPluginParams.TargetPlatforms = Settings.TargetPlatforms;
			BuildPluginParams.bRocket = Settings.bRocket;
			BuildPluginParams.bCreateSubFolder = Settings.bCreateSubFolder;
			BuildPluginParams.bStrictIncludes = Settings.bStrictIncludes;
			BuildPluginParams.bZipUp = Settings.bZipUp;
			BuildPluginParams.bStopPackagingProcessImmediately = Settings.bStopPackagingProcessImmediately;
			BuildPluginParams.bOutputAllZipFilesToSingleFolder = Settings.bOutputAllZipFilesToSingleFolder;
			BuildPluginParams.bKeepBinariesFolder = Settings.bKeepBinariesFolder;
			if (!Settings.bSelectOutputDirectoryManually)
			{
				BuildPluginParams.OutputDirectoryPath = Settings.OutputDirectoryPath.Path;
			}
		}
		
		Default.BuildPluginParams = BuildPluginParams;

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

		Params.BuildPluginParams.PluginName = FoundBuildTarget->GetPluginName();
		Params.BuildPluginParams.PluginFriendlyName = FoundBuildTarget->GetPluginFriendlyName();
		Params.BuildPluginParams.PluginVersionName = FoundBuildTarget->GetPluginVersionName();
		Params.BuildPluginParams.bCanPluginContainContent = FoundBuildTarget->CanPluginContainContent();
		Params.BuildPluginParams.UPluginFile = FoundBuildTarget->GetUPluginFile();

		return true;
	}

	bool FPackagePluginParams::IsValid() const
	{
		// Whether the plugin exists.
		{
			auto Predicate = [&](const FBuildTargets::FBuildTarget& BuildTarget) -> bool
			{
				return BuildPluginParams.PluginFriendlyName.Equals(BuildTarget.GetPluginFriendlyName());
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
