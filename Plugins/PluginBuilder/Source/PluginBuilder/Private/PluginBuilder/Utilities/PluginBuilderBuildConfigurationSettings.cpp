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
	
	SelectedBuildTarget = PluginBuilder::FBuildTargets::GetDefaultBuildTarget();
	if (SelectedBuildTargetName.IsEmpty() && SelectedBuildTarget.IsSet())
	{
		SelectedBuildTargetName = SelectedBuildTarget->GetPluginName();
	}
}

FString UPluginBuilderBuildConfigurationSettings::GetSettingsName() const
{
	return TEXT("BuildConfiguration");
}

bool UPluginBuilderBuildConfigurationSettings::IsReadyToStartPackagePluginTask() const
{
	return (SelectedBuildTarget.IsSet() && (EngineVersions.Num() > 0));
}

#undef LOCTEXT_NAMESPACE
