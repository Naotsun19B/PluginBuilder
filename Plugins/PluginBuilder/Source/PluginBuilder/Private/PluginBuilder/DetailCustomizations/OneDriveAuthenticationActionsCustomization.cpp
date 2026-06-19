// Copyright 2022-2026 Naotsun. All Rights Reserved.

#include "PluginBuilder/DetailCustomizations/OneDriveAuthenticationActionsCustomization.h"
#include "PluginBuilder/Types/OneDriveAuthenticationActions.h"
#include "PluginBuilder/Utilities/OneDriveSettings.h"
#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"

#define LOCTEXT_NAMESPACE "OneDriveAuthenticationActionsCustomization"

namespace PluginBuilder
{
	void FOneDriveAuthenticationActionsCustomization::Register()
	{
		CachedPropertyTypeName = GetNameSafe(FOneDriveAuthenticationActions::StaticStruct());

		auto& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
		PropertyEditorModule.RegisterCustomPropertyTypeLayout(
			*CachedPropertyTypeName,
			FOnGetPropertyTypeCustomizationInstance::CreateStatic(&MakeInstance)
		);
	}

	void FOneDriveAuthenticationActionsCustomization::Unregister()
	{
		auto& PropertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));
		PropertyEditorModule.UnregisterCustomPropertyTypeLayout(
			*CachedPropertyTypeName
		);
	}

	TSharedRef<IPropertyTypeCustomization> FOneDriveAuthenticationActionsCustomization::MakeInstance()
	{
		return MakeShared<FOneDriveAuthenticationActionsCustomization>();
	}

	void FOneDriveAuthenticationActionsCustomization::CustomizeHeader(
		TSharedRef<IPropertyHandle> PropertyHandle,
		FDetailWidgetRow& HeaderRow,
		IPropertyTypeCustomizationUtils& CustomizationUtils
	)
	{
		HeaderRow
		.NameContent()
		[
			SNew(STextBlock)
			.Text(LOCTEXT("AuthenticationLabel", "Authentication"))
		]
		.ValueContent()
		.HAlign(HAlign_Fill)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(FMargin(0.f, 0.f, 4.f, 0.f))
			[
				SNew(SButton)
				.Text(LOCTEXT("SignIn", "Sign In to OneDrive"))
				.OnClicked(this, &FOneDriveAuthenticationActionsCustomization::OnSignInClicked)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SButton)
				.Text(LOCTEXT("SignOut", "Sign Out"))
				.OnClicked(this, &FOneDriveAuthenticationActionsCustomization::OnSignOutClicked)
			]
		];
	}

	void FOneDriveAuthenticationActionsCustomization::CustomizeChildren(
		TSharedRef<IPropertyHandle> PropertyHandle,
		IDetailChildrenBuilder& ChildBuilder,
		IPropertyTypeCustomizationUtils& CustomizationUtils
	)
	{
	}

	FReply FOneDriveAuthenticationActionsCustomization::OnSignInClicked()
	{
		UOneDriveSettings* Settings = GetMutableDefault<UOneDriveSettings>();
		if (IsValid(Settings))
		{
			Settings->SignIn();
		}
		return FReply::Handled();
	}

	FReply FOneDriveAuthenticationActionsCustomization::OnSignOutClicked()
	{
		UOneDriveSettings* Settings = GetMutableDefault<UOneDriveSettings>();
		if (IsValid(Settings))
		{
			Settings->SignOut();
		}
		return FReply::Handled();
	}

	FString FOneDriveAuthenticationActionsCustomization::CachedPropertyTypeName;
}

#undef LOCTEXT_NAMESPACE
