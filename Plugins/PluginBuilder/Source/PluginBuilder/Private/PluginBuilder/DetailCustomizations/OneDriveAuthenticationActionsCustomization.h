// Copyright 2022-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"
#include "Input/Reply.h"

namespace PluginBuilder
{
	/**
	 * Details panel customization for FOneDriveAuthenticationActions.
	 * Renders Sign In / Sign Out buttons inside the OneDrive settings.
	 */
	class FOneDriveAuthenticationActionsCustomization : public IPropertyTypeCustomization
	{
	public:
		// Registers this customization with FPropertyEditorModule.
		static void Register();

		// Unregisters this customization from FPropertyEditorModule.
		static void Unregister();

		// Returns an instance of this customization.
		static TSharedRef<IPropertyTypeCustomization> MakeInstance();

		// IPropertyTypeCustomization interface.
		virtual void CustomizeHeader(
			TSharedRef<IPropertyHandle> PropertyHandle,
			FDetailWidgetRow& HeaderRow,
			IPropertyTypeCustomizationUtils& CustomizationUtils
		) override;

		virtual void CustomizeChildren(
			TSharedRef<IPropertyHandle> PropertyHandle,
			IDetailChildrenBuilder& ChildBuilder,
			IPropertyTypeCustomizationUtils& CustomizationUtils
		) override;
		// End of IPropertyTypeCustomization interface.

	private:
		// Called when the Sign In button is clicked.
		FReply OnSignInClicked();

		// Called when the Sign Out button is clicked.
		FReply OnSignOutClicked();

	private:
		// The cache of type names for properties that utilize this details panel.
		static FString CachedPropertyTypeName;
	};
}
