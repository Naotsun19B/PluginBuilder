﻿// Copyright 2022 Naotsun. All Rights Reserved.

#include "PluginBuilder/Utilities/EngineVersions.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include <winreg.h>
#include "Windows/HideWindowsPlatformTypes.h"

namespace PluginBuilder
{
	namespace WinReg
	{
		static constexpr int MaxKeyNameLength = 255;
		static const FString UnrealEngineKeyPath = TEXT("SOFTWARE\\EpicGames\\Unreal Engine");

		struct FScopedKey
		{
		public:
			explicit FScopedKey(HKEY Root, LPCWSTR SubKey, REGSAM Desired = KEY_READ)
			{
				const LSTATUS Result = RegOpenKeyExW(
                	Root,
                	SubKey,
                	0,
                	Desired,
                	&Key
                );
				bIsValid = (Result == ERROR_SUCCESS);
			}
			
			~FScopedKey()
			{
				if (IsValid())
				{
					RegCloseKey(Key);
				}
			}

			bool IsValid() const
			{
				return bIsValid;
			}

			HKEY& operator*()
			{
				return Key;
			}

			bool GetNumOfSubKeys(DWORD& NumOfSubKeys) const
			{
				FILETIME LastWriteTime;
				const LSTATUS Result = RegQueryInfoKeyW(
					Key,
					nullptr,
					nullptr,
					nullptr,
					&NumOfSubKeys,
					nullptr,
					nullptr,
					nullptr,
					nullptr,
					nullptr,
					nullptr,
					&LastWriteTime
				);

				return (Result == ERROR_SUCCESS);
			}

			bool GetSubKeyName(const DWORD Index, FString& SubKeyName) const
			{
				TCHAR KeyNameBuffer[MaxKeyNameLength];
				DWORD KeyNameLength = MaxKeyNameLength;

				FILETIME LastWriteTime;
				const LSTATUS Result = RegEnumKeyExW(
					Key,
					Index,
					KeyNameBuffer,
					&KeyNameLength,
					nullptr,
					nullptr,
					nullptr,
					&LastWriteTime
				);

				if (Result == ERROR_SUCCESS)
				{
					SubKeyName = FString(KeyNameLength, KeyNameBuffer);
					return true;
				}

				return false;
			}

			bool GetStringValue(LPCWSTR PropertyName, FString& Value) const
			{
				DWORD DataSize = 0;
				LSTATUS Result = RegGetValueW(
					Key,
					nullptr,
					PropertyName,
					RRF_RT_REG_SZ,
					nullptr,
					nullptr,
					&DataSize
				);
				if (Result != ERROR_SUCCESS)
				{
					return false;
				}
				
				auto& ValueBuffer = Value.GetCharArray();
				ValueBuffer.SetNumUninitialized(DataSize / sizeof(wchar_t));
				Result = RegGetValueW(
					Key,
					nullptr,
					PropertyName,
					RRF_RT_REG_SZ,
					nullptr,
					ValueBuffer.GetData(),
					&DataSize
				);
				
				return (Result == ERROR_SUCCESS);
			}

		private:
			HKEY Key;
			bool bIsValid;
		};
	}
	
	TArray<FEngineVersions::FEngineVersion> FEngineVersions::GetEngineVersions(bool bWithRefresh /* = true */)
	{
		if (bWithRefresh)
		{
			RefreshEngineVersions();
		}
		
		return EngineVersions;
	}

	void FEngineVersions::RefreshEngineVersions()
	{
		const TArray<FString>& VersionNames = GetVersionNames();
		EngineVersions.Reset(VersionNames.Num());
		for (const auto& VersionName : VersionNames)
		{
			const FString InstalledDirectoryKeyPath = (WinReg::UnrealEngineKeyPath + TEXT('\\') + VersionName);
			const WinReg::FScopedKey InstalledDirectoryKey(HKEY_LOCAL_MACHINE, *InstalledDirectoryKeyPath);
			if (!InstalledDirectoryKey.IsValid())
			{
				continue;
			}

			FString InstalledDirectory;
			if (!InstalledDirectoryKey.GetStringValue(TEXT("InstalledDirectory"), InstalledDirectory))
			{
				continue;
			}

			const FString UATBatchFile = (
				InstalledDirectory / TEXT("Engine") / TEXT("Build") / TEXT("BatchFiles") / TEXT("RunUAT.bat")
			);

			FEngineVersion EngineVersion;
			EngineVersion.VersionName = VersionName;
			EngineVersion.InstalledDirectory = InstalledDirectory;
			EngineVersion.UATBatchFile = UATBatchFile;
			EngineVersions.Add(EngineVersion);
		}
	}

	TArray<FString> FEngineVersions::GetVersionNames()
	{
		const WinReg::FScopedKey UnrealEngineKey(HKEY_LOCAL_MACHINE, *WinReg::UnrealEngineKeyPath);
		if (!UnrealEngineKey.IsValid())
		{
			return {};
		}

		DWORD NumOfSubKeys = 0;
		if (!UnrealEngineKey.GetNumOfSubKeys(NumOfSubKeys))
		{
			return {};
		}

		TArray<FString> UnrealEngineVersions;
		UnrealEngineVersions.Reserve(NumOfSubKeys);
		for (DWORD Index = 0; Index < NumOfSubKeys; Index++)
		{
			FString SubKeyName;
			if (UnrealEngineKey.GetSubKeyName(Index, SubKeyName))
			{
				UnrealEngineVersions.Add(SubKeyName);
			}
		}

		return UnrealEngineVersions;
	}

	TArray<FEngineVersions::FEngineVersion> FEngineVersions::EngineVersions;
}
