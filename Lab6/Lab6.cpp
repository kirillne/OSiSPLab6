// Lab6.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	STARTUPINFO startupInfo;
	PROCESS_INFORMATION piProcess;
	ZeroMemory( &startupInfo, sizeof(startupInfo) );
	startupInfo.cb = sizeof(startupInfo);
	ZeroMemory( &piProcess, sizeof(piProcess) );

	TCHAR commandLine[] = L"";
	TCHAR applicationPath[] = L"C:\\Windows\\system32\\notepad.exe";

	if (CreateProcess(applicationPath, commandLine, 
		 NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &piProcess))
	{
		std::cout<<"Process starts"  << std::endl;
	}
	else
	{
		std::cout<<"Process name is incorrect " << GetLastError() << std::endl;
	}
	Sleep(10000);
	return 0;
}

