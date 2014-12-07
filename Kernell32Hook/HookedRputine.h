#pragma once

class HookedRputine
{
public:
	HookedRputine(PSTR dllName, PSTR functionName, PROC newFunction);
	~HookedRputine(void);
	PROC OriginalFunction;
private:
	PCSTR dllName;     
	PCSTR funcName;          
	PROC newFunction;              
};
