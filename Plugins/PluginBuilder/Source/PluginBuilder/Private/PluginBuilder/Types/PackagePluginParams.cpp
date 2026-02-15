// Copyright 2022-2026 Naotsun. All Rights Reserved.

#include "PluginBuilder/Types/PackagePluginParams.h"
#include "PluginBuilder/Types/BuildTargets.h"
#include "PluginBuilder/Types/EngineVersions.h"
#include "PluginBuilder/Utilities/PluginBuilderEditorSettings.h"
#include "PluginBuilder/Utilities/PluginBuilderBuildConfigurationSettings.h"

namespace PluginBuilder
{
	FString FUATBatchFileParams::GetPluginNameInSpecifiedFormat() const
	{
		return (bUseFriendlyName ? PluginFriendlyName : PluginName);
	}

	bool FBuildPluginParams::IsFormatExpectedByMarketplace() const
	{
		return !bUnversioned;
	}

	bool FZipUpPluginParams::IsFormatExpectedByMarketplace() const
	{
		return (!bKeepBinariesFolder && !bKeepUPluginProperties);
	}

	bool FPackagePluginParams::MakeDefault(FPackagePluginParams& Default)
	{
		const auto& EditorSettings = GetSettings<UPluginBuilderEditorSettings>();
		const auto& BuildConfigurationSettings = GetSettings<UPluginBuilderBuildConfigurationSettings>();
		if (!BuildConfigurationSettings.IsReadyToStartPackagePluginTask())
		{
			return false;
		}

		const FBuildTargets::FBuildTarget& BuildTarget = BuildConfigurationSettings.SelectedBuildTarget.GetValue();
		
		FUATBatchFileParams UATBatchFileParams;
		{
			UATBatchFileParams.PluginName = BuildTarget.GetPluginName();
			UATBatchFileParams.PluginFriendlyName = BuildTarget.GetPluginFriendlyName();
			UATBatchFileParams.bUseFriendlyName = EditorSettings.bUseFriendlyName;
			UATBatchFileParams.PluginVersionName = BuildTarget.GetPluginVersionName();
			UATBatchFileParams.UPluginFile = BuildTarget.GetUPluginFile();
			if (!EditorSettings.bSelectOutputDirectoryManually)
			{
				UATBatchFileParams.OutputDirectoryPath = EditorSettings.OutputDirectoryPath.Path;
			}
			UATBatchFileParams.bStopPackagingProcessImmediately = EditorSettings.bStopPackagingProcessImmediately;
		}
		
		FBuildPluginParams BuildPluginParams;
		{
			BuildPluginParams.bNoHostPlatform = BuildConfigurationSettings.bNoHostPlatform;
			BuildPluginParams.HostPlatforms = BuildConfigurationSettings.HostPlatforms;
			BuildPluginParams.TargetPlatforms = BuildConfigurationSettings.TargetPlatforms;
			BuildPluginParams.bRocket = BuildConfigurationSettings.bRocket;
			BuildPluginParams.bCreateSubFolder = BuildConfigurationSettings.bCreateSubFolder;
			BuildPluginParams.bStrictIncludes = BuildConfigurationSettings.bStrictIncludes;
			BuildPluginParams.bUnversioned = BuildConfigurationSettings.bUnversioned;
		}

		FZipUpPluginParams ZipUpPluginParams;
		{
			ZipUpPluginParams.bCanPluginContainContent = BuildTarget.CanPluginContainContent();
			ZipUpPluginParams.bOutputAllZipFilesToSingleFolder = BuildConfigurationSettings.bOutputAllZipFilesToSingleFolder;
			ZipUpPluginParams.bKeepBinariesFolder = BuildConfigurationSettings.bKeepBinariesFolder;
			ZipUpPluginParams.bKeepUPluginProperties = BuildConfigurationSettings.bKeepUPluginProperties;
			ZipUpPluginParams.bAppendEngineVersionToZipFileName = BuildConfigurationSettings.bAppendEngineVersionToZipFileName;
			ZipUpPluginParams.CompressionLevel = BuildConfigurationSettings.CompressionLevel;
		}

		Default.EngineVersions = BuildConfigurationSettings.EngineVersions;
		Default.UATBatchFileParams = UATBatchFileParams;
		Default.BuildPluginParams = BuildPluginParams;
		if (BuildConfigurationSettings.bZipUp)
		{
			Default.ZipUpPluginParams = ZipUpPluginParams;
		}
#if UE_5_00_OR_LATER
		Default.bShowOnlyLogsFromThisPluginWhenPackageProcessStarts = EditorSettings.bShowOnlyLogsFromThisPluginWhenPackageProcessStarts;
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

	bool FPackagePluginParams::IsFormatExpectedByMarketplace() const
	{
		bool bIsFormatExpectedByMarketplace = true;

		if (BuildPluginParams.IsSet())
		{
			if (!BuildPluginParams->IsFormatExpectedByMarketplace())
			{
				bIsFormatExpectedByMarketplace = false;
			}
		}
		if (ZipUpPluginParams.IsSet())
		{
			if (!ZipUpPluginParams->IsFormatExpectedByMarketplace())
			{
				bIsFormatExpectedByMarketplace = false;
			}
		}

		return bIsFormatExpectedByMarketplace;
	}
}
