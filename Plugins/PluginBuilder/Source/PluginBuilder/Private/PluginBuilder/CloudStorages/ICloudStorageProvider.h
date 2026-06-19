// Copyright 2022-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace PluginBuilder
{
	/**
	 * An interface for cloud storage providers that support file upload and share URL retrieval.
	 * Implement this interface to add support for additional cloud storage services.
	 */
	class ICloudStorageProvider
	{
	public:
		// Destructor.
		virtual ~ICloudStorageProvider() = default;

		// Returns the display name of this provider (e.g. "OneDrive").
		virtual FString GetProviderName() const = 0;

		// Returns the base folder path inside remote storage where files are uploaded.
		// Returns an empty string to upload directly under the root.
		virtual FString GetRemoteBaseFolderPath() const { return FString(); }

		// Returns whether the user is currently authenticated.
		virtual bool IsAuthenticated() const = 0;

		// Refreshes the access token if it has expired.
		// Calls OnComplete(true) if the token is valid or was refreshed successfully.
		virtual void RefreshTokenIfNeeded(TFunction<void(bool bSuccess)> OnComplete) = 0;

		// Uploads a local file to the specified remote path.
		// OnComplete is called with (bSuccess, RemoteItemId).
		// OnProgress is called periodically with a value from 0.0 to 1.0.
		virtual void UploadFile(
			const FString& LocalFilePath,
			const FString& RemoteFilePath,
			TFunction<void(bool bSuccess, const FString& RemoteItemId)> OnComplete,
			TFunction<void(float Progress)> OnProgress
		) = 0;

		// Looks up an existing item by its remote path without uploading.
		// Calls OnComplete(true, ItemId) if the file exists, or OnComplete(false, FString()) if it does not.
		virtual void FindItem(
			const FString& RemoteFilePath,
			TFunction<void(bool bFound, const FString& ItemId)> OnComplete
		) = 0;

		// Creates a shareable URL for an already-uploaded item.
		// OnComplete is called with (bSuccess, ShareUrl).
		virtual void GetShareUrl(
			const FString& RemoteItemId,
			TFunction<void(bool bSuccess, const FString& ShareUrl)> OnComplete
		) = 0;
	};
}
