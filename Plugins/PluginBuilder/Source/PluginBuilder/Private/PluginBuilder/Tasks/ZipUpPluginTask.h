// Copyright 2022-2023 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PluginBuilder/Tasks/IUATBatchFileTask.h"

namespace PluginBuilder
{
	/**
	 * A task class to zip up the plugin.
	 */
	class PLUGINBUILDER_API FZipUpPluginTask : public IUATBatchFileTask
	{
	public:
		// Constructor.
		FZipUpPluginTask(
			const FString& InEngineVersion,
			const FUATBatchFileParams& InUATBatchFileParams,
			const FZipUpPluginParams& InZipUpPluginParams,
			const TSharedPtr<IUATBatchFileTask>& DependentTask
		);
		
		// IUATBatchFileTask interface.
		virtual void Initialize() override;
		virtual void Terminate() override;
		virtual TArray<FString> GetUATArguments() const override;
		virtual FString GetDestinationDirectoryPath() const override;
		// End of IUATBatchFileTask interface.
		
	private:
		// Returns the path of the working directory where files are removed for compression.
		FString GetZipTempDirectoryPath() const;
		
		// Copies the properties of the original uplugin file to the UAT output uplugin file.
		bool CopyUPluginProperties() const;

	private:
		// The dataset used to process plugin zip up.
		FZipUpPluginParams ZipUpPluginParams;

		// The path of the output compressed file.
		FString ZipFilePath;
	};
}
