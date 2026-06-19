// Copyright 2022-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PluginBuilder/Tasks/IUATBatchFileTask.h"

namespace PluginBuilder
{
	/**
	 * A task class to build the plugin.
	 */
	class PLUGINBUILDER_API FBuildPluginTask : public IUATBatchFileTask
	{
	public:
		// Constructor.
		FBuildPluginTask(
			const FString& InEngineVersion,
			const FUATBatchFileParams& InUATBatchFileParams,
			const FBuildPluginParams& InBuildPluginParams
		);

		// IPluginBuilderTask interface.
		virtual bool IsBuildTask() const override { return true; }
		virtual float GetProgress() const override;
		virtual FString GetProgressText() const override;
		// End of IPluginBuilderTask interface.

	protected:
		// IUATBatchFileTask interface.
		virtual void Initialize() override;
		virtual TArray<FString> GetUATArguments() const override;
		virtual FString GetDestinationDirectoryPath() const override;
		virtual void OnOutputLine(const FString& Line) override;
		// End of IUATBatchFileTask interface.

	private:
		// The dataset used to process plugin build.
		FBuildPluginParams BuildPluginParams;

		// Total number of compile actions reported by UBT. 0 means unknown.
		int32 TotalActions = 0;

		// Number of compile actions completed so far.
		int32 CompletedActions = 0;
	};
}
