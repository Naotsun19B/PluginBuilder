// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
#include "PluginBuilder/Types/PackagePluginParams.h"
#include "PluginBuilder/Utilities/EditorNotificationUtils.h"

namespace PluginBuilder
{
	class FPackagePluginTask;
	
	/**
	 * A class that handles the packaging of plugins.
	 */
	class PLUGINBUILDER_API FPluginPackager : public FTickableGameObject
	{
	public:
		// Creates and starts a task to specify a parameters and package the plugin.
		static void StartPackagePluginTask();
		static void StartPackagePluginTask(const FPackagePluginParams& InParams);

		// Returns whether package processing is being done.
		static bool IsRunning();
		
		// FTickableObjectBase interface.
		virtual void Tick(float DeltaTime) override;
		virtual bool IsTickable() const override;
		virtual TStatId GetStatId() const override;
		virtual bool IsTickableWhenPaused() const override;
		virtual bool IsTickableInEditor() const override;
		// End of FTickableObjectBase interface.

	private:
		// Called once when package processing begins.
		void Initialize();

		// Called once when package processing ends.
		void Terminate();

		// Opens and focus the Output Log tab.
		static void OpenOutputLog();

		// Called when the editor notification cancel button is pressed.
		void OnCancelButtonPressed();
		
	private:
		// A running task that packages a plugin.
		static TUniquePtr<FPluginPackager> Instance;

		// An editor notification item that package a plugin.
		static FNotificationHandle PendingNotificationHandle;

		// A dataset used to process plugin packages.
		FPackagePluginParams Params;

		// A list of tasks scheduled to process.
		TArray<TSharedRef<FPackagePluginTask>> Tasks;
		
		// Whether the task needs to be canceled.
		bool bNeedToCancel = false;

		// Whether any error occurred during the packaging process.
		bool bHasAnyError = false;
	};
}
