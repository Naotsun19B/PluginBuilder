// Copyright 2022-2026 Naotsun. All Rights Reserved.

#include "PluginBuilder/CloudStorages/OneDrive/OneDriveClient.h"
#include "PluginBuilder/Utilities/OneDriveSettings.h"
#include "PluginBuilder/Utilities/PluginBuilderPackagingSettings.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "PlatformHttp.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/DateTime.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFileManager.h"
#include "HAL/PlatformFile.h"

namespace PluginBuilder
{
	FString FOneDriveClient::GetProviderName() const
	{
		return TEXT("OneDrive");
	}

	FString FOneDriveClient::GetRemoteBaseFolderPath() const
	{
		const auto& Settings = GetSettings<UOneDriveSettings>();
		return Settings.BaseFolderPath;
	}

	bool FOneDriveClient::IsAuthenticated() const
	{
		const auto& Settings = GetSettings<UOneDriveSettings>();
		return Settings.IsAuthenticated();
	}

	void FOneDriveClient::RefreshTokenIfNeeded(TFunction<void(bool bSuccess)> OnComplete)
	{
		const auto& Settings = GetSettings<UOneDriveSettings>();
		if (!Settings.IsAuthenticated())
		{
			UE_LOG(LogPluginBuilder, Error, TEXT("OneDrive: Not authenticated. Please sign in from editor preferences."));
			OnComplete(false);
			return;
		}

		const int64 NowUnix = FDateTime::UtcNow().ToUnixTimestamp();
		if (NowUnix < Settings.GetTokenExpiryTime())
		{
			// Token is still valid.
			OnComplete(true);
			return;
		}

		// Token expired — use the refresh token.
		UE_LOG(LogPluginBuilder, Log, TEXT("OneDrive: Access token expired. Refreshing..."));

		const FString& ClientId = Settings.ClientId;
		const FString RefreshToken = Settings.GetRefreshToken();

		const FString Body = FString::Printf(
			TEXT("client_id=%s&refresh_token=%s&grant_type=refresh_token&scope=Files.ReadWrite%%20offline_access%%20User.Read"),
			*FPlatformHttp::UrlEncode(ClientId),
			*FPlatformHttp::UrlEncode(RefreshToken)
		);

		const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
		Request->SetURL(TEXT("https://login.microsoftonline.com/consumers/oauth2/v2.0/token"));
		Request->SetVerb(TEXT("POST"));
		Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
		Request->SetContentAsString(Body);
		Request->OnProcessRequestComplete().BindLambda(
			[OnComplete, &Settings](FHttpRequestPtr /* Request */, FHttpResponsePtr Response, bool bConnected)
			{
				if (!bConnected || !Response.IsValid() || Response->GetResponseCode() != 200)
				{
					UE_LOG(LogPluginBuilder, Error, TEXT("OneDrive: Token refresh failed. Response code: %d"),
						Response.IsValid() ? Response->GetResponseCode() : -1);
					OnComplete(false);
					return;
				}

				TSharedPtr<FJsonObject> Json;
				const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
				if (!FJsonSerializer::Deserialize(Reader, Json) || !Json.IsValid())
				{
					UE_LOG(LogPluginBuilder, Error, TEXT("OneDrive: Failed to parse token refresh response."));
					OnComplete(false);
					return;
				}

				FString NewAccessToken;
				FString NewRefreshToken;
				int32 ExpiresIn = 3600;
				Json->TryGetStringField(TEXT("access_token"), NewAccessToken);
				Json->TryGetStringField(TEXT("refresh_token"), NewRefreshToken);
				Json->TryGetNumberField(TEXT("expires_in"), ExpiresIn);

				if (NewAccessToken.IsEmpty())
				{
					UE_LOG(LogPluginBuilder, Error, TEXT("OneDrive: Token refresh response missing access_token."));
					OnComplete(false);
					return;
				}

				const int64 NewExpiry = FDateTime::UtcNow().ToUnixTimestamp() + static_cast<int64>(ExpiresIn) - 60;
				const FString CurrentDisplayName = Settings.UserDisplayName;
				const FString RefreshToStore = NewRefreshToken.IsEmpty() ? Settings.GetRefreshToken() : NewRefreshToken;
				const_cast<UOneDriveSettings&>(Settings).StoreTokens(NewAccessToken, RefreshToStore, NewExpiry, CurrentDisplayName);
				const_cast<UOneDriveSettings&>(Settings).SaveConfig();

				UE_LOG(LogPluginBuilder, Log, TEXT("OneDrive: Access token refreshed successfully."));
				OnComplete(true);
			}
		);
		Request->ProcessRequest();
	}

	void FOneDriveClient::UploadFile(
		const FString& LocalFilePath,
		const FString& RemoteFilePath,
		TFunction<void(bool bSuccess, const FString& RemoteItemId)> OnComplete,
		TFunction<void(float Progress)> OnProgress
	)
	{
		RefreshTokenIfNeeded([this, LocalFilePath, RemoteFilePath, OnComplete, OnProgress](bool bTokenOk)
		{
			if (!bTokenOk)
			{
				OnComplete(false, FString());
				return;
			}

			const FString AccessToken = GetSettings<UOneDriveSettings>().GetAccessToken();
			CreateUploadSession(RemoteFilePath, AccessToken, [this, LocalFilePath, OnComplete, OnProgress](bool bSessionOk, const FString& UploadUrl)
			{
				if (!bSessionOk)
				{
					OnComplete(false, FString());
					return;
				}
				UploadChunks(UploadUrl, LocalFilePath, OnComplete, OnProgress);
			});
		});
	}

	void FOneDriveClient::GetShareUrl(
		const FString& RemoteItemId,
		TFunction<void(bool bSuccess, const FString& ShareUrl)> OnComplete
	)
	{
		RefreshTokenIfNeeded([this, RemoteItemId, OnComplete](bool bTokenOk)
		{
			if (!bTokenOk)
			{
				OnComplete(false, FString());
				return;
			}

			const FString AccessToken = GetSettings<UOneDriveSettings>().GetAccessToken();
			const FString Url = FString::Printf(
				TEXT("https://graph.microsoft.com/v1.0/me/drive/items/%s/createLink"),
				*RemoteItemId
			);

			const FString BodyJson = TEXT("{\"type\":\"edit\",\"scope\":\"anonymous\"}");

			const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
			Request->SetURL(Url);
			Request->SetVerb(TEXT("POST"));
			Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *AccessToken));
			Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
			Request->SetContentAsString(BodyJson);
			Request->OnProcessRequestComplete().BindLambda(
				[OnComplete](FHttpRequestPtr /* Request */, FHttpResponsePtr Response, bool bConnected)
				{
					if (!bConnected || !Response.IsValid())
					{
						OnComplete(false, FString());
						return;
					}

					const int32 Code = Response->GetResponseCode();
					if (Code != 200 && Code != 201)
					{
						UE_LOG(LogPluginBuilder, Error, TEXT("OneDrive: createLink failed with code %d: %s"), Code, *Response->GetContentAsString());
						OnComplete(false, FString());
						return;
					}

					TSharedPtr<FJsonObject> Json;
					const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
					if (!FJsonSerializer::Deserialize(Reader, Json) || !Json.IsValid())
					{
						OnComplete(false, FString());
						return;
					}

					FString ShareUrl;
					const TSharedPtr<FJsonObject>* LinkObject = nullptr;
					if (Json->TryGetObjectField(TEXT("link"), LinkObject) && LinkObject != nullptr)
					{
						(*LinkObject)->TryGetStringField(TEXT("webUrl"), ShareUrl);
					}

					OnComplete(!ShareUrl.IsEmpty(), ShareUrl);
				}
			);
			Request->ProcessRequest();
		});
	}

	void FOneDriveClient::CreateUploadSession(
		const FString& RemoteFilePath,
		const FString& AccessToken,
		TFunction<void(bool bSuccess, const FString& UploadUrl)> OnComplete
	)
	{
		// Graph API path: me/drive/root:/{remote path}:/createUploadSession
		const FString EncodedPath = FPlatformHttp::UrlEncode(RemoteFilePath).Replace(TEXT("%2F"), TEXT("/"));
		const FString ApiUrl = FString::Printf(
			TEXT("https://graph.microsoft.com/v1.0/me/drive/root:/%s:/createUploadSession"),
			*EncodedPath
		);

		const EOneDriveConflictBehavior ConflictBehavior = GetSettings<UPluginBuilderPackagingSettings>().ConflictBehavior;
		const FString BodyJson = FString::Printf(
			TEXT("{\"item\":{\"@microsoft.graph.conflictBehavior\":\"%s\"}}"),
			LexToString(ConflictBehavior)
		);

		const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
		Request->SetURL(ApiUrl);
		Request->SetVerb(TEXT("POST"));
		Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *AccessToken));
		Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
		Request->SetContentAsString(BodyJson);
		Request->OnProcessRequestComplete().BindLambda(
			[OnComplete](FHttpRequestPtr /* Request */, FHttpResponsePtr Response, bool bConnected)
			{
				if (!bConnected || !Response.IsValid() || Response->GetResponseCode() != 200)
				{
					UE_LOG(LogPluginBuilder, Error, TEXT("OneDrive: createUploadSession failed. Code: %d"),
						Response.IsValid() ? Response->GetResponseCode() : -1);
					OnComplete(false, FString());
					return;
				}

				TSharedPtr<FJsonObject> Json;
				const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
				if (!FJsonSerializer::Deserialize(Reader, Json) || !Json.IsValid())
				{
					OnComplete(false, FString());
					return;
				}

				FString UploadUrl;
				Json->TryGetStringField(TEXT("uploadUrl"), UploadUrl);
				OnComplete(!UploadUrl.IsEmpty(), UploadUrl);
			}
		);
		Request->ProcessRequest();
	}

	void FOneDriveClient::UploadChunks(
		const FString& UploadUrl,
		const FString& LocalFilePath,
		TFunction<void(bool bSuccess, const FString& ItemId)> OnComplete,
		TFunction<void(float Progress)> OnProgress
	)
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		if (!PlatformFile.FileExists(*LocalFilePath))
		{
			UE_LOG(LogPluginBuilder, Error, TEXT("OneDrive: Local file not found: %s"), *LocalFilePath);
			OnComplete(false, FString());
			return;
		}

		TArray<uint8> FileData;
		if (!FFileHelper::LoadFileToArray(FileData, *LocalFilePath))
		{
			UE_LOG(LogPluginBuilder, Error, TEXT("OneDrive: Failed to read file: %s"), *LocalFilePath);
			OnComplete(false, FString());
			return;
		}

		UE_LOG(LogPluginBuilder, Log, TEXT("OneDrive: Uploading %s (%lld bytes)..."), *FPaths::GetCleanFilename(LocalFilePath), static_cast<int64>(FileData.Num()));

		UploadNextChunk(UploadUrl, FileData, 0, OnComplete, OnProgress);
	}

	void FOneDriveClient::UploadNextChunk(
		const FString& UploadUrl,
		const TArray<uint8>& FileData,
		int64 ByteOffset,
		TFunction<void(bool bSuccess, const FString& ItemId)> OnComplete,
		TFunction<void(float Progress)> OnProgress
	)
	{
		const int64 TotalBytes = static_cast<int64>(FileData.Num());
		const int64 EndByte = FMath::Min(ByteOffset + ChunkSize - 1, TotalBytes - 1);
		const int64 ChunkLength = EndByte - ByteOffset + 1;

		const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
		Request->SetURL(UploadUrl);
		Request->SetVerb(TEXT("PUT"));
		Request->SetHeader(TEXT("Content-Length"), FString::FromInt(static_cast<int32>(ChunkLength)));
		Request->SetHeader(
			TEXT("Content-Range"),
			FString::Printf(TEXT("bytes %lld-%lld/%lld"), ByteOffset, EndByte, TotalBytes)
		);

		const TArray<uint8> ChunkData(FileData.GetData() + ByteOffset, static_cast<int32>(ChunkLength));
		Request->SetContent(ChunkData);

		Request->OnProcessRequestComplete().BindLambda(
			[this, UploadUrl, FileData, ByteOffset, ChunkLength, TotalBytes, OnComplete, OnProgress]
			(FHttpRequestPtr /* Request */, FHttpResponsePtr Response, bool bConnected)
			{
				if (!bConnected || !Response.IsValid())
				{
					UE_LOG(LogPluginBuilder, Error, TEXT("OneDrive: Chunk upload failed (connection error)."));
					OnComplete(false, FString());
					return;
				}

				const int32 Code = Response->GetResponseCode();
				const int64 NextOffset = ByteOffset + ChunkLength;
				const float Progress = static_cast<float>(NextOffset) / static_cast<float>(TotalBytes);

				if (OnProgress)
				{
					OnProgress(Progress);
				}

				// 202 Accepted = more chunks remain. 200/201 = upload complete.
				if (Code == 202)
				{
					UploadNextChunk(UploadUrl, FileData, NextOffset, OnComplete, OnProgress);
					return;
				}

				if (Code == 200 || Code == 201)
				{
					TSharedPtr<FJsonObject> Json;
					const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
					FString ItemId;
					if (FJsonSerializer::Deserialize(Reader, Json) && Json.IsValid())
					{
						Json->TryGetStringField(TEXT("id"), ItemId);
					}
					UE_LOG(LogPluginBuilder, Log, TEXT("OneDrive: Upload complete. Item ID: %s"), *ItemId);
					OnComplete(!ItemId.IsEmpty(), ItemId);
					return;
				}

				UE_LOG(LogPluginBuilder, Error, TEXT("OneDrive: Unexpected response code during chunk upload: %d"), Code);
				OnComplete(false, FString());
			}
		);
		Request->ProcessRequest();
	}
}
