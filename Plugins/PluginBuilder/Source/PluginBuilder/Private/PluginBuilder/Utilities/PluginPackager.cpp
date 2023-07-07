// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/Utilities/PluginPackager.h"
#include "PluginBuilder/Tasks/IUATBatchFileTask.h"
#include "PluginBuilder/Tasks/BuildPluginTask.h"
#include "PluginBuilder/Tasks/ZipUpPluginTask.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "DesktopPlatformModule.h"
#include "HAL/PlatformFileManager.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Docking/TabManager.h"
#include "Editor.h"
#if UE_5_01_OR_LATER
#include "OutputLogModule.h"
#endif

#define LOCTEXT_NAMESPACE "PluginPackager"

namespace PluginBuilder
{
	DECLARE_STATS_GROUP(TEXT("PackagePluginTask"), STATGROUP_PackagePluginTask, STATCAT_Advanced);
	
	bool FPluginPackager::StartPackagePluginTask(const TOptional<FPackagePluginParams>& InParams /* = {} */)
	{
		if (IsPackagePluginTaskRunning())
		{
			UE_LOG(LogPluginBuilder, Warning, TEXT("A package plugin task is currently running. (plugin in package : %s)"), *Instance->Params.UATBatchFileParams.PluginFriendlyName);
			return false;
		}

		FPackagePluginParams ParamsToPass;
		if (InParams.IsSet())
		{
			ParamsToPass = InParams.GetValue();
		}
		else
		{
            if (!FPackagePluginParams::MakeDefault(ParamsToPass))
            {
            	UE_LOG(LogPluginBuilder, Warning, TEXT("No plugin or engine version to build for was specified."));
            	return false;
            }
		}
		if (!ParamsToPass.IsValid())
		{
			UE_LOG(LogPluginBuilder, Warning, TEXT("The specified plugin does not exist, or an invalid value is specified for the engine versions."));
			return false;
		}

		if (!ParamsToPass.UATBatchFileParams.OutputDirectoryPath.IsSet())
		{
			if (IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get())
			{
				FString OutputDirectoryPath;
				const bool bWasSelected = DesktopPlatform->OpenDirectoryDialog(
					FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr),
					TEXT("Select Output Directory"),
					FPaths::ProjectDir(),
					OutputDirectoryPath
				);
				if (bWasSelected)
				{
					ParamsToPass.UATBatchFileParams.OutputDirectoryPath = OutputDirectoryPath;
				}
			}
		}
		if (!ParamsToPass.UATBatchFileParams.OutputDirectoryPath.IsSet())
		{
			return false;
		}

		Instance = MakeUnique<FPluginPackager>();
		Instance->Params = ParamsToPass;
		Instance->Initialize();

		return true;
	}

	bool FPluginPackager::IsPackagePluginTaskRunning()
	{
		return Instance.IsValid();
	}

	void FPluginPackager::Tick(float DeltaTime)
	{
		check(Tasks.IsValidIndex(0));
		const TSharedRef<IUATBatchFileTask>& Task = Tasks[0];

		if (Task->GetState() == IUATBatchFileTask::EState::PreInitialize)
		{
			Task->Initialize();
		}
		if (Task->GetState() == IUATBatchFileTask::EState::Processing)
		{
			Task->Tick(DeltaTime);
		}
		if (Task->GetState() == IUATBatchFileTask::EState::PreTerminate)
		{
			Task->Terminate();
		}
		if (Task->GetState() == IUATBatchFileTask::EState::Terminated)
		{
			if (Task->HasAnyError())
			{
				bHasAnyError = true;
			}
			
			if (bWasCanceled)
			{
				Tasks.Empty();
			}
			else
			{
				Tasks.RemoveAt(0);
			}
		}

		if (Tasks.Num() == 0)
		{
			Terminate();
		}
	}

	bool FPluginPackager::IsTickable() const
	{
		return (Tasks.Num() > 0);
	}

	TStatId FPluginPackager::GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FPluginPackager, STATGROUP_PackagePluginTask);
	}

	bool FPluginPackager::IsTickableWhenPaused() const
	{
		return true;
	}

	bool FPluginPackager::IsTickableInEditor() const
	{
		return true;
	}

	void FPluginPackager::Initialize()
	{
		for (const auto& EngineVersion : Params.EngineVersions)
		{
			TSharedPtr<FBuildPluginTask> BuildPluginTask = nullptr;
			if (Params.BuildPluginParams.IsSet())
			{
				BuildPluginTask = MakeShared<FBuildPluginTask>(
					EngineVersion,
					Params.UATBatchFileParams,
					Params.BuildPluginParams.GetValue()
				);
				Tasks.Add(BuildPluginTask.ToSharedRef());
			}
			
			if (Params.ZipUpPluginParams.IsSet())
			{
				Tasks.Add(
					MakeShared<FZipUpPluginTask>(
						EngineVersion,
						Params.UATBatchFileParams,
						Params.ZipUpPluginParams.GetValue(),
						BuildPluginTask
					)
				);
			}
		}
		
		PendingNotificationHandle = FEditorNotificationUtils::ShowNotification(
			FText::Format(
				LOCTEXT("NotificationTextFormat", "Packaging {0} ({1})..."),
				FText::FromString(Params.UATBatchFileParams.PluginFriendlyName),
				FText::FromString(Params.UATBatchFileParams.PluginVersionName)
			),
			FEditorNotificationUtils::CS_Pending,
			0.f,
			TArray<FNotificationInteraction>{
				// Show Output Log Hyperlink.
				FNotificationInteraction(
					LOCTEXT("ShowOutputLogLinkText", "Show Output Log"),
					FSimpleDelegate::CreateStatic(&FPluginPackager::OpenOutputLog)
				),
				// Cancel Button.
				FNotificationInteraction(
					LOCTEXT("CancelButtonLabel", "Cancel"),
					LOCTEXT("CancelButtonTooltip", "Cancels the plugin packaging process."),
					FSimpleDelegate::CreateRaw(this, &FPluginPackager::OnCancelButtonPressed)
				)
			}
		);

		check(IsValid(GEditor));
		GEditor->PlayEditorSound(TEXT("/Engine/EditorSounds/Notifications/CompileStart_Cue.CompileStart_Cue"));

#if UE_5_01_OR_LATER
		if (Params.bShowOnlyLogsFromThisPluginWhenPackageProcessStarts)
		{
			FOutputLogModule::Get().UpdateOutputLogFilter(TArray<FName>{ LogPluginBuilder.GetCategoryName() });
		}
#endif
	}

	void FPluginPackager::Terminate()
	{
		if (PendingNotificationHandle.IsValid())
		{
			PendingNotificationHandle.Fadeout();
		}

		FText NotificationText;
		FEditorNotificationUtils::ECompletionState CompletionState;
		if (bWasCanceled)
		{
			NotificationText = LOCTEXT("PackageCanceled", "Plugin packaging has been cancelled.");
			CompletionState = FEditorNotificationUtils::CS_Success;
		}
		else if (bHasAnyError)
		{
			NotificationText = LOCTEXT("PackageFailed", "Failed to package the plugin.");
			CompletionState = FEditorNotificationUtils::CS_Fail;
		}
		else
		{
			NotificationText = LOCTEXT("PackageSucceeded", "Plugin packaging has completed successfully.");
			CompletionState = FEditorNotificationUtils::CS_Success;
		}

		FEditorNotificationUtils::ShowNotification(
			NotificationText,
			CompletionState
		);
		
		check(IsValid(GEditor));
		if (!bHasAnyError && !bWasCanceled)
		{
			GEditor->PlayEditorSound(TEXT("/Engine/EditorSounds/Notifications/CompileSuccess_Cue.CompileSuccess_Cue"));
		}
		else
		{
			GEditor->PlayEditorSound(TEXT("/Engine/EditorSounds/Notifications/CompileFailed_Cue.CompileFailed_Cue"));
		}
		
		Instance.Reset();
	}

	void FPluginPackager::OpenOutputLog()
	{
		static const FName OutputLogTabId = TEXT("OutputLog");
							
		const TSharedRef<FGlobalTabmanager> GlobalTabManager = FGlobalTabmanager::Get();
		GlobalTabManager->TryInvokeTab(OutputLogTabId);
	}

	void FPluginPackager::OnCancelButtonPressed()
	{
		bWasCanceled = true;
		if (Tasks.IsValidIndex(0))
		{
			Tasks[0]->RequestCancel();
		}

		if (PendingNotificationHandle.IsValid())
		{
			PendingNotificationHandle.SetText(
				LOCTEXT("CancelPackagingNotificationText", "Waiting for packaging cancellation...")
			);
		}
	}

	TUniquePtr<FPluginPackager> FPluginPackager::Instance;
	FNotificationHandle FPluginPackager::PendingNotificationHandle;
}

#undef LOCTEXT_NAMESPACE
