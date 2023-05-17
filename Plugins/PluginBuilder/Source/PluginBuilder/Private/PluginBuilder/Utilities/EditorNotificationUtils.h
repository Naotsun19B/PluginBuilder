// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class SNotificationItem;

namespace PluginBuilder
{
	/**
	 * A structure of notification item handles that can indirectly manipulate SNotificationItem.
	 */
	struct PLUGINBUILDER_API FNotificationHandle
	{
	public:
		// Constructor.
		FNotificationHandle();
		explicit FNotificationHandle(const TSharedPtr<SNotificationItem>& InNotificationItem);

		// Returns whether editor notifications pointed to by this handle are enabled.
		bool IsValid() const;

		// Changes the text of the currently displayed editor notification.
		void SetText(const FText& Text);
		
		// Begins the fade out,
		void Fadeout();
			
	private:
		// The indirectly operated SNotificationItem.
		TSharedPtr<SNotificationItem> NotificationItem;
	};
	
	/**
	 * A structure for adding buttons, hyperlinks, etc. to notifications.
	 */
	struct PLUGINBUILDER_API FNotificationInteraction
	{
	public:
		// An enum class that defines type of things that users of editor notifications can interact with.
		enum class EInteractionType : uint8 
		{
			Hyperlink,
			Button,
		};
		EInteractionType Type;

		// The text displayed by the button or hyperlink.
		FText Text;

		// The tooltip text that appears when you hover over a button.
		FText Tooltip;

		// The callback that is called when a button or hyperlink is pressed.
		FSimpleDelegate Callback;

	public:
		// Constructor.
		FNotificationInteraction(
			EInteractionType InType,
			const FText& InText,
			const FText& InTooltip,
			const FSimpleDelegate& InCallback
		);
		FNotificationInteraction();
		FNotificationInteraction(const FText& InText, const FSimpleDelegate& InCallback);
		FNotificationInteraction(const FText& InText, const FText& InTooltip, const FSimpleDelegate& InCallback);
	};

	/**
	 * A utility class that defines functions related to editor notifications.
	 */
	class PLUGINBUILDER_API FEditorNotificationUtils
	{
	public:
		// Define notification types so don't need to include "SNotificationList.h".
		using ECompletionState = int32;
		static const ECompletionState CS_Pending;
		static const ECompletionState CS_Success;
		static const ECompletionState CS_Fail;

	public:
		// Shows notifications at the bottom right of the editor (When Expire Duration is 0 or less, you need to call the fade process manually.).
		static FNotificationHandle ShowNotification(
			const FText& NotificationText, 
			ECompletionState CompletionState, 
			float ExpireDuration = 4.f, 
			const TArray<FNotificationInteraction>& Interactions = TArray<FNotificationInteraction>()
		);
	};
}
