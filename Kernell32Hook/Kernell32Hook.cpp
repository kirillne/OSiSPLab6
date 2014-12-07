// Kernell32Hook.cpp: определяет экспортированные функции для приложения DLL.
//

#include "stdafx.h"
//#include "Kernell32Hook.h"
#include "HookedRputine.h"
#include <tchar.h>

HANDLE file;
CRITICAL_SECTION fileWriteCriticalSection;
DWORD t; 

typedef HANDLE (WINAPI *PCREATEFILEW) (
									   LPCWSTR lpFileName,
									   DWORD dwDesiredAccess,
									   DWORD dwShareMode,
									   LPSECURITY_ATTRIBUTES lpSecurityAttributes,
									   DWORD dwCreationDisposition,
									   DWORD dwFlagsAndAttributes,
									   HANDLE hTemplateFile
									   );
typedef BOOL (WINAPI *PWRITEFILE)(
								  HANDLE hFile,
								  LPCVOID lpBuffer,
								  DWORD nNumberOfBytesToWrite,
								  LPDWORD lpNumberOfBytesWritten,
								  LPOVERLAPPED lpOverlapped
								  );
typedef BOOL (WINAPI *PCLOSEHANDLE)(
									HANDLE hObject
									);
typedef BOOL (WINAPI *PREADFILE)(
								 HANDLE hFile,
								 LPVOID lpBuffer,
								 DWORD nNumberOfBytesToRead,
								 LPDWORD lpNumberOfBytesRead,
								 LPOVERLAPPED lpOverlapped
								 );
typedef LONG (WINAPI *PREGCREATEKEYEXW)(
										HKEY hKey,
										LPCTSTR lpSubKey,
										DWORD Reserved,
										LPTSTR lpClass,
										DWORD dwOptions,
										REGSAM samDesired,
										LPSECURITY_ATTRIBUTES lpSecurityAttributes,
										PHKEY phkResult,
										LPDWORD lpdwDisposition
										);
typedef LONG (WINAPI *PREGOPENKEYEXW)(
									  HKEY hKey,
									  LPCTSTR lpSubKey,
									  DWORD ulOptions,
									  REGSAM samDesired,
									  PHKEY phkResult
									  );
typedef LONG (WINAPI* PREGCLOSEKEY)(
									HKEY hKey
									);
typedef LONG (WINAPI* PREGDELETEKEYW)(
									  HKEY hKey,
									  LPCTSTR lpSubKey
									  );
typedef LONG (WINAPI* PREGDELETETREEW)(
									   HKEY hKey,
									   LPCTSTR lpSubKey
									   );
typedef LONG (WINAPI* PREGDELETEVALUEW)(
										HKEY hKey,
										LPCTSTR lpValueName
										);
typedef LONG (WINAPI* PREGDELETEKEYVALUEW)(
	HKEY hKey,
	LPCTSTR lpSubKey,
	LPCTSTR lpValueName
	);
typedef LONG (WINAPI* PREGREPLACEKEYW)(
									   HKEY hKey,
									   LPCTSTR lpSubKey,
									   LPCTSTR lpNewFile,
									   LPCTSTR lpOldFile
									   );
typedef LONG (WINAPI* PREGRESTOREKEYW)(
									   HKEY hKey,
									   LPCTSTR lpFile,
									   DWORD dwFlags
									   );
typedef LONG (WINAPI* PREGSAVEKEYW)(
									HKEY hKey,
									LPCTSTR lpFile,
									LPSECURITY_ATTRIBUTES lpSecurityAttributes
									);
typedef LONG (WINAPI* PREGSAVEKEYEXW)(
									  HKEY hKey,
									  LPCTSTR lpFile,
									  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
									  DWORD Flags
									  );
typedef LONG (WINAPI* PREGGETVALUEW)( 
									 HKEY hkey,
									 LPCTSTR lpSubKey,
									 LPCTSTR lpValue,
									 DWORD dwFlags,
									 LPDWORD pdwType,
									 PVOID pvData,
									 LPDWORD pcbData
									 );
typedef LONG (WINAPI* PREGLOADKEYW)(
									HKEY hKey,
									LPCTSTR lpSubKey,
									LPCTSTR lpFile
									);
typedef LONG (WINAPI* PREGSETKEYVALUEW)(
										HKEY hKey,
										LPCTSTR lpSubKey,
										LPCTSTR lpValueName,
										DWORD dwType,
										LPCVOID lpData,
										DWORD cbData
										);
typedef LONG (WINAPI* PREGSETVALUEEXW)(
									   HKEY hKey,
									   LPCTSTR lpValueName,
									   DWORD Reserved,
									   DWORD dwType,
									   const BYTE* lpData,
									   DWORD cbData
									   );
typedef LONG (WINAPI* PREGCOPYTREEW)(
									 HKEY hKeySrc,
									 LPCTSTR lpSubKey,
									 HKEY hKeyDest
									 );

extern HookedRputine HookedCreateFileW;
extern HookedRputine HookedWriteFile;
extern HookedRputine HookedReadFile;
extern HookedRputine HookedRegCreateKeyExW;
extern HookedRputine HookedRegDeleteKeyW;
extern HookedRputine HookedRegDeleteTreeW;
extern HookedRputine HookedRegDeleteValueW;
extern HookedRputine HookedRegDeleteKeyValueW;
extern HookedRputine HookedRegReplaceKeyW;
extern HookedRputine HookedRegRestoreKeyW;				
extern HookedRputine HookedRegSaveKeyW;
extern HookedRputine HookedRegSaveKeyExW;
extern HookedRputine HookedRegGetValueW;
extern HookedRputine HookedRegLoadKeyW;
extern HookedRputine HookedRegSetKeyValueW;
extern HookedRputine HookedRegSetValueExW;
extern HookedRputine HookedRegCopyTreeW;

void write(LPCWSTR func, LPCWSTR param)
{
	EnterCriticalSection(&fileWriteCriticalSection);
	WCHAR res[300] = L"";
	wcscat(res, func);
	wcscat(res, L" ");
	wcscat(res, param);
	wcscat(res, L"\r\n");
	((PWRITEFILE)HookedWriteFile.OriginalFunction)(file, res, (_tcslen(res) * sizeof(WCHAR)), &t, NULL);
	LeaveCriticalSection(&fileWriteCriticalSection);
}

HANDLE WINAPI Hook_CreateFileW(
							   LPCWSTR lpFileName,
							   DWORD dwDesiredAccess,
							   DWORD dwShareMode,
							   LPSECURITY_ATTRIBUTES lpSecurityAttributes,
							   DWORD dwCreationDisposition,
							   DWORD dwFlagsAndAttributes,
							   HANDLE hTemplateFile)  
{
	HANDLE result = ((PCREATEFILEW)HookedCreateFileW.OriginalFunction)(
		lpFileName,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);
	write(L"CreateFileW", lpFileName);
	return result;
};
BOOL WINAPI Hook_WriteFile(
						   HANDLE hFile,
						   LPCVOID lpBuffer,
						   DWORD nNumberOfBytesToWrite,
						   LPDWORD lpNumberOfBytesWritten,
						   LPOVERLAPPED lpOverlapped)
{
	BOOL result = ((PWRITEFILE)HookedWriteFile.OriginalFunction)(
		hFile,
		lpBuffer,
		nNumberOfBytesToWrite,
		lpNumberOfBytesWritten,
		lpOverlapped);
	WCHAR tmp[20];
	_itow(*lpNumberOfBytesWritten, tmp, 10);
	wcscat(tmp, L" bytes");
	write(L"WriteFile", tmp);
	return result;
}

BOOL WINAPI Hook_ReadFile(
						  HANDLE hFile,
						  LPVOID lpBuffer,
						  DWORD nNumberOfBytesToRead,
						  LPDWORD lpNumberOfBytesRead,
						  LPOVERLAPPED lpOverlapped)
{
	BOOL result = ((PREADFILE)HookedReadFile.OriginalFunction)(
		hFile,
		lpBuffer,
		nNumberOfBytesToRead,
		lpNumberOfBytesRead,
		lpOverlapped);
	WCHAR tmp[255];
	_itow(*lpNumberOfBytesRead, tmp, 200);
	wcscat(tmp, L" bytes");
	write(L"ReadFile", tmp);
	return result;
}
LONG WINAPI Hook_RegCreateKeyExW(
								 HKEY hKey,
								 LPCTSTR lpSubKey,
								 DWORD Reserved,
								 LPTSTR lpClass,
								 DWORD dwOptions,
								 REGSAM samDesired,
								 LPSECURITY_ATTRIBUTES lpSecurityAttributes,
								 PHKEY phkResult, 
								 LPDWORD lpdwDisposition)
{  
	LONG result = ((PREGCREATEKEYEXW)HookedRegCreateKeyExW.OriginalFunction)(
		hKey,
		lpSubKey,
		Reserved,
		lpClass,
		dwOptions,
		samDesired,
		lpSecurityAttributes,
		phkResult,
		lpdwDisposition);
	write(L"RegCreateKeyExW", lpSubKey);
	return result;
}

LONG WINAPI Hook_RegDeleteKeyW(
							   HKEY hKey,
							   LPCTSTR lpSubKey)
{
	LONG result = ((PREGDELETEKEYW)HookedRegDeleteKeyW.OriginalFunction)(hKey, lpSubKey);
	write(L"RegDeleteKeyW", lpSubKey);
	return result;
}
LONG WINAPI Hook_RegDeleteTreeW(
								HKEY hKey,
								LPCTSTR lpSubKey)
{
	LONG result = ((PREGDELETETREEW)HookedRegDeleteTreeW.OriginalFunction)(hKey, lpSubKey);
	write(L"RegDeleteTreeW", lpSubKey);
	return result;
}
LONG WINAPI Hook_RegDeleteValueW(
								 HKEY hKey,
								 LPCTSTR lpValueName)
{
	LONG result = ((PREGDELETEVALUEW)HookedRegDeleteValueW.OriginalFunction)(hKey, lpValueName);
	write(L"RegDeleteValueW", lpValueName);
	return result;
}
LONG WINAPI Hook_RegDeleteKeyValueW(
									HKEY hKey,
									LPCTSTR lpSubKey,
									LPCTSTR lpValueName)
{
	LONG result = ((PREGDELETEKEYVALUEW)HookedRegDeleteKeyValueW.OriginalFunction)(hKey,
		lpSubKey, lpValueName);
	WCHAR res[300] = L"";
	wcscat(res, lpSubKey);
	wcscat(res, L" ");
	wcscat(res, lpValueName);
	write(L"RegDeleteKeyValueW", res);
	return result;
}
LONG WINAPI Hook_RegReplaceKeyW(
								HKEY hKey,
								LPCTSTR lpSubKey,
								LPCTSTR lpNewFile,
								LPCTSTR lpOldFile)
{
	LONG result = ((PREGREPLACEKEYW)HookedRegReplaceKeyW.OriginalFunction)(hKey,
		lpSubKey,
		lpNewFile,
		lpOldFile);
	WCHAR res[300] = L"";
	wcscat(res, lpSubKey);
	wcscat(res, L" ");
	wcscat(res, lpNewFile);
	wcscat(res, L" ");
	wcscat(res, lpOldFile);
	write(L"RegReplaceKeyW", res);
	return result;
}
LONG WINAPI Hook_RegRestoreKeyW(
								HKEY hKey,
								LPCTSTR lpFile,
								DWORD dwFlags)
{
	LONG result = ((PREGRESTOREKEYW)HookedRegRestoreKeyW.OriginalFunction)(
		hKey,
		lpFile,
		dwFlags);
	write(L"RegRestoreKeyW", lpFile);
	return result;
}
LONG WINAPI Hook_RegSaveKeyW(
							 HKEY hKey,
							 LPCTSTR lpFile,
							 LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	LONG result = ((PREGSAVEKEYW)HookedRegSaveKeyW.OriginalFunction)(
		hKey,
		lpFile,
		lpSecurityAttributes);
	write(L"RegSaveKeyW", lpFile);
	return result;   
} 
LONG WINAPI Hook_RegSaveKeyExW(
							   HKEY hKey,
							   LPCTSTR lpFile,
							   LPSECURITY_ATTRIBUTES lpSecurityAttributes,
							   DWORD Flags)
{
	LONG result = ((PREGSAVEKEYEXW)HookedRegSaveKeyExW.OriginalFunction)(
		hKey,
		lpFile,
		lpSecurityAttributes, Flags);
	write(L"RegSaveKeyExW", lpFile);
	return result;
}
LONG WINAPI Hook_RegGetValueW(
							  HKEY hkey,
							  LPCTSTR lpSubKey,
							  LPCTSTR lpValue,
							  DWORD dwFlags,
							  LPDWORD pdwType,
							  PVOID pvData,
							  LPDWORD pcbData)
{
	LONG result = ((PREGGETVALUEW)HookedRegGetValueW.OriginalFunction)(
		hkey,
		lpSubKey,
		lpValue,
		dwFlags,
		pdwType,
		pvData,
		pcbData);
	WCHAR res[300] = L"";
	wcscat(res, lpSubKey);
	wcscat(res, L" ");
	wcscat(res, lpValue);
	write(L"RegGetValueW", res);
	return result;
}   
LONG WINAPI Hook_RegLoadKeyW(
							 HKEY hKey,
							 LPCTSTR lpSubKey,
							 LPCTSTR lpFile)
{
	LONG result = ((PREGLOADKEYW)HookedRegLoadKeyW.OriginalFunction)(
		hKey,
		lpSubKey,
		lpFile);
	WCHAR res[300] = L"";
	wcscat(res, lpSubKey);
	wcscat(res, L" ");
	wcscat(res, lpFile);
	write(L"RegLoadKeyW", res);
	return result;
}
LONG WINAPI Hook_RegSetKeyValueW(
								 HKEY hKey,
								 LPCTSTR lpSubKey,
								 LPCTSTR lpValueName,
								 DWORD dwType,
								 LPCVOID lpData,
								 DWORD cbData)
{
	LONG result = ((PREGSETKEYVALUEW)HookedRegSetKeyValueW.OriginalFunction)(
		hKey,
		lpSubKey,
		lpValueName,
		dwType,
		lpData,
		cbData);
	WCHAR res[300] = L"";
	wcscat(res, lpSubKey);
	wcscat(res, L" ");
	wcscat(res, lpValueName);
	write(L"RegSetKeyValueW", res);
	return result;
}
LONG WINAPI Hook_RegSetValueExW(
								HKEY hKey,
								LPCTSTR lpValueName,
								DWORD Reserved,
								DWORD dwType,
								const BYTE* lpData,
								DWORD cbData)
{
	LONG result = ((PREGSETVALUEEXW)HookedRegSetValueExW.OriginalFunction)(
		hKey,
		lpValueName,
		Reserved,
		dwType,
		lpData,
		cbData);
	write(L"RegSetValueExW", lpValueName);
	return result;
}
LONG WINAPI Hook_RegCopyTreeW(
							  HKEY hKeySrc,
							  LPCTSTR lpSubKey,
							  HKEY hKeyDest)
{
	LONG result = ((PREGCOPYTREEW)HookedRegCopyTreeW.OriginalFunction)(
		hKeySrc,
		lpSubKey,
		hKeyDest);
	write(L"RegCopyTreeW", lpSubKey);
	return result;
}

HookedRputine HookedCreateFileW("Kernel32.dll", "CreateFileW",(PROC)Hook_CreateFileW);
HookedRputine HookedWriteFile("Kernel32.dll", "WriteFile", (PROC)Hook_WriteFile);
HookedRputine HookedReadFile("Kernel32.dll", "ReadFile", (PROC)Hook_ReadFile);
HookedRputine HookedRegCreateKeyExW("Advapi32.dll", "RegCreateKeyExW", (PROC)Hook_RegCreateKeyExW);
HookedRputine HookedRegDeleteKeyW("Advapi32.dll", "RegDeleteKeyW", (PROC)Hook_RegDeleteKeyW);
HookedRputine HookedRegDeleteTreeW("Advapi32.dll", "RegDeleteTreeW", (PROC)Hook_RegDeleteTreeW);
HookedRputine HookedRegDeleteValueW("Advapi32.dll", "RegDeleteValueW", (PROC)Hook_RegDeleteValueW);
HookedRputine HookedRegDeleteKeyValueW("Advapi32.dll", "RegDeleteKeyValueW", (PROC)Hook_RegDeleteKeyValueW);
HookedRputine HookedRegReplaceKeyW("Advapi32.dll", "RegReplaceKeyW", (PROC)Hook_RegReplaceKeyW);
HookedRputine HookedRegRestoreKeyW("Advapi32.dll", "RegRestoreKeyW", (PROC)Hook_RegRestoreKeyW);
HookedRputine HookedRegSaveKeyW("Advapi32.dll", "RegSaveKeyW", (PROC)Hook_RegSaveKeyW);
HookedRputine HookedRegSaveKeyExW("Advapi32.dll", "RegSaveKeyExW", (PROC)Hook_RegSaveKeyExW);
HookedRputine HookedRegGetValueW("Advapi32.dll", "RegGetValueW", (PROC)Hook_RegGetValueW);
HookedRputine HookedRegLoadKeyW("Advapi32.dll", "RegLoadKeyW", (PROC)Hook_RegLoadKeyW);
HookedRputine HookedRegSetKeyValueW("Advapi32.dll", "RegSetKeyValueW", (PROC)Hook_RegSetKeyValueW);
HookedRputine HookedRegSetValueExW("Advapi32.dll", "RegSetValueExW", (PROC)Hook_RegSetValueExW);
HookedRputine HookedRegCopyTreeW("Advapi32.dll", "RegCopyTreeW", (PROC)Hook_RegCopyTreeW);


BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, PVOID fImpLoad) {

	if (fdwReason == DLL_PROCESS_ATTACH) {
		InitializeCriticalSection(&fileWriteCriticalSection);
		file = ((PCREATEFILEW)HookedCreateFileW.OriginalFunction)(L"C:\\results.txt", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, NULL, NULL);
	}
	if (fdwReason == DLL_PROCESS_DETACH) {
		DeleteCriticalSection(&fileWriteCriticalSection);
		CloseHandle(file);
	}
	return true;
}
