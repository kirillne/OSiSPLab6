#include "StdAfx.h"
#include "HookedRputine.h"
#include <tlhelp32.h>
#include <ImageHlp.h>
#pragma comment(lib, "ImageHlp")


static void HookInOneModul(PCSTR moduleName,PROC currentFunc, PROC newFunc, HMODULE hMod) 
{
	ULONG ulSize;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc = NULL;
	try 
	{
		pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR) ImageDirectoryEntryToData(
			hMod, TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);
	} 
	catch (...) { }
	if (pImportDesc == NULL)
		return;  

	for (; pImportDesc->Name; pImportDesc++) 
	{
		PSTR pszModName = (PSTR) ((PBYTE) hMod + pImportDesc->Name);
		if (lstrcmpiA(pszModName, moduleName) == 0) 
		{
			PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA)
				((PBYTE) hMod + pImportDesc->FirstThunk);

			for (; pThunk->u1.Function; pThunk++) 
			{
				PROC* ppfn = (PROC*) &pThunk->u1.Function;
				BOOL bFound = (*ppfn == currentFunc);
				if (bFound) 
				{
					if (!WriteProcessMemory(GetCurrentProcess(), ppfn, &newFunc, 
						sizeof(newFunc), NULL) && (GetLastError() == ERROR_NOACCESS)) 
					{
						DWORD dwOldProtect;
						if (VirtualProtect(ppfn, sizeof(newFunc), PAGE_WRITECOPY, 
							&dwOldProtect)) 
						{
							WriteProcessMemory(GetCurrentProcess(), ppfn, &newFunc, 
								sizeof(newFunc), NULL);
							VirtualProtect(ppfn, sizeof(newFunc), dwOldProtect, 
								&dwOldProtect);
						}
					}
					return;  
				}
			}
		}  
	}
}

static void HookInAllModuls(PCSTR moduleName, PROC currentFunc, PROC newFunc) 
{
	MEMORY_BASIC_INFORMATION mbi;
	HMODULE hmodThisMod = (VirtualQuery(HookInAllModuls, &mbi, sizeof(mbi)) != 0) 
		? (HMODULE) mbi.AllocationBase : NULL;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());

	MODULEENTRY32 me = { sizeof(me) };
	for (BOOL bOk = Module32First(hSnapshot, &me); bOk; bOk = Module32Next(hSnapshot, &me)) 
	{
		if (me.hModule != hmodThisMod) 
		{
			HookInOneModul(moduleName, currentFunc, newFunc, me.hModule);
		}
	}
}
HookedRputine::HookedRputine(PSTR dllName, PSTR functionName, PROC newFunction)
{
	this->dllName = dllName;
	this->funcName = functionName;
	this->newFunction = newFunction;
	OriginalFunction = GetProcAddress(GetModuleHandleA(dllName),functionName);
	HookInAllModuls(dllName, OriginalFunction, newFunction);
}


HookedRputine::~HookedRputine(void)
{
	HookInAllModuls(dllName, newFunction, OriginalFunction);
}

