// Lab6.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"


bool WINAPI InjectDll(HANDLE hProcess, LPCWSTR newdDllName)
{
	BOOL result = false; 
	HANDLE hThread = NULL;
	PWSTR newdDllNameRemote = NULL;

	__try 
	{
		int dllNameLen  = (1 + lstrlenW(newdDllName)) * sizeof(wchar_t);

		newdDllNameRemote = (PWSTR)VirtualAllocEx(hProcess, NULL, dllNameLen, 
			MEM_COMMIT, PAGE_READWRITE);
		if (newdDllNameRemote == NULL) 
			__leave;

		if (!WriteProcessMemory(hProcess, newdDllNameRemote,(PVOID)newdDllName, 
			dllNameLen, NULL)) 
			__leave;

		PTHREAD_START_ROUTINE pfnThreadRtn = (PTHREAD_START_ROUTINE)
			GetProcAddress(GetModuleHandle(L"Kernel32"), "LoadLibraryW");
		if (pfnThreadRtn == NULL) __leave;

		hThread = CreateRemoteThread(hProcess, NULL, 0, pfnThreadRtn, newdDllNameRemote, 0, NULL);
		if (hThread == NULL) __leave;

		WaitForSingleObject(hThread, INFINITE);
		result = TRUE; 
	}
	__finally 
	{ 
		if (newdDllNameRemote != NULL) 
			VirtualFreeEx(hProcess, newdDllNameRemote, 0, MEM_RELEASE);
		if (hThread  != NULL) 
			CloseHandle(hThread);
		if (hProcess != NULL) 
			CloseHandle(hProcess);

	}
	return(result);

}

int _tmain(int argc, _TCHAR* argv[])
{
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION piProcess;
	ZeroMemory( &startupInfo, sizeof(startupInfo) );
	startupInfo.cb = sizeof(startupInfo);
	ZeroMemory( &piProcess, sizeof(piProcess) );
	SECURITY_ATTRIBUTES saProcess, saThread;
	saProcess.nLength = sizeof(saProcess);
	saProcess.lpSecurityDescriptor = NULL;
	saProcess.bInheritHandle = TRUE;
	saThread.nLength = sizeof(saThread);
	saThread.lpSecurityDescriptor = NULL;
	saThread.bInheritHandle = FALSE;

	TCHAR commandLine[] = L"";
	TCHAR applicationPath[] = L"C:\\Windows\\system32\\notepad.exe";

	if (CreateProcess(applicationPath, commandLine, 
		 &saProcess, &saThread, FALSE, 0, NULL, NULL, &startupInfo, &piProcess))
	{
		std::cout<<"Process starts"  << std::endl;
	}
	else
	{
		std::cout<<"Process name is incorrect " << GetLastError() << std::endl;
		return 1;
	}
	
	if (InjectDll(piProcess.hProcess, L"C:\\Projects\\Lab6\\Debug\\Kernell32Hook.dll"))
	{
		std::cout<<"Dll injected";
	}
	else
	{
		std::cout<<"Injecting error " << GetLastError() << std::endl;
	}
	/*LoadLibraryW(L"Kernell32Hook.dll");
	CreateFile(L"C:\\asas",GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, NULL, NULL);
	*/
	char a;
	std::cin >> a; 
	return 0;
}

