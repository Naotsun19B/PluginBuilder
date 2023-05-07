// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/IPluginBuilder.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "PluginBuilder/CommandActions/PluginBuilderCommands.h"
#include "PluginBuilder/Utilities/PluginBuilderStyle.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "PluginBuilder/UIExtensions/ToolMenuExtender.h"
#include "PluginBuilder/Utilities/PluginPackager.h"

DEFINE_LOG_CATEGORY(LogPluginBuilder);

namespace PluginBuilder
{
	const FName IPluginBuilder::ModuleName = TEXT("PluginBuilder");
	
	class FPluginBuilderModule : public IPluginBuilder
	{
	public:
		// IModuleInterface interface.
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;
		// End of IModuleInterface interface.

		// IPluginBuilder interface.
		virtual bool StartPackagePluginTask(const TOptional<FPackagePluginParams>& InParams) override;
		virtual bool IsPackagePluginTaskRunning() override;
		// End of IPluginBuilder interface.
	};
	
	void FPluginBuilderModule::StartupModule()
	{
		// Register command actions.
		FPluginBuilderCommands::Register();
		FPluginBuilderCommands::Bind();

		// Register style set.
		FPluginBuilderStyle::Register();
		
		// Register settings.
		UPluginBuilderSettings::Register();

		// Register menu extension.
		FToolMenuExtender::Register();
	}

	void FPluginBuilderModule::ShutdownModule()
	{
		// Unregister menu extension.
		FToolMenuExtender::Unregister();
		
		// Unregister settings.
		UPluginBuilderSettings::Unregister();

		// Unregister style set.
		FPluginBuilderStyle::Unregister();
		
		// Unregister command actions.
		if (FPluginBuilderCommands::IsBound())
		{
			FPluginBuilderCommands::Unregister();
		}
	}
	
	bool FPluginBuilderModule::StartPackagePluginTask(const TOptional<FPackagePluginParams>& InParams)
	{
		return FPluginPackager::StartPackagePluginTask(InParams);
	}

	bool FPluginBuilderModule::IsPackagePluginTaskRunning()
	{
		return FPluginPackager::IsPackagePluginTaskRunning();
	}
}

IMPLEMENT_MODULE(PluginBuilder::FPluginBuilderModule, PluginBuilder)
