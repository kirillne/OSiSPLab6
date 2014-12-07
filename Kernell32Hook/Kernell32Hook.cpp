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


extern HookedRputine HookedCreateFileW;
extern HookedRputine HookedWriteFile;

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



HookedRputine HookedCreateFileW("Kernel32.dll", "CreateFileW",(PROC)Hook_CreateFileW);
HookedRputine HookedWriteFile("Kernel32.dll", "WriteFile", (PROC)Hook_WriteFile);


BOOL WINAPI DllMain(HINSTANCE hInstDll, DWORD fdwReason, PVOID fImpLoad) {

	if (fdwReason == DLL_PROCESS_ATTACH) {
		InitializeCriticalSection(&fileWriteCriticalSection);
		file = ((PCREATEFILEW)HookedCreateFileW.OriginalFunction)(L"C:\\results.txt", GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, NULL, NULL);
	}
	/*if (fdwReason == DLL_PROCESS_DETACH) {
		DeleteCriticalSection(&fileWriteCriticalSection);
		CloseHandle(file);
	}*/
	return true;
}
