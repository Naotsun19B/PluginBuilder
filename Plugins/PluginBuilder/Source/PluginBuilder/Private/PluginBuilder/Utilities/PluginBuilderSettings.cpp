// Copyright 2022-2025 Naotsun. All Rights Reserved.

#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "Modules/ModuleManager.h"
#include "ISettingsModule.h"
#include "ISettingsContainer.h"
#include "ISettingsCategory.h"
#include "ISettingsSection.h"
#include "Misc/CoreDelegates.h"
#include "UObject/UObjectIterator.h"

#define LOCTEXT_NAMESPACE "PluginBuilderSettings"

namespace PluginBuilder
{
	namespace Settings
	{
		static const FName ContainerName	= TEXT("Editor");
		static const FName CategoryName		= TEXT("Plugins");

		ISettingsModule* GetSettingsModule()
		{
			return FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings"));
		}
	}
}

void UPluginBuilderSettings::Register()
{
	// Since this plugin's LoadingPhase is PostEngineInit, it is called directly.
	HandleOnPostEngineInit();
	FCoreDelegates::OnEnginePreExit.AddStatic(&UPluginBuilderSettings::HandleOnEnginePreExit);
}

FName UPluginBuilderSettings::GetSectionName() const
{
	return *(PluginBuilder::Global::PluginName.ToString() + GetSettingsName());
}

FText UPluginBuilderSettings::GetDisplayName() const
{
	return FText::Format(
		LOCTEXT("DisplayNameFormat", "{0} - {1}"),
		FText::FromString(FName::NameToDisplayString(PluginBuilder::Global::PluginName.ToString(), false)),
		FText::FromString(GetSettingsName())
	);
}

FText UPluginBuilderSettings::GetTooltipText() const
{
	const UClass* SettingsClass = GetClass();
	check(IsValid(SettingsClass));
	return SettingsClass->GetToolTipText();
}

void UPluginBuilderSettings::OpenSettings(const FName& SectionName)
{
	if (ISettingsModule* SettingsModule = PluginBuilder::Settings::GetSettingsModule())
	{
		SettingsModule->ShowViewer(
			PluginBuilder::Settings::ContainerName,
			PluginBuilder::Settings::CategoryName,
			SectionName
		);
	}
}

void UPluginBuilderSettings::HandleOnPostEngineInit()
{
	ISettingsModule* SettingsModule = PluginBuilder::Settings::GetSettingsModule();
	if (SettingsModule == nullptr)
	{
		return;
	}
	
	for (auto* Settings : TObjectRange<UPluginBuilderSettings>(RF_NoFlags))
	{
		if (!IsValid(Settings))
		{
			continue;
		}

		const UClass* SettingsClass = Settings->GetClass();
		if (!IsValid(SettingsClass))
		{
			continue;
		}
		if (SettingsClass->HasAnyClassFlags(CLASS_Abstract))
		{
			continue;
		}
		if (!Settings->IsTemplate())
		{
			continue;
		}
		
		SettingsModule->RegisterSettings(
			PluginBuilder::Settings::ContainerName,
			PluginBuilder::Settings::CategoryName,
			Settings->GetSectionName(),
			Settings->GetDisplayName(),
			Settings->GetTooltipText(),
			Settings
		);

		Settings->AddToRoot();
		AllSettings.Add(Settings);
	}
}

void UPluginBuilderSettings::HandleOnEnginePreExit()
{
	ISettingsModule* SettingsModule = PluginBuilder::Settings::GetSettingsModule();
	if (SettingsModule == nullptr)
	{
		return;
	}

	for (auto* Settings : AllSettings)
	{
		Settings->PreSaveConfig();
		
		const TSharedPtr<ISettingsContainer> Container = SettingsModule->GetContainer(PluginBuilder::Settings::ContainerName);
		check(Container.IsValid());
		const TSharedPtr<ISettingsCategory> Category = Container->GetCategory(PluginBuilder::Settings::CategoryName);
		check(Category.IsValid());
		const TSharedPtr<ISettingsSection> Section = Category->GetSection(Settings->GetSectionName());
		check(Section.IsValid());
		Section->Save();
		
		SettingsModule->UnregisterSettings(
			PluginBuilder::Settings::ContainerName,
			PluginBuilder::Settings::CategoryName,
			Settings->GetSectionName()
		);

		Settings->RemoveFromRoot();
	}
}

TArray<UPluginBuilderSettings*> UPluginBuilderSettings::AllSettings;

#undef LOCTEXT_NAMESPACE
