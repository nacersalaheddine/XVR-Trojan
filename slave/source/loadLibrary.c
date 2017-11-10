#include <stdlib.h>
#include <windows.h>
#include "types.h"
#include "memProtect.h"
#include "loadLibrary.h"

void* loadLibrary_Load(int idx)
{
	char* temp = (char*)memProtect_GetLibData(idx);

	if(!temp)
	{
		return NULL;
	}

	HMODULE hmod = LoadLibrary(temp);

	free(temp);

	return hmod;
}

lib_FARPROC loadLibrary_LoadFunc(void* hmod, int idx)
{
	if(!hmod)
	{
		return NULL;
	}

	char* temp = (char*)memProtect_GetLibData(idx);
	
	if(!temp)
	{
		return NULL;
	}

	lib_FARPROC fproc = (lib_FARPROC)GetProcAddress(hmod, temp);
	
	free(temp);

	return fproc;
}