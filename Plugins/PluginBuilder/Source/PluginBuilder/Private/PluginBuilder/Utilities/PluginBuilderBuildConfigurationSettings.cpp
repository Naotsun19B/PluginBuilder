// Copyright 2022-2026 Naotsun. All Rights Reserved.

#include "PluginBuilder/Utilities/PluginBuilderBuildConfigurationSettings.h"
#include "PluginBuilder/Types/HostPlatforms.h"
#include "PluginBuilder/Types/TargetPlatforms.h"

UPluginBuilderBuildConfigurationSettings::UPluginBuilderBuildConfigurationSettings()
	: bNoHostPlatform(false)
	, bRocket(true)
	, bCreateSubFolder(false)
	, bStrictIncludes(false)
	, bUnversioned(false)
	, bZipUp(true)
	, bOutputAllZipFilesToSingleFolder(false)
	, bKeepBinariesFolder(false)
	, bKeepUPluginProperties(false)
	, bAppendEngineVersionToZipFileName(false)
	, CompressionLevel(5)
{
}

void UPluginBuilderBuildConfigurationSettings::PostInitProperties()
{
	UObject::PostInitProperties();

	if (SelectedBuildTargetName.IsEmpty())
	{
		SelectedBuildTarget = PluginBuilder::FBuildTargets::GetDefaultBuildTarget();
		if (SelectedBuildTarget.IsSet())
		{
			SelectedBuildTargetName = SelectedBuildTarget->GetPluginName();
		}
	}
	else
	{
		SelectedBuildTarget = PluginBuilder::FBuildTargets::LoadBuildTarget(SelectedBuildTargetName);
	}

	EngineVersionsString.ParseIntoArray(EngineVersions, TEXT(","));
	HostPlatformsString.ParseIntoArray(HostPlatforms, TEXT(","));
	TargetPlatformsString.ParseIntoArray(TargetPlatforms, TEXT(","));
}

bool UPluginBuilderBuildConfigurationSettings::ShouldRegisterToSettingsPanel() const
{
	return false;
}

void UPluginBuilderBuildConfigurationSettings::PreSaveConfig()
{
	Super::PreSaveConfig();

	if (SelectedBuildTarget.IsSet())
	{
		SelectedBuildTargetName = SelectedBuildTarget->GetPluginFriendlyName();
	}

	HostPlatforms.RemoveAll(
		[](const FString& HostPlatformName) -> bool
		{
			return !PluginBuilder::FHostPlatforms::IsAvailableHostPlatform(HostPlatformName);
		}
	);
	TargetPlatforms.RemoveAll(
		[](const FString& TargetPlatformName) -> bool
		{
			return !PluginBuilder::FTargetPlatforms::IsAvailableTargetPlatform(TargetPlatformName);
		}
	);
	
	EngineVersionsString = FString::Join(EngineVersions, TEXT(","));
	HostPlatformsString = FString::Join(HostPlatforms, TEXT(","));
	TargetPlatformsString = FString::Join(TargetPlatforms, TEXT(","));
}

bool UPluginBuilderBuildConfigurationSettings::IsReadyToStartPackagePluginTask() const
{
	return (SelectedBuildTarget.IsSet() && (EngineVersions.Num() > 0));
}

#undef LOCTEXT_NAMESPACE
