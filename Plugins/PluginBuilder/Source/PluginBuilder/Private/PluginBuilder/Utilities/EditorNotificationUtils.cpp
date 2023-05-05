// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/Utilities/EditorNotificationUtils.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

namespace PluginBuilder
{
	FNotificationHandle::FNotificationHandle()
		: FNotificationHandle(nullptr)
	{
	}

	FNotificationHandle::FNotificationHandle(const TSharedPtr<SNotificationItem>& InNotificationItem)
		: NotificationItem(InNotificationItem)
	{
	}

	bool FNotificationHandle::IsValid() const
	{
		return NotificationItem.IsValid();
	}

	void FNotificationHandle::SetText(const FText& Text)
	{
		if (IsValid())
		{
			NotificationItem->SetText(Text);
		}
	}

	void FNotificationHandle::Fadeout()
	{
		if (IsValid())
		{
			NotificationItem->Fadeout();
			NotificationItem = nullptr;
		}
	}

	FNotificationInteraction::FNotificationInteraction(
		EInteractionType InType,
		const FText& InText,
		const FText& InTooltip,
		const FSimpleDelegate& InCallback
	)
		: Type(InType), Text(InText), Tooltip(InTooltip), Callback(InCallback)
	{
	}
	
	FNotificationInteraction::FNotificationInteraction()
		: FNotificationInteraction(EInteractionType::Hyperlink, {}, {}, {})
	{
	}

	FNotificationInteraction::FNotificationInteraction(const FText& InText, const FSimpleDelegate& InCallback)
		: FNotificationInteraction(EInteractionType::Hyperlink, InText, {}, InCallback)
	{
	}

	FNotificationInteraction::FNotificationInteraction(const FText& InText, const FText& InTooltip, const FSimpleDelegate& InCallback)
		: FNotificationInteraction(EInteractionType::Button, InText, InTooltip, InCallback)
	{
	}
	
	constexpr FEditorNotificationUtils::ECompletionState FEditorNotificationUtils::CS_Pending	= SNotificationItem::ECompletionState::CS_Pending;
	constexpr FEditorNotificationUtils::ECompletionState FEditorNotificationUtils::CS_Success	= SNotificationItem::ECompletionState::CS_Success;
	constexpr FEditorNotificationUtils::ECompletionState FEditorNotificationUtils::CS_Fail		= SNotificationItem::ECompletionState::CS_Fail;
	
	FNotificationHandle FEditorNotificationUtils::ShowNotification(
		const FText& NotificationText,
		ECompletionState CompletionState,
		const float ExpireDuration /* = 4.f */,
		const TArray<FNotificationInteraction>& Interactions /* = TArray<FNotificationInteraction>() */
	)
	{
		FNotificationInfo NotificationInfo(NotificationText);
		NotificationInfo.bFireAndForget = (ExpireDuration > 0.f);
		if (NotificationInfo.bFireAndForget)
		{
			NotificationInfo.ExpireDuration = ExpireDuration;
		}
		NotificationInfo.bUseLargeFont = true;

		const auto StateEnum = static_cast<SNotificationItem::ECompletionState>(CompletionState);

		for (const auto& Interaction : Interactions)
		{
			switch (Interaction.Type)
			{
			case FNotificationInteraction::EInteractionType::Hyperlink:
				{
					NotificationInfo.HyperlinkText = Interaction.Text;
					NotificationInfo.Hyperlink = Interaction.Callback;
					break;
				}
			case FNotificationInteraction::EInteractionType::Button:
				{
					const FNotificationButtonInfo ButtonInfo(Interaction.Text, Interaction.Tooltip, Interaction.Callback, StateEnum);
					NotificationInfo.ButtonDetails.Add(ButtonInfo);
					break;
				}
			default: break;
			}
		}

		const TSharedPtr<SNotificationItem> NotificationItem = FSlateNotificationManager::Get().AddNotification(NotificationInfo);
		if (NotificationItem.IsValid())
		{
			NotificationItem->SetCompletionState(StateEnum);
		}
	
		return FNotificationHandle(NotificationItem);
	}
}
