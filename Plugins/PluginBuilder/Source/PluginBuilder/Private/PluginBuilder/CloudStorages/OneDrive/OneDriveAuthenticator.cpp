// Copyright 2022-2026 Naotsun. All Rights Reserved.

#include "PluginBuilder/CloudStorages/OneDrive/OneDriveAuthenticator.h"
#include "PluginBuilder/Utilities/OneDriveSettings.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "PluginBuilder/PluginBuilderGlobals.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "PlatformHttp.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/DateTime.h"
#include "Async/Async.h"
#include "HAL/RunnableThread.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "Networking.h"

namespace PluginBuilder
{
	void FOneDriveAuthenticator::StartAuthentication(
		const FString& InClientId,
		TFunction<void(bool bSuccess)> InOnComplete
	)
	{
		const FString CodeVerifier = GenerateCodeVerifier();
		const FString AuthUrl = BuildAuthorizationUrl(InClientId, CodeVerifier);

		FPlatformProcess::LaunchURL(*AuthUrl, nullptr, nullptr);

		TFunction<void(const FString&)> OnCodeReceived = [InClientId, CodeVerifier, InOnComplete](const FString& Code)
		{
			ExchangeCodeForTokens(InClientId, Code, CodeVerifier, InOnComplete);
		};

		FOneDriveAuthenticator* Authenticator = new FOneDriveAuthenticator(InClientId, CodeVerifier, MoveTemp(OnCodeReceived));
		FRunnableThread::Create(Authenticator, TEXT("OneDriveAuthListenerThread"), 0, TPri_Normal);
	}

	FOneDriveAuthenticator::FOneDriveAuthenticator(
		const FString& InClientId,
		const FString& InCodeVerifier,
		TFunction<void(const FString& Code)> InOnCodeReceived
	)
		: ClientId(InClientId)
		, CodeVerifier(InCodeVerifier)
		, OnCodeReceived(MoveTemp(InOnCodeReceived))
		, bShouldStop(false)
	{
	}

	bool FOneDriveAuthenticator::Init()
	{
		return true;
	}

	uint32 FOneDriveAuthenticator::Run()
	{
		ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
		if (SocketSubsystem == nullptr)
		{
			UE_LOG(LogPluginBuilder, Error, TEXT("OneDrive: Failed to get socket subsystem."));
			AsyncTask(ENamedThreads::GameThread, [this]() { OnCodeReceived(FString()); });
			return 1;
		}

		FSocket* ListenSocket = SocketSubsystem->CreateSocket(NAME_Stream, TEXT("OneDriveAuthListener"), false);
		if (ListenSocket == nullptr)
		{
			UE_LOG(LogPluginBuilder, Error, TEXT("OneDrive: Failed to create listener socket."));
			AsyncTask(ENamedThreads::GameThread, [this]() { OnCodeReceived(FString()); });
			return 1;
		}

		const TSharedRef<FInternetAddr> InternetAddress = SocketSubsystem->CreateInternetAddr();
		InternetAddress->SetIp(0x7F000001); // 127.0.0.1
		InternetAddress->SetPort(RedirectPort);

		ListenSocket->SetReuseAddr(true);
		if (!ListenSocket->Bind(*InternetAddress) || !ListenSocket->Listen(1))
		{
			UE_LOG(LogPluginBuilder, Error, TEXT("OneDrive: Failed to bind/listen on port %d."), RedirectPort);
			SocketSubsystem->DestroySocket(ListenSocket);
			AsyncTask(ENamedThreads::GameThread, [this]() { OnCodeReceived(FString()); });
			return 1;
		}

		UE_LOG(LogPluginBuilder, Log, TEXT("OneDrive: Waiting for OAuth callback on port %d..."), RedirectPort);

		FSocket* ClientSocket = nullptr;
		while (!bShouldStop && ClientSocket == nullptr)
		{
			bool bHasPendingConnection = false;
			if (ListenSocket->WaitForPendingConnection(bHasPendingConnection, 500 * 1000))
			{
				if (bHasPendingConnection)
				{
					TSharedRef<FInternetAddr> ClientAddr = SocketSubsystem->CreateInternetAddr();
					ClientSocket = ListenSocket->Accept(*ClientAddr, TEXT("OneDriveCallback"));
				}
			}
		}

		SocketSubsystem->DestroySocket(ListenSocket);

		if (ClientSocket == nullptr || bShouldStop)
		{
			if (ClientSocket != nullptr)
			{
				SocketSubsystem->DestroySocket(ClientSocket);
			}
			AsyncTask(ENamedThreads::GameThread, [this]() { OnCodeReceived(FString()); });
			return 1;
		}

		// Read the HTTP GET request.
		uint8 Buffer[4096];
		int32 BytesRead = 0;
		ClientSocket->Recv(Buffer, sizeof(Buffer) - 1, BytesRead);
		Buffer[BytesRead] = '\0';
		const FString RequestData = UTF8_TO_TCHAR(reinterpret_cast<const ANSICHAR*>(Buffer));

		// Send a minimal HTTP 200 response.
		const FString HtmlBody = TEXT("<!DOCTYPE html><html><body><h2>Authentication successful.</h2><p>You can close this window and return to the editor.</p></body></html>");
		const FString HttpResponse = FString::Printf(
			TEXT("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: %d\r\nConnection: close\r\n\r\n%s"),
			HtmlBody.Len(),
			*HtmlBody
		);
		const FTCHARToUTF8 ResponseUtf8(*HttpResponse);
		int32 BytesSent = 0;
		ClientSocket->Send(reinterpret_cast<const uint8*>(ResponseUtf8.Get()), ResponseUtf8.Length(), BytesSent);
		ClientSocket->Close();
		SocketSubsystem->DestroySocket(ClientSocket);

		// Extract the authorization code from the GET request line.
		// Format: "GET /callback?code=XXXX&... HTTP/1.1"
		FString AuthCode;
		const FString CodeParam = TEXT("?code=");
		const int32 CodeStart = RequestData.Find(CodeParam);
		if (CodeStart != INDEX_NONE)
		{
			const int32 ValueStart = CodeStart + CodeParam.Len();
			const int32 ValueEnd = RequestData.Find(TEXT("&"), ESearchCase::CaseSensitive, ESearchDir::FromStart, ValueStart);
			const int32 SpaceEnd = RequestData.Find(TEXT(" "), ESearchCase::CaseSensitive, ESearchDir::FromStart, ValueStart);
			const int32 End = (ValueEnd != INDEX_NONE && (SpaceEnd == INDEX_NONE || ValueEnd < SpaceEnd)) ? ValueEnd : SpaceEnd;
			if (End != INDEX_NONE)
			{
				AuthCode = RequestData.Mid(ValueStart, End - ValueStart);
			}
			else
			{
				AuthCode = RequestData.Mid(ValueStart);
				AuthCode.TrimEndInline();
			}
		}

		if (AuthCode.IsEmpty())
		{
			UE_LOG(LogPluginBuilder, Error, TEXT("OneDrive: Failed to extract authorization code from callback."));
			AsyncTask(ENamedThreads::GameThread, [this]() { OnCodeReceived(FString()); });
			return 1;
		}

		// The code arrives URL-encoded in the query string; decode it once so that
		// ExchangeCodeForTokens can re-encode it cleanly for the form body.
		AuthCode = FPlatformHttp::UrlDecode(AuthCode);

		UE_LOG(LogPluginBuilder, Log, TEXT("OneDrive: Authorization code received."));

		TFunction<void(const FString&)> Callback = OnCodeReceived;
		AsyncTask(
			ENamedThreads::GameThread, [Callback, AuthCode]()
			{
				Callback(AuthCode);
			}
		);

		return 0;
	}

	void FOneDriveAuthenticator::Stop()
	{
		bShouldStop = true;
	}

	void FOneDriveAuthenticator::ExchangeCodeForTokens(
		const FString& InClientId,
		const FString& InCode,
		const FString& InCodeVerifier,
		TFunction<void(bool bSuccess)> InOnComplete
	)
	{
		if (InCode.IsEmpty())
		{
			InOnComplete(false);
			return;
		}

		const FString Body = FString::Printf(
			TEXT("client_id=%s&code=%s&redirect_uri=%s&grant_type=authorization_code&code_verifier=%s"),
			*FPlatformHttp::UrlEncode(InClientId),
			*FPlatformHttp::UrlEncode(InCode),
			*FPlatformHttp::UrlEncode(RedirectUri),
			*FPlatformHttp::UrlEncode(InCodeVerifier)
		);

		const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
		Request->SetURL(TEXT("https://login.microsoftonline.com/consumers/oauth2/v2.0/token"));
		Request->SetVerb(TEXT("POST"));
		Request->SetHeader(TEXT("Content-Type"), TEXT("application/x-www-form-urlencoded"));
		Request->SetContentAsString(Body);
		Request->OnProcessRequestComplete().BindLambda(
			[InOnComplete](FHttpRequestPtr /* Request */, FHttpResponsePtr Response, bool bConnectedSuccessfully)
			{
				if (!bConnectedSuccessfully || !Response.IsValid() || Response->GetResponseCode() != 200)
				{
					UE_LOG(LogPluginBuilder, Error, TEXT("OneDrive: Token exchange failed. Response code: %d"),
						Response.IsValid() ? Response->GetResponseCode() : -1);
					if (Response.IsValid())
					{
						UE_LOG(LogPluginBuilder, Error, TEXT("OneDrive: Token exchange error body: %s"), *Response->GetContentAsString());
					}
					InOnComplete(false);
					return;
				}

				TSharedPtr<FJsonObject> JsonObject;
				const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
				if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
				{
					UE_LOG(LogPluginBuilder, Error, TEXT("OneDrive: Failed to parse token response."));
					InOnComplete(false);
					return;
				}

				FString AccessToken;
				FString RefreshToken;
				int32 ExpiresIn = 3600;
				JsonObject->TryGetStringField(TEXT("access_token"), AccessToken);
				JsonObject->TryGetStringField(TEXT("refresh_token"), RefreshToken);
				JsonObject->TryGetNumberField(TEXT("expires_in"), ExpiresIn);

				if (AccessToken.IsEmpty() || RefreshToken.IsEmpty())
				{
					UE_LOG(LogPluginBuilder, Error, TEXT("OneDrive: Token response missing required fields."));
					InOnComplete(false);
					return;
				}

				const int64 ExpiryTime = FDateTime::UtcNow().ToUnixTimestamp() + static_cast<int64>(ExpiresIn) - 60;

				FetchUserDisplayName(
					AccessToken, [AccessToken, RefreshToken, ExpiryTime, InOnComplete](const FString& DisplayName)
					{
						auto& Settings = GetSettings<UOneDriveSettings>();
						const_cast<UOneDriveSettings&>(Settings).StoreTokens(AccessToken, RefreshToken, ExpiryTime, DisplayName);
						const_cast<UOneDriveSettings&>(Settings).SaveConfig();
						InOnComplete(true);
					}
				);
			}
		);
		Request->ProcessRequest();
	}

	void FOneDriveAuthenticator::FetchUserDisplayName(
		const FString& InAccessToken,
		TFunction<void(const FString& DisplayName)> InOnComplete
	)
	{
		const TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
		Request->SetURL(TEXT("https://graph.microsoft.com/v1.0/me?$select=displayName"));
		Request->SetVerb(TEXT("GET"));
		Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *InAccessToken));
		Request->OnProcessRequestComplete().BindLambda(
			[InOnComplete](FHttpRequestPtr /* Request */, FHttpResponsePtr Response, bool bSuccess)
			{
				FString DisplayName;
				if (bSuccess && Response.IsValid() && Response->GetResponseCode() == 200)
				{
					TSharedPtr<FJsonObject> JsonObject;
					const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
					if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
					{
						JsonObject->TryGetStringField(TEXT("displayName"), DisplayName);
					}
				}
				InOnComplete(DisplayName);
			}
		);
		Request->ProcessRequest();
	}

	FString FOneDriveAuthenticator::GenerateCodeVerifier()
	{
		static const FString Alphabet = TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~");
		
		FString Verifier;
		Verifier.Reserve(64);
		for (int32 Index = 0; Index < 64; Index++)
		{
			Verifier += Alphabet[FMath::RandRange(0, Alphabet.Len() - 1)];
		}
		
		return Verifier;
	}

	FString FOneDriveAuthenticator::BuildAuthorizationUrl(const FString& InClientId, const FString& InCodeVerifier)
	{
		return FString::Printf(
			TEXT("https://login.microsoftonline.com/consumers/oauth2/v2.0/authorize")
			TEXT("?client_id=%s")
			TEXT("&response_type=code")
			TEXT("&redirect_uri=%s")
			TEXT("&scope=%s")
			TEXT("&code_challenge=%s")
			TEXT("&code_challenge_method=plain"),
			*FPlatformHttp::UrlEncode(InClientId),
			*FPlatformHttp::UrlEncode(RedirectUri),
			*FPlatformHttp::UrlEncode(TEXT("Files.ReadWrite offline_access User.Read")),
			*FPlatformHttp::UrlEncode(InCodeVerifier)
		);
	}
	
	const FString FOneDriveAuthenticator::RedirectUri = TEXT("http://localhost:7890/callback");
}
