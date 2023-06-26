// Copyright 2022-2023 Naotsun. All Rights Reserved.

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

	protected:
		// IUATBatchFileTask interface.
		virtual void Initialize() override;
		virtual TArray<FString> GetUATArguments() const override;
		virtual FString GetDestinationDirectoryPath() const override;
		// End of IUATBatchFileTask interface.

	private:
		// The dataset used to process plugin build.
		FBuildPluginParams BuildPluginParams;
	};
}
