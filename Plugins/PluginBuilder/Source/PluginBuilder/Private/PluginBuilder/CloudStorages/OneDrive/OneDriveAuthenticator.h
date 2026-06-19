// Copyright 2022-2026 Naotsun. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"

namespace PluginBuilder
{
	/**
	 * Handles the OAuth2 authorization code flow with PKCE (plain method) for OneDrive.
	 * Opens the user's browser to the Microsoft login page, then listens on a local
	 * TCP port for the redirect callback. After receiving the authorization code,
	 * exchanges it for access and refresh tokens via the Microsoft identity platform.
	 */
	class FOneDriveAuthenticator : public FRunnable
	{
	public:
		// Starts the full authentication flow asynchronously.
		// OnComplete is called on the game thread with true on success.
		static void StartAuthentication(
			const FString& InClientId,
			TFunction<void(bool bSuccess)> InOnComplete
		);

	public:
		// Constructor.
		FOneDriveAuthenticator(
			const FString& InClientId,
			const FString& InCodeVerifier,
			TFunction<void(const FString& Code)> InOnCodeReceived
		);

		// FRunnable interface.
		virtual bool Init() override;
		virtual uint32 Run() override;
		virtual void Stop() override;
		// End of FRunnable interface.

	private:
		// Exchanges the authorization code for access/refresh tokens via HTTP.
		static void ExchangeCodeForTokens(
			const FString& InClientId,
			const FString& InCode,
			const FString& InCodeVerifier,
			TFunction<void(bool bSuccess)> InOnComplete
		);

		// Fetches the signed-in user's display name via Microsoft Graph.
		static void FetchUserDisplayName(
			const FString& InAccessToken,
			TFunction<void(const FString& DisplayName)> InOnComplete
		);

		// Generates a cryptographically random alphanumeric string suitable for use as a PKCE code verifier.
		static FString GenerateCodeVerifier();

		// Builds and returns the Microsoft OAuth2 authorization URL.
		static FString BuildAuthorizationUrl(const FString& InClientId, const FString& InCodeVerifier);

	private:
		// Azure AD application client ID.
		FString ClientId;
		// PKCE code verifier generated at authentication start.
		FString CodeVerifier;
		// Callback invoked with the extracted authorization code on success.
		TFunction<void(const FString& Code)> OnCodeReceived;
		// Set to true when Stop() is called to exit the listener loop.
		bool bShouldStop;

		// The local port on which the redirect callback is received.
		static constexpr int32 RedirectPort = 7890;

		// The redirect URI registered in the Azure AD app.
		static const FString RedirectUri;
	};
}
