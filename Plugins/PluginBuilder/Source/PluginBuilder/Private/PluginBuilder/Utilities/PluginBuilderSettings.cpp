// Copyright 2022-2026 Naotsun. All Rights Reserved.

#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "Modules/ModuleManager.h"
#include "ISettingsModule.h"
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

bool UPluginBuilderSettings::ShouldRegisterToSettingsPanel() const
{
	return true;
}

FName UPluginBuilderSettings::GetSectionName() const
{
	const FString& SettingsName = GetSettingsName();
	if (SettingsName.IsEmpty())
	{
		return PluginBuilder::Global::PluginName;
	}
	
	return *(PluginBuilder::Global::PluginName.ToString() + SettingsName);
}

FText UPluginBuilderSettings::GetDisplayName() const
{
	const FText& PluginDisplayName = FText::FromString(FName::NameToDisplayString(PluginBuilder::Global::PluginName.ToString(), false));
	
	const FString& SettingsName = GetSettingsName();
	if (SettingsName.IsEmpty())
	{
		return PluginDisplayName;
	}
	
	return FText::Format(
		LOCTEXT("DisplayNameFormat", "{0} - {1}"),
		PluginDisplayName,
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

FString UPluginBuilderSettings::GetSettingsName() const
{
	return {};
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

		if (Settings->ShouldRegisterToSettingsPanel())
		{
			SettingsModule->RegisterSettings(
				PluginBuilder::Settings::ContainerName,
				PluginBuilder::Settings::CategoryName,
				Settings->GetSectionName(),
				Settings->GetDisplayName(),
				Settings->GetTooltipText(),
				Settings
			);
		}
		
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
		if (Settings->ShouldRegisterToSettingsPanel())
		{
			SettingsModule->UnregisterSettings(
				PluginBuilder::Settings::ContainerName,
				PluginBuilder::Settings::CategoryName,
				Settings->GetSectionName()
			);
		}

		Settings->PreSaveConfig();
		
		const UClass* SettingsClass = Settings->GetClass();
		check(IsValid(SettingsClass));
		if (SettingsClass->HasAnyClassFlags(CLASS_DefaultConfig))
		{
#if UE_5_00_OR_LATER
			Settings->TryUpdateDefaultConfigFile();
#else
			Settings->UpdateDefaultConfigFile();
#endif
		}
		else if (SettingsClass->HasAnyClassFlags(CLASS_GlobalUserConfig))
		{
			Settings->UpdateGlobalUserConfigFile();
		}
		else if (SettingsClass->HasAnyClassFlags(CLASS_ProjectUserConfig))
		{
			Settings->UpdateProjectUserConfigFile();
		}
		else
		{
			const FString& ConfigFilename = GetConfigFilename(Settings);
#if UE_5_00_OR_LATER
			Settings->TryUpdateDefaultConfigFile(ConfigFilename);
#else
			Settings->UpdateDefaultConfigFile(ConfigFilename);
#endif
		}

		Settings->RemoveFromRoot();
	}
}

TArray<UPluginBuilderSettings*> UPluginBuilderSettings::AllSettings;

#undef LOCTEXT_NAMESPACE
