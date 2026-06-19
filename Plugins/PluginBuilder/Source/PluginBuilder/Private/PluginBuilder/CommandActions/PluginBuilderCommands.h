// Copyright 2022-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/Commands/UICommandInfo.h"

namespace PluginBuilder
{
	/**
	 * A class to register the shortcut key used in this plugin.
	 */
	class PLUGINBUILDER_API FPluginBuilderCommands : public TCommands<FPluginBuilderCommands>
	{
	public:
		// Constructor.
		FPluginBuilderCommands();

		// TCommands interface.
		virtual void RegisterCommands() override;
		// End of TCommands interface.

		// Returns whether the commands registered here are bound.
		static bool IsBound();

		// Binds commands registered here.
		static void Bind();
	
	protected:
		// Binds commands actually registered here.
		virtual void BindCommands();

	public:
		// The list of shortcut keys used by this plugin.
		TSharedRef<FUICommandList> CommandBindings;
		
		// Instances of bound commands.
		TSharedPtr<FUICommandInfo> BuildPlugin;
		TSharedPtr<FUICommandInfo> NoHostPlatform;
		TSharedPtr<FUICommandInfo> Rocket;
		TSharedPtr<FUICommandInfo> CreateSubFolder;
		TSharedPtr<FUICommandInfo> StrictIncludes;
		TSharedPtr<FUICommandInfo> Unversioned;
		TSharedPtr<FUICommandInfo> ZipUp;
		TSharedPtr<FUICommandInfo> KeepBinariesFolder;
		TSharedPtr<FUICommandInfo> OutputAllZipFilesToSingleFolder;
		TSharedPtr<FUICommandInfo> KeepUPluginProperties;
		TSharedPtr<FUICommandInfo> AppendEngineVersionToZipFileName;
		TSharedPtr<FUICommandInfo> OpenBuildSettings;

		// Whether to automatically upload zip files to cloud storage after packaging.
		TSharedPtr<FUICommandInfo> AutoUploadZipFiles;
		// Whether to retrieve a share URL for each uploaded file.
		TSharedPtr<FUICommandInfo> GetShareUrls;
		// Conflict behavior: overwrite the existing file.
		TSharedPtr<FUICommandInfo> ConflictBehaviorReplace;
		// Conflict behavior: rename the uploaded file to avoid collision.
		TSharedPtr<FUICommandInfo> ConflictBehaviorRename;
		// Conflict behavior: abort the upload if a collision is detected.
		TSharedPtr<FUICommandInfo> ConflictBehaviorFail;
		// Conflict behavior: skip the upload if the file exists; retrieve a share URL for the existing file.
		TSharedPtr<FUICommandInfo> ConflictBehaviorIgnore;
		// Opens the cloud storage provider settings page.
		TSharedPtr<FUICommandInfo> OpenCloudStorageSettings;

		// Runs only the build step (no zip, no upload).
		TSharedPtr<FUICommandInfo> BuildPluginOnly;
		// Runs only the zip step against existing build output (no build, no upload).
		TSharedPtr<FUICommandInfo> ZipPluginOnly;
		// Uploads existing zip files from the PackagedPlugins folder to cloud storage.
		TSharedPtr<FUICommandInfo> UploadToCloud;

	private:
		// Whether the commands registered here are bound.
		bool bIsBound;
	};
}
