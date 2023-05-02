// Copyright 2022-2023 Naotsun. All Rights Reserved.

#include "PluginBuilder/Utilities/ZipUtils.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include <shlobj.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <tlhelp32.h>

namespace PluginBuilder
{
	namespace WinShell
	{
		// In the Windows API, paths must be separated by backslashes.
		inline FString ToWindowsPath(const FString& InPath)
		{
			return InPath.Replace(TEXT("/"), TEXT("\\"));
		}
		
		struct FFolderAutoCleaner
		{
		public:
			explicit FFolderAutoCleaner(Folder* InFolder)
				: Folder(InFolder)
			{
			}

			~FFolderAutoCleaner()
			{
				if (Folder != nullptr)
				{
					Folder->Release();
				}
			}
			
		private:
			Folder* Folder;
		};
		
		struct FScopedVariant
		{
		public:
			FScopedVariant(
				const VARENUM Type,
				const TFunction<void(VARIANT& VariantRef)>& Predicate
			)
			{
				VariantInit(&Variant);
				Variant.vt = Type;
				Predicate(Variant);
			}

			~FScopedVariant()
			{
				VariantClear(&Variant);
			}

			VARIANT& operator*()
			{
				return Variant;
			}
					
		private:
			VARIANT Variant;
		};
			
		struct FScopedStringVariant : public FScopedVariant
		{
		public:
			explicit FScopedStringVariant(const FString& Value)
				: FScopedVariant(
					VT_BSTR,
					[&](VARIANT& VariantRef)
					{
						VariantRef.bstrVal = SysAllocString(*Value);
					}
				)
			{
			}
		};

		struct FScopedDispatchVariant : public FScopedVariant
		{
		public:
			explicit FScopedDispatchVariant(IDispatch* Value)
				: FScopedVariant(
					VT_DISPATCH,
					[&](VARIANT& VariantRef)
					{
						VariantRef.pdispVal = Value;
					}
				)
			{
			}
		};

		struct FScopedLongVariant : public FScopedVariant
		{
		public:
			explicit FScopedLongVariant(LONG Value)
				: FScopedVariant(
					VT_I4,
					[&](VARIANT& VariantRef)
					{
						VariantRef.lVal = Value;
					}
				)
			{
			}
		};
		
		class FZipWriter : public FNoncopyable
		{
		public:
			explicit FZipWriter(const FString& InZipFilePath)
				: ShellDispatch(nullptr)
				, bHasErrorWithInitialization(false)
				, ZipFilePath(ToWindowsPath(FPaths::ConvertRelativePathToFull(InZipFilePath)))
			{
				CoInitializeEx(nullptr, COINIT_MULTITHREADED);
				
				const HRESULT Result = CoCreateInstance(
					CLSID_Shell,
					nullptr,
					CLSCTX_INPROC_SERVER,
					IID_PPV_ARGS(&ShellDispatch)
				);

				bHasErrorWithInitialization = FAILED(Result);
				if (bHasErrorWithInitialization)
				{
					return;
				}
				
				// Create a ZIP file seed if the ZIP file does not exist.
				if (!FPaths::FileExists(ZipFilePath))
				{
					static const TArray<uint8> EmptyZipBinary = { 80, 75, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
					if (!FFileHelper::SaveArrayToFile(EmptyZipBinary, *ZipFilePath))
					{
						bHasErrorWithInitialization = true;
					}
				}
			}

			~FZipWriter()
			{
				CoUninitialize();
			}

			bool AddDirectoryToZip(const FString& DirectoryPath)
			{
				if (bHasErrorWithInitialization)
				{
					return false;
				}
				
				FScopedStringVariant SourceFolderVariant(ToWindowsPath(DirectoryPath));
				Folder* SourceFolder;
				if (ShellDispatch->NameSpace(*SourceFolderVariant, &SourceFolder) != S_OK)
				{
					return false;
				}
				FFolderAutoCleaner SourceFolderCleaner(SourceFolder);

				// Create a Folder object for the ZIP file.
				FScopedStringVariant ZipFileVariant(ZipFilePath);
				Folder* ZipFolder;
				if (ShellDispatch->NameSpace(*ZipFileVariant, &ZipFolder) != S_OK)
				{
					return false;
				}
				FFolderAutoCleaner ZipFolderCleaner(ZipFolder);
				
				// Count the number of threads before copying to identify the thread that is copying.
				DWORD PreCopyNumOfThreads = GetNumOfThreads();

				// Copy the folder to ZIP.
				FScopedDispatchVariant CopyItemVariant(SourceFolder);
				// If FOF_SILENT is specified, the progress during processing will not be displayed.
				FScopedLongVariant CopyOptionsVariant(FOF_NO_UI);

				HRESULT CopyResult = ZipFolder->CopyHere(*CopyItemVariant, *CopyOptionsVariant);
				if (CopyResult != S_OK)
				{
					return false;
				}
				
				// Gets the number of sibling threads and child threads again,
				// considers the last thread found as a copy thread, and waits until it ends.
				if (CopyResult == S_OK)
				{
					DWORD PostCopyNumOfThreads = GetNumOfThreads();
					if (PreCopyNumOfThreads < PostCopyNumOfThreads)
					{
						WaitForCopyThread();
					}
				}
				
				return true;
			}

		private:
			static void EnumerateThreads(const TFunction<void(const THREADENTRY32& ThreadEntry)>& Predicate)
			{
				const HANDLE Handle = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
				if (Handle != INVALID_HANDLE_VALUE)
				{
					THREADENTRY32 ThreadEntry;
					ThreadEntry.dwSize = sizeof(ThreadEntry);
					if (Thread32First(Handle, &ThreadEntry))
					{
						do
						{
							if ((ThreadEntry.th32OwnerProcessID == GetCurrentProcessId()) &&
								(ThreadEntry.th32ThreadID != GetCurrentThreadId()))
							{
								Predicate(ThreadEntry);
							}
							
							ThreadEntry.dwSize = sizeof(ThreadEntry);
						}
						while (Thread32Next(Handle, &ThreadEntry));
					}
				}
				
				CloseHandle(Handle);
			}
			
			static DWORD GetNumOfThreads()
			{
				DWORD NumOfThreads = 0;

				EnumerateThreads(
					[&](const THREADENTRY32& ThreadEntry)
					{
						NumOfThreads++;
					}
				);
				
				return NumOfThreads;
			}

			static void WaitForCopyThread()
			{
				TArray<HANDLE> Threads;
				EnumerateThreads(
					[&](const THREADENTRY32& ThreadEntry)
					{
						const HANDLE Thread = OpenThread(
							THREAD_ALL_ACCESS,
							FALSE,
							ThreadEntry.th32ThreadID
						);
						Threads.Add(Thread);
					}
				);
				
				WaitForMultipleObjects(
					Threads.Num(),
					Threads.GetData(),
					TRUE,
					INFINITE
				);

				for (const auto& Thread : Threads)
				{
					CloseHandle(Thread);
				}
			}
			
		private:
			IShellDispatch* ShellDispatch;
			bool bHasErrorWithInitialization;
			FString ZipFilePath;
		};
	}
	
	bool FZipUtils::ZipUp(const FString& DirectoryPathToZipUp, const FString& OutputZipFilePath)
	{
		WinShell::FZipWriter ZipWriter(OutputZipFilePath);
		return ZipWriter.AddDirectoryToZip(DirectoryPathToZipUp);
	}
}

#include "Windows/HideWindowsPlatformTypes.h"
