// Copyright 2022-2026 Naotsun. All Rights Reserved.

#include "PluginBuilder/IPluginBuilder.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "PluginBuilder/CommandActions/PluginBuilderCommands.h"
#include "PluginBuilder/Utilities/PluginBuilderStyle.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "PluginBuilder/UIExtensions/ToolMenuExtender.h"
#include "PluginBuilder/Utilities/PluginPackager.h"
#include "PluginBuilder/Types/EngineVersions.h"
#include "PluginBuilder/Types/HostPlatforms.h"
#include "PluginBuilder/Types/TargetPlatforms.h"

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
		// Registers command actions.
		FPluginBuilderCommands::Register();
		FPluginBuilderCommands::Bind();

		// Registers style set.
		FPluginBuilderStyle::Register();
		
		// Registers settings.
		UPluginBuilderSettings::Register();

		// Registers menu extension.
		FToolMenuExtender::Register();
		
		// Logs installed engine versions and available platforms.
		FEngineVersions::LogInstalledEngineVersions();
		FHostPlatforms::LogAvailableHostPlatformNames();
		FTargetPlatforms::LogAvailableTargetPlatformNames();
	}

	void FPluginBuilderModule::ShutdownModule()
	{
		// Unregisters menu extension.
		FToolMenuExtender::Unregister();

		// Unregisters style set.
		FPluginBuilderStyle::Unregister();
		
		// Unregisters command actions.
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
