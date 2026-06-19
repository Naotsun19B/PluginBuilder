// Copyright 2022-2026 Naotsun. All Rights Reserved.

#include "PluginBuilder/Utilities/PluginPackager.h"
#include "PluginBuilder/Tasks/IPluginBuilderTask.h"
#include "PluginBuilder/Tasks/IUATBatchFileTask.h"
#include "PluginBuilder/Tasks/BuildPluginTask.h"
#include "PluginBuilder/Tasks/ZipUpPluginTask.h"
#include "PluginBuilder/Tasks/UploadToCloudTask.h"
#include "PluginBuilder/Types/BuildTargets.h"
#include "PluginBuilder/Utilities/PluginBuilderPackagingSettings.h"
#include "PluginBuilder/CloudStorages/CloudStorageManager.h"
#include "PluginBuilder/CloudStorages/ICloudStorageProvider.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
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

	bool FPluginPackager::StartBuildOnlyTask()
	{
		FPackagePluginParams Params;
		if (!FPackagePluginParams::MakeDefault(Params))
		{
			UE_LOG(LogPluginBuilder, Warning, TEXT("No plugin or engine version to build for was specified."));
			return false;
		}
		Params.ZipUpPluginParams.Reset();
		Params.CloudStorageParams.Reset();
		return StartPackagePluginTask(Params);
	}

	bool FPluginPackager::StartZipOnlyTask()
	{
		const auto& BuildConfigurationSettings = GetSettings<UPluginBuilderPackagingSettings>();
		if (!BuildConfigurationSettings.IsReadyToStartPackagePluginTask())
		{
			UE_LOG(LogPluginBuilder, Warning, TEXT("No plugin or engine version to build for was specified."));
			return false;
		}

		FPackagePluginParams Params;
		if (!FPackagePluginParams::MakeDefault(Params))
		{
			return false;
		}
		Params.BuildPluginParams.Reset();
		Params.CloudStorageParams.Reset();

		if (!Params.ZipUpPluginParams.IsSet())
		{
			// bZipUp was disabled in settings; populate zip params directly from current settings.
			const FBuildTargets::FBuildTarget& BuildTarget = BuildConfigurationSettings.SelectedBuildTarget.GetValue();
			FZipUpPluginParams ZipParams;
			ZipParams.bCanPluginContainContent = BuildTarget.CanPluginContainContent();
			ZipParams.bOutputAllZipFilesToSingleFolder = BuildConfigurationSettings.bOutputAllZipFilesToSingleFolder;
			ZipParams.bKeepBinariesFolder = BuildConfigurationSettings.bKeepBinariesFolder;
			ZipParams.bKeepUPluginProperties = BuildConfigurationSettings.bKeepUPluginProperties;
			ZipParams.bAppendEngineVersionToZipFileName = BuildConfigurationSettings.bAppendEngineVersionToZipFileName;
			ZipParams.CompressionLevel = BuildConfigurationSettings.CompressionLevel;
			Params.ZipUpPluginParams = ZipParams;
		}

		return StartPackagePluginTask(Params);
	}

	bool FPluginPackager::StartUploadOnlyTask(
		const TArray<FString>& InZipFilePaths,
		const FString& InPackagedPluginsPath,
		const FString& InPluginName,
		bool bInGetShareUrls
	)
	{
		if (IsPackagePluginTaskRunning())
		{
			UE_LOG(LogPluginBuilder, Warning, TEXT("A package plugin task is currently running. Cannot start upload."));
			return false;
		}

		if (InZipFilePaths.Num() == 0)
		{
			UE_LOG(LogPluginBuilder, Warning, TEXT("Cloud Storage upload: No zip files found in the specified directory."));
			return false;
		}

		Instance = MakeUnique<FPluginPackager>();
		Instance->Params.UATBatchFileParams.PluginFriendlyName = InPluginName;
		Instance->Tasks.Add(
			MakeShared<FUploadToCloudTask>(InZipFilePaths, InPackagedPluginsPath, InPluginName, bInGetShareUrls)
		);
		Instance->TotalTaskCount = 1;
		Instance->bIsUploadOnlyMode = true;

		PendingNotificationHandle = FEditorNotification::Pending(
			FText::Format(
				LOCTEXT("UploadNotificationTextFormat", "Uploading to Cloud Storage...\r\n{0}\r\n{1}"),
				FText::FromString(InPluginName),
				FText::FromString(Instance->Tasks[0]->GetTaskLabel())
			),
			0.f,
			TArray<FEditorNotificationInteraction>{
				FEditorNotificationInteraction(
					LOCTEXT("ShowOutputLogLinkText", "Show Output Log"),
					FSimpleDelegate::CreateStatic(&FPluginPackager::OpenOutputLog)
				),
				FEditorNotificationInteraction(
					LOCTEXT("CancelButtonLabel", "Cancel"),
					LOCTEXT("CancelUploadButtonTooltip", "Cancels the OneDrive upload process."),
					FSimpleDelegate::CreateRaw(Instance.Get(), &FPluginPackager::OnCancelButtonPressed)
				)
			}
		);

		check(IsValid(GEditor));
		GEditor->PlayEditorSound(TEXT("/Engine/EditorSounds/Notifications/CompileStart_Cue.CompileStart_Cue"));

		return true;
	}

	bool FPluginPackager::IsPackagePluginTaskRunning()
	{
		return Instance.IsValid();
	}

	void FPluginPackager::CleanupStatics()
	{
		Instance.Reset();
		PendingNotificationHandle = FEditorNotificationHandle{};
	}

	void FPluginPackager::Tick(float DeltaTime)
	{
		check(Tasks.IsValidIndex(0));
		const TSharedRef<IPluginBuilderTask>& Task = Tasks[0];

		if (Task->GetState() == IPluginBuilderTask::EState::PreInitialize)
		{
			Task->Initialize();
		}
		if (Task->GetState() == IPluginBuilderTask::EState::Processing)
		{
			Task->Tick(DeltaTime);

			if (Task->GetState() == IPluginBuilderTask::EState::Processing)
			{
				NotificationUpdateTimer += DeltaTime;
				if (!bWasCanceled && PendingNotificationHandle.IsValid() && (NotificationUpdateTimer >= NotificationUpdateInterval))
				{
					NotificationUpdateTimer = 0.f;
					PendingNotificationHandle.SetText(BuildNotificationText(Task));
				}
			}
		}
		if (Task->GetState() == IPluginBuilderTask::EState::PreTerminate)
		{
			Task->Terminate();
		}
		if (Task->GetState() == IPluginBuilderTask::EState::Terminated)
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

				if ((Tasks.Num() > 0) && PendingNotificationHandle.IsValid())
				{
					NotificationUpdateTimer = 0.f;
					PendingNotificationHandle.SetText(BuildNotificationText(Tasks[0]));
				}
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
				TSharedPtr<FZipUpPluginTask> ZipTask = MakeShared<FZipUpPluginTask>(
					EngineVersion,
					Params.UATBatchFileParams,
					Params.ZipUpPluginParams.GetValue(),
					BuildPluginTask
				);
				Tasks.Add(ZipTask.ToSharedRef());
				ZipTaskRefs.Add(ZipTask);
			}
		}

		// Add cloud upload task when params request it and zip files will be produced.
		if (Params.CloudStorageParams.IsSet() && (ZipTaskRefs.Num() > 0))
		{
			TSharedPtr<ICloudStorageProvider> Provider = FCloudStorageManager::GetCurrentProvider();
			if (Provider.IsValid() && Provider->IsAuthenticated())
			{
				const FString PackagedPluginsPath = Params.UATBatchFileParams.OutputDirectoryPath.Get(FPaths::ProjectDir()) / TEXT("PackagedPlugins");
				Tasks.Add(MakeShared<FUploadToCloudTask>(
					ZipTaskRefs,
					PackagedPluginsPath,
					Params.UATBatchFileParams.GetPluginNameInSpecifiedFormat(),
					Params.CloudStorageParams.GetValue().bGetShareUrls
				));
			}
			else
			{
				UE_LOG(LogPluginBuilder, Warning, TEXT("Cloud storage upload skipped: not authenticated."));
			}
		}

		TotalTaskCount = Tasks.Num();

		for (const TSharedRef<IPluginBuilderTask>& Task : Tasks)
		{
			if (Task->IsBuildTask())
			{
				TotalBuildCount++;
			}
			else if (Task->IsZipTask())
			{
				TotalZipCount++;
			}
			else if (Task->IsCloudUploadTask())
			{
				TotalUploadCount++;
			}
		}
		TArray<FString> TaskCountParts;
		if (TotalBuildCount > 0)
		{
			TaskCountParts.Add(FString::Printf(TEXT("%d %s"), TotalBuildCount, (TotalBuildCount == 1) ? TEXT("Build") : TEXT("Builds")));
		}
		if (TotalZipCount > 0)
		{
			TaskCountParts.Add(FString::Printf(TEXT("%d %s"), TotalZipCount, (TotalZipCount == 1) ? TEXT("Zip") : TEXT("Zips")));
		}
		if (TotalUploadCount > 0)
		{
			TaskCountParts.Add(FString::Printf(TEXT("%d %s"), TotalUploadCount, (TotalUploadCount == 1) ? TEXT("Upload") : TEXT("Uploads")));
		}
		const FString TaskCountText = FString::Join(TaskCountParts, TEXT(", "));

		PendingNotificationHandle = FEditorNotification::Pending(
			FText::Format(
				LOCTEXT("NotificationTextFormat", "Preparing...\r\n{0} ({1})\r\n{2}"),
				FText::FromString(Params.UATBatchFileParams.PluginFriendlyName),
				FText::FromString(Params.UATBatchFileParams.PluginVersionName),
				FText::FromString(TaskCountText)
			),
			0.f,
			TArray<FEditorNotificationInteraction>{
				// Show Output Log Hyperlink.
				FEditorNotificationInteraction(
					LOCTEXT("ShowOutputLogLinkText", "Show Output Log"),
					FSimpleDelegate::CreateStatic(&FPluginPackager::OpenOutputLog)
				),
				// Cancel Button.
				FEditorNotificationInteraction(
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
		UE_LOG(LogPluginBuilder, Log, TEXT("===================================================================================================="));
		
		if (PendingNotificationHandle.IsValid())
		{
			PendingNotificationHandle.Fadeout();
		}
		
		if (bWasCanceled)
		{
			FEditorNotification::Success(LOCTEXT("PackageCanceled", "Plugin packaging has been cancelled."));
		}
		else if (bHasAnyError)
		{
			FEditorNotification::Fail(LOCTEXT("PackageFailed", "Failed to package the plugin."));
		}
		else
		{
			FEditorNotification::Success(LOCTEXT("PackageSucceeded", "Plugin packaging has completed successfully."));

			if (!Params.IsFormatExpectedByMarketplace())
			{
				UE_LOG(LogPluginBuilder, Warning, TEXT("The created package is not in a format that can be submitted to the marketplace."));
				UE_LOG(LogPluginBuilder, Warning, TEXT("If you plan to submit to the marketplace, please review the build options and zip up options."));
				UE_LOG(LogPluginBuilder, Log, TEXT("===================================================================================================="));
			}
		}
		
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

	FText FPluginPackager::BuildNotificationText(const TSharedRef<IPluginBuilderTask>& CurrentTask) const
	{
		const float TaskProgress = CurrentTask->GetProgress();
		const int32 TaskProgressPercent = ((TaskProgress >= 0.f) ? FMath::RoundToInt(TaskProgress * 100.f) : 0);

		FText Message;
		if (bIsUploadOnlyMode || CurrentTask->IsCloudUploadTask())
		{
			Message = LOCTEXT("UploadProgressText", "Uploading to Cloud Storage...");
		}
		else if (CurrentTask->IsZipTask())
		{
			Message = LOCTEXT("ZipProgressText", "Zipping Up...");
		}
		else
		{
			Message = LOCTEXT("BuildProgressText", "Building...");
		}

		int32 RemainingBuildCount = 0;
		int32 RemainingZipCount = 0;
		int32 RemainingUploadCount = 0;
		for (const TSharedRef<IPluginBuilderTask>& Task : Tasks)
		{
			if (Task->IsBuildTask())
			{
				RemainingBuildCount++;
			}
			else if (Task->IsZipTask())
			{
				RemainingZipCount++;
			}
			else if (Task->IsCloudUploadTask())
			{
				RemainingUploadCount++;
			}
		}
		TArray<FString> ProgressParts;
		if (TotalBuildCount > 0)
		{
			ProgressParts.Add(FString::Printf(TEXT("Build %d/%d"), (TotalBuildCount - RemainingBuildCount), TotalBuildCount));
		}
		if (TotalZipCount > 0)
		{
			ProgressParts.Add(FString::Printf(TEXT("Zip %d/%d"), (TotalZipCount - RemainingZipCount), TotalZipCount));
		}
		if (TotalUploadCount > 0)
		{
			ProgressParts.Add(FString::Printf(TEXT("Upload %d/%d"), (TotalUploadCount - RemainingUploadCount), TotalUploadCount));
		}
		const FString ProgressText = FString::Join(ProgressParts, TEXT(", "));

		return FText::Format(
			LOCTEXT("BuildProgressTextFormat", "{0} {1}%\r\n{2} ({3})\r\n{4}\r\n{5}"),
			Message,
			FText::AsNumber(TaskProgressPercent),
			FText::FromString(Params.UATBatchFileParams.PluginFriendlyName),
			FText::FromString(Params.UATBatchFileParams.PluginVersionName),
			FText::FromString(CurrentTask->GetTaskLabel()),
			FText::FromString(ProgressText)
		);
	}

	TUniquePtr<FPluginPackager> FPluginPackager::Instance;
	FEditorNotificationHandle FPluginPackager::PendingNotificationHandle;
}

#undef LOCTEXT_NAMESPACE
