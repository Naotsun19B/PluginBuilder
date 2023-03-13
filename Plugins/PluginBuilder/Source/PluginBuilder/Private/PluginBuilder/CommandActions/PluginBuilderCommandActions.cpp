// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/CommandActions/PluginBuilderCommandActions.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "PluginBuilder/Tasks/PackagePluginTask.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Docking/TabManager.h"

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

		NotificationInfo.HyperlinkText = LOCTEXT("ShowOutputLogLinkText", "Show Output Log");
		NotificationInfo.Hyperlink.BindLambda(
			[]()
			{
				static const FName OutputLogTabId = TEXT("OutputLog");
				
				const TSharedRef<FGlobalTabmanager> GlobalTabManager = FGlobalTabmanager::Get();
#if UE_4_26_OR_LATER
				GlobalTabManager->TryInvokeTab(OutputLogTabId);
#else
				GlobalTabManager->InvokeTab(OutputLogTabId);
#endif
			}
		);
		
		const FNotificationButtonInfo ButtonInfo(
			LOCTEXT("CancelButtonLabel", "Cancel"),
			LOCTEXT("CancelButtonTooltip", "Cancels the plugin packaging process."),
			FSimpleDelegate::CreateStatic(&FPluginBuilderCommandActions::HandleOnCancelPackagingButtonPressed),
			SNotificationItem::ECompletionState::CS_Pending
		);
		NotificationInfo.ButtonDetails.Add(ButtonInfo);

		PendingNotificationItem = FSlateNotificationManager::Get().AddNotification(NotificationInfo);
		if (PendingNotificationItem.IsValid())
		{
			PendingNotificationItem->SetCompletionState(SNotificationItem::ECompletionState::CS_Pending);
		}
		
		ActivePackagePluginTask->StartProcess(FPackagePluginTask::FOnTaskFinished::CreateStatic(&FPluginBuilderCommandActions::HandleOnTaskFinished));

		if (IsValid(GEditor))
		{
			GEditor->PlayEditorSound(TEXT("/Engine/EditorSounds/Notifications/CompileStart_Cue.CompileStart_Cue"));
		}
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

		if (PendingNotificationItem.IsValid())
		{
			const FText NotificationText = LOCTEXT("CancelPackagingNotificationText", "Waiting for packaging cancellation...");
			PendingNotificationItem->SetText(NotificationText);
		}
	}

	void FPluginBuilderCommandActions::HandleOnTaskFinished(const bool bWasSuccessful, const bool bWasCanceled)
	{
		ActivePackagePluginTask.Reset();
		
		if (PendingNotificationItem.IsValid())
		{
			PendingNotificationItem->Fadeout();
			PendingNotificationItem.Reset();
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
		if (IsValid(GEditor))
		{
			if (bWasSuccessful && !bWasCanceled)
			{
				GEditor->PlayEditorSound(TEXT("/Engine/EditorSounds/Notifications/CompileSuccess_Cue.CompileSuccess_Cue"));
			}
			else
			{
				GEditor->PlayEditorSound(TEXT("/Engine/EditorSounds/Notifications/CompileFailed_Cue.CompileFailed_Cue"));
			}
		}
	}

	TSharedPtr<FPackagePluginTask> FPluginBuilderCommandActions::ActivePackagePluginTask = nullptr;
	TSharedPtr<SNotificationItem> FPluginBuilderCommandActions::PendingNotificationItem = nullptr;
}

#undef LOCTEXT_NAMESPACE
