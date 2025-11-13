// Copyright 2022-2025 Naotsun. All Rights Reserved.

#include "PluginBuilder/Utilities/PluginBuilderEditorSettings.h"
#include "Misc/Paths.h"

UPluginBuilderEditorSettings::UPluginBuilderEditorSettings()
	: bSearchOnlyEnabled(true)
	, bContainsProjectPlugins(true)
	, bContainsEnginePlugins(false)
	, bSelectOutputDirectoryManually(false)
	, bOutputToBuildDirectoryOfEachProject(false)
	, bUseFriendlyName(true)
	, bShowOnlyLogsFromThisPluginWhenPackageProcessStarts(false)
	, bStopPackagingProcessImmediately(false)
{
}

void UPluginBuilderEditorSettings::PostInitProperties()
{
	UObject::PostInitProperties();

	ResetOutputDirectoryPath();
}

void UPluginBuilderEditorSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.MemberProperty == nullptr)
	{
		return;
	}

	if (PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UPluginBuilderEditorSettings, OutputDirectoryPath) ||
		PropertyChangedEvent.MemberProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UPluginBuilderEditorSettings, bOutputToBuildDirectoryOfEachProject))
	{
		ResetOutputDirectoryPath();
	}
}

bool UPluginBuilderEditorSettings::CanEditChange(const FProperty* InProperty) const
{
	if (!UObject::CanEditChange(InProperty) || (InProperty == nullptr))
	{
		return false;
	}

#if !UE_5_01_OR_LATER
	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UPluginBuilderEditorSettings, bShowOnlyLogsFromThisPluginWhenPackageProcessStarts))
	{
		return false;
	}
#endif

	return true;
}

void UPluginBuilderEditorSettings::ResetOutputDirectoryPath()
{
	if (OutputDirectoryPath.Path.IsEmpty() || bOutputToBuildDirectoryOfEachProject)
	{
		OutputDirectoryPath.Path = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("Build"));
	}
}

#undef LOCTEXT_NAMESPACE
