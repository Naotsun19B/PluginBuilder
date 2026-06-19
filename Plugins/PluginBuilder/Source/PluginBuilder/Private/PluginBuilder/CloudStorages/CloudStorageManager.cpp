// Copyright 2022-2026 Naotsun. All Rights Reserved.

#include "PluginBuilder/CloudStorages/CloudStorageManager.h"
#include "PluginBuilder/CloudStorages/ICloudStorageProvider.h"
#include "PluginBuilder/CloudStorages/OneDrive/OneDriveClient.h"
#include "PluginBuilder/Utilities/PluginBuilderEditorSettings.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"

namespace PluginBuilder
{
	TSharedPtr<ICloudStorageProvider> FCloudStorageManager::CurrentProvider;

	TSharedPtr<ICloudStorageProvider> FCloudStorageManager::GetCurrentProvider()
	{
		const auto& EditorSettings = GetSettings<UPluginBuilderEditorSettings>();

		switch (EditorSettings.CloudStorageProvider)
		{
		case ECloudStorageProvider::OneDrive:
		default:
			if (!CurrentProvider.IsValid())
			{
				CurrentProvider = MakeShared<FOneDriveClient>();
			}
			break;
		}

		return CurrentProvider;
	}

	void FCloudStorageManager::RegisterProvider(TSharedPtr<ICloudStorageProvider> InProvider)
	{
		CurrentProvider = MoveTemp(InProvider);
	}
}
