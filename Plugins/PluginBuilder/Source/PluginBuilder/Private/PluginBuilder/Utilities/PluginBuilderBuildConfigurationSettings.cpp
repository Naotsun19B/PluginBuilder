// Copyright 2022-2025 Naotsun. All Rights Reserved.

#include "PluginBuilder/Utilities/PluginBuilderBuildConfigurationSettings.h"

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
	, CompressionLevel(5)
{
}

void UPluginBuilderBuildConfigurationSettings::PostInitProperties()
{
	UObject::PostInitProperties();

	if (SelectedBuildTargetName.IsEmpty())
	{
		SelectedBuildTarget = PluginBuilder::FBuildTargets::GetDefaultBuildTarget();
		SelectedBuildTargetName = SelectedBuildTarget->GetPluginName();
	}

	EngineVersionsString.ParseIntoArray(EngineVersions, TEXT(","));
	HostPlatformsString.ParseIntoArray(HostPlatforms, TEXT(","));
	TargetPlatformsString.ParseIntoArray(TargetPlatforms, TEXT(","));
}

FString UPluginBuilderBuildConfigurationSettings::GetSettingsName() const
{
	return TEXT("BuildConfiguration");
}

void UPluginBuilderBuildConfigurationSettings::PreSaveConfig()
{
	Super::PreSaveConfig();

	if (SelectedBuildTarget.IsSet())
	{
		SelectedBuildTargetName = SelectedBuildTarget->GetPluginFriendlyName();
	}
	
	EngineVersionsString = FString::Join(EngineVersions, TEXT(","));
	HostPlatformsString = FString::Join(HostPlatforms, TEXT(","));
	TargetPlatformsString = FString::Join(TargetPlatforms, TEXT(","));
}

bool UPluginBuilderBuildConfigurationSettings::IsReadyToStartPackagePluginTask() const
{
	return (SelectedBuildTarget.IsSet() && (EngineVersions.Num() > 0));
}

#undef LOCTEXT_NAMESPACE
