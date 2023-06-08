// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/Types/EngineVersions.h"
#include "PluginBuilder/Utilities/PluginBuilderSettings.h"
#include "Misc/Paths.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include <winreg.h>

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
	
	TArray<FEngineVersions::FEngineVersion> FEngineVersions::GetEngineVersions(const bool bWithRefresh /* = true */)
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
			InstalledDirectory.ReplaceInline(TEXT("\\"), TEXT("/"));

			const FString UATBatchFile = FPaths::Combine(
				*InstalledDirectory, TEXT("Engine"), TEXT("Build"), TEXT("BatchFiles"), TEXT("RunUAT.bat")
			);

			FEngineVersion EngineVersion;
			EngineVersion.VersionName = VersionName;
			EngineVersion.InstalledDirectory = InstalledDirectory;
			EngineVersion.UATBatchFile = UATBatchFile;
			EngineVersion.MajorVersionName = TEXT("Custom");
			{
				int32 MajorVersion = INDEX_NONE;
				{
					FString MajorVersionString;
					if (EngineVersion.VersionName.Split(TEXT("."), &MajorVersionString, nullptr))
					{
						if (FCString::IsNumeric(*MajorVersionString))
						{
							MajorVersion = FCString::Atoi(*MajorVersionString);
						}
					}
				}
				if (MajorVersion != INDEX_NONE)
				{
					EngineVersion.MajorVersionName = FString::Printf(TEXT("UE%d"), MajorVersion);
				}
			}
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

		UnrealEngineVersions.Sort(
			[](const FString& Lhs, const FString& Rhs) -> bool
			{
				const float LhsValue = FCString::Atof(*Lhs);
				const float RhsValue = FCString::Atof(*Rhs);
				return (LhsValue < RhsValue);
			}
		);

		return UnrealEngineVersions;
	}

	bool FEngineVersions::FindUATBatchFileByVersionName(const FString& VersionName, FString& UATBatchFile, const bool bWithRefresh /* = true */)
	{
		if (bWithRefresh)
		{
			RefreshEngineVersions();
		}
		
		for (const auto& EngineVersion : EngineVersions)
		{
			if (EngineVersion.VersionName.Equals(VersionName))
			{
				UATBatchFile = EngineVersion.UATBatchFile;
				return true;
			}
		}

		return false;
	}

	TArray<FString> FEngineVersions::GetMajorVersionNames(const bool bWithRefresh /* = true */)
	{
		if (bWithRefresh)
		{
			RefreshEngineVersions();
		}

		TArray<FString> MajorVersionNames;
		for (const auto& EngineVersion : EngineVersions)
		{
			if (MajorVersionNames.Contains(EngineVersion.MajorVersionName))
			{
				continue;
			}

			MajorVersionNames.Add(EngineVersion.MajorVersionName);
		}

		return MajorVersionNames;
	}

	void FEngineVersions::ToggleEngineVersion(const FEngineVersion EngineVersion)
	{
		UPluginBuilderSettings::ModifyProperties(
			[&EngineVersion](UPluginBuilderSettings& Settings)
			{
				if (Settings.EngineVersions.Contains(EngineVersion.VersionName))
				{
					Settings.EngineVersions.Remove(EngineVersion.VersionName);
				}
				else
				{
					Settings.EngineVersions.Add(EngineVersion.VersionName);
				}
			},
			UPluginBuilderSettings::EPostModifiedProcessing::SortEngineVersion
		);
	}

	bool FEngineVersions::GetEngineVersionState(const FEngineVersion EngineVersion)
	{
		return UPluginBuilderSettings::Get().EngineVersions.Contains(EngineVersion.VersionName);
	}

	void FEngineVersions::EnableAllEngineVersions()
	{
		UPluginBuilderSettings::ModifyProperties(
			[](UPluginBuilderSettings& Settings)
			{
				Settings.EngineVersions.Reset(EngineVersions.Num());
				for (const auto& EngineVersion : EngineVersions)
				{
					Settings.EngineVersions.Add(EngineVersion.VersionName);
				}
			},
			UPluginBuilderSettings::EPostModifiedProcessing::SortEngineVersion
		);
	}

	void FEngineVersions::EnableByMajorVersion(const FString MajorVersionName)
	{
		UPluginBuilderSettings::ModifyProperties(
			[&MajorVersionName](UPluginBuilderSettings& Settings)
			{
				Settings.EngineVersions.Empty();
				for (const auto& EngineVersion : EngineVersions)
				{
					if (!MajorVersionName.Equals(EngineVersion.MajorVersionName))
					{
						continue;
					}

					Settings.EngineVersions.Add(EngineVersion.VersionName);
				}
			},
			UPluginBuilderSettings::EPostModifiedProcessing::SortEngineVersion
		);
	}

	void FEngineVersions::EnableLatest3EngineVersions()
	{
		UPluginBuilderSettings::ModifyProperties(
			[](UPluginBuilderSettings& Settings)
			{
				Settings.EngineVersions.Empty();
				const int32 NumOfEngineVersions = EngineVersions.Num();
				for (int32 Index = NumOfEngineVersions - 3; Index < NumOfEngineVersions; Index++)
				{
					Settings.EngineVersions.Add(EngineVersions[Index].VersionName);
				}
			},
			UPluginBuilderSettings::EPostModifiedProcessing::SortEngineVersion
		);
	}

	TArray<FEngineVersions::FEngineVersion> FEngineVersions::EngineVersions;
}

#include "Windows/HideWindowsPlatformTypes.h"
