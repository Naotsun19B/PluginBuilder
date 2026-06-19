// Copyright 2022-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PluginBuilder/CloudStorages/ICloudStorageProvider.h"

namespace PluginBuilder
{
	/**
	 * ICloudStorageProvider implementation for Microsoft OneDrive.
	 * Uses the Microsoft Graph API (v1.0) and the resumable upload session protocol.
	 */
	class FOneDriveClient : public ICloudStorageProvider
	{
	public:
		// ICloudStorageProvider interface.
		virtual FString GetProviderName() const override;
		virtual FString GetRemoteBaseFolderPath() const override;
		virtual bool IsAuthenticated() const override;
		virtual void RefreshTokenIfNeeded(TFunction<void(bool bSuccess)> OnComplete) override;
		virtual void UploadFile(
			const FString& LocalFilePath,
			const FString& RemoteFilePath,
			TFunction<void(bool bSuccess, const FString& RemoteItemId)> OnComplete,
			TFunction<void(float Progress)> OnProgress
		) override;
		virtual void GetShareUrl(
			const FString& RemoteItemId,
			TFunction<void(bool bSuccess, const FString& ShareUrl)> OnComplete
		) override;
		// End of ICloudStorageProvider interface.

	private:
		// Creates an upload session and returns its upload URL.
		void CreateUploadSession(
			const FString& RemoteFilePath,
			const FString& AccessToken,
			TFunction<void(bool bSuccess, const FString& UploadUrl)> OnComplete
		);

		// Uploads file data in chunks using the given upload session URL.
		// Returns the item ID of the completed upload.
		void UploadChunks(
			const FString& UploadUrl,
			const FString& LocalFilePath,
			TFunction<void(bool bSuccess, const FString& ItemId)> OnComplete,
			TFunction<void(float Progress)> OnProgress
		);

		// Sends one chunk and recurses for the next.
		void UploadNextChunk(
			const FString& UploadUrl,
			const TArray<uint8>& FileData,
			int64 ByteOffset,
			TFunction<void(bool bSuccess, const FString& ItemId)> OnComplete,
			TFunction<void(float Progress)> OnProgress
		);

	private:
		// Maximum chunk size for resumable uploads (10 MB, must be a multiple of 320 KiB).
		static constexpr int64 ChunkSize = (10 * 1024 * 1024);
	};
}
