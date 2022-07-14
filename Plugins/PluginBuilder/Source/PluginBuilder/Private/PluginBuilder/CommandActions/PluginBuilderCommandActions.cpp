// Copyright 2022 Naotsun. All Rights Reserved.

#include "PluginBuilder/CommandActions/PluginBuilderCommandActions.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "PluginBuilder/Tasks/PackagePluginTask.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "PluginBuilderCommandActions"

namespace PluginBuilder
{
	void FPluginBuilderCommandActions::BuildPlugin()
	{
		if (ActivePackagePluginTask.IsValid())
		{
			return;
		}
		
		ActivePackagePluginTask = FPackagePluginTask::CreateTask();
		check(ActivePackagePluginTask.IsValid());

		FText NotificationText;
		{
			const FPackagePluginArgs& Args = ActivePackagePluginTask->GetArgs();
			NotificationText = FText::Format(
				LOCTEXT("NotificationTextFormat", "Packaging {0} ({1})..."),
				FText::FromString(Args.PluginName),
				FText::FromString(Args.PluginVersionName)
			);
		}
		FNotificationInfo NotificationInfo(NotificationText);
		NotificationInfo.bFireAndForget = false;
		NotificationInfo.bUseLargeFont = true;

		const FNotificationButtonInfo ButtonInfo(
			LOCTEXT("CancelButtonLabel", "Cancel Packaging"),
			LOCTEXT("CancelButtonTooltip", "Cancels the plugin packaging process."),
			FSimpleDelegate::CreateStatic(&FPluginBuilderCommandActions::HandleOnCancelPackagingButtonPressed),
			SNotificationItem::ECompletionState::CS_Pending
		);
		NotificationInfo.ButtonDetails.Add(ButtonInfo);

		const TSharedPtr<SNotificationItem> NotificationItem = FSlateNotificationManager::Get().AddNotification(NotificationInfo);
		if (NotificationItem.IsValid())
		{
			NotificationItem->SetCompletionState(SNotificationItem::ECompletionState::CS_Pending);
		}
		
		ActivePackagePluginTask->StartProcess(
			FPackagePluginTask::FOnTaskFinished::CreateStatic(
				&FPluginBuilderCommandActions::HandleOnTaskFinished, NotificationItem
			)
		);
	}

	bool FPluginBuilderCommandActions::CanBuildPlugin()
	{
		const auto& Settings = UPluginBuilderSettings::Get();
		
		return (
			!ActivePackagePluginTask.IsValid() &&
			Settings.SelectedBuildTarget.IsSet() &&
			(Settings.EngineVersions.Num() > 0)
		);
	}

	void FPluginBuilderCommandActions::OpenBuildSettings()
	{
		UPluginBuilderSettings::OpenSettings();
	}

	void FPluginBuilderCommandActions::HandleOnCancelPackagingButtonPressed()
	{
		if (ActivePackagePluginTask.IsValid())
		{
			ActivePackagePluginTask->RequestCancelProcess();
		}
	}

	void FPluginBuilderCommandActions::HandleOnTaskFinished(
		const bool bWasSuccessful,
		const bool bWasCanceled,
		TSharedPtr<SNotificationItem> PendingNotificationItem
	)
	{
		ActivePackagePluginTask.Reset();
		
		if (PendingNotificationItem.IsValid())
		{
			PendingNotificationItem->Fadeout();
		}

		FText NotificationText;
		if (bWasCanceled)
		{
			NotificationText = LOCTEXT("PackageCanceled", "Plugin packaging has been cancelled.");
		}
		else if (bWasSuccessful)
		{
			NotificationText = LOCTEXT("PackageSucceeded", "Plugin packaging has completed successfully.");
		}
		else
		{
			NotificationText = LOCTEXT("PackageFailed", "Failed to package the plugin.");
		}
		
		FNotificationInfo NotificationInfo(NotificationText);
		NotificationInfo.bFireAndForget = true;
		NotificationInfo.ExpireDuration = 4.f;
		NotificationInfo.bUseLargeFont = true;
		
		const TSharedPtr<SNotificationItem> NotificationItem = FSlateNotificationManager::Get().AddNotification(NotificationInfo);
		if (NotificationItem.IsValid())
		{
			NotificationItem->SetCompletionState(
				(bWasSuccessful && !bWasCanceled) ?
				SNotificationItem::ECompletionState::CS_Success :
				SNotificationItem::ECompletionState::CS_Fail
			);
		}
	}

	TSharedPtr<FPackagePluginTask> FPluginBuilderCommandActions::ActivePackagePluginTask = nullptr;
}

#undef LOCTEXT_NAMESPACE
