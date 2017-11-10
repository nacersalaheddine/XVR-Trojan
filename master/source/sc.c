#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "types.h"
#include "logger.h"
#include "sc/sc.h"
#include "sc/error.h"

int SC_CanRun = 0;

HMODULE sc_dll;
int(*_SC_Lib_Start)(void);
void(*_SC_Lib_Stop)(void);
void(*_SC_Lib_SetImage)(unsigned char*, unsigned int);
int(*_SC_Lib_IsImageUpdate)(void);
int(*_SC_Lib_GetLastError)(void);

int SC_Start(void)
{
	if(!sc_dll)
	{
		return 0;
	}
	
	return _SC_Lib_Start();
}

void SC_Stop(void)
{
	if(!sc_dll)
	{
		return;
	}

	_SC_Lib_Stop();
}

void SC_SetImage(uint8* data, uint32 len)
{
	if(!sc_dll)
	{
		return;
	}

	_SC_Lib_SetImage(data, len);
}

int SC_GetLastError(void)
{
	if(!sc_dll)
	{
		return 0;
	}

	return _SC_Lib_GetLastError();
}

int SC_loadFunctions(void)
{
	_SC_Lib_Start = (void*)GetProcAddress(sc_dll, "SC_Start");

	if(!_SC_Lib_Start)
	{
		return 0;
	}

	_SC_Lib_Stop = (void*)GetProcAddress(sc_dll, "SC_Stop");

	if(!_SC_Lib_Stop)
	{
		return 0;
	}

	_SC_Lib_SetImage = (void*)GetProcAddress(sc_dll, "SC_SetImage");

	if(!_SC_Lib_SetImage)
	{
		return 0;
	}

	_SC_Lib_GetLastError = (void*)GetProcAddress(sc_dll, "SC_GetLoopLastError");

	if(!_SC_Lib_GetLastError)
	{
		return 0;
	}

	return 1;
}

void SC_FreeLibrary(void)
{
	if(!sc_dll)
	{
		return;
	}

	if(!FreeLibrary(sc_dll))
	{
		LOG(LOG_ERR, "Failed to unload \"%s\"", SC_LIB_XVR_SC);
	}
}

void SC_LoadLibrary(void)
{
	if(sc_dll)
	{
		SC_FreeLibrary();
	}

	LOG(LOG_INFO, "Searching for \"%s\"\n", SC_LIB_SDL2);

	FILE *f = fopen(SC_LIB_SDL2, "r");

	if(!f)
	{
		LOG(LOG_WAR, "Didn't found \"%s\"\n", SC_LIB_SDL2);
		LOG(LOG_TABLE, "Download their binary (%s) from their website \"%s\"\n", SC_LIB_SDL2, SC_LIB_SDL2_WEBLINK);
		return;
	}

	fclose(f);

	LOG(LOG_INFO, "Searching for \"%s\"\n", SC_LIB_XVR_SC);

	f = fopen(SC_LIB_XVR_SC, "r");

	if(!f)
	{
		LOG(LOG_WAR, "Didn't found \"%s\"\n", SC_LIB_XVR_SC);

		return;
	}

	fclose(f);

	LOG(LOG_INFO, "Loading \"%s\"\n", SC_LIB_XVR_SC);

	sc_dll = LoadLibrary(SC_LIB_XVR_SC);

	if(!sc_dll)
	{
		LOG(LOG_ERR, "Failed to load \"%s\"\n", SC_LIB_XVR_SC);
	}else{
		if(SC_loadFunctions())
		{
			SC_CanRun = 1;

			LOG(LOG_SUCC, "Loaded!\n");
		}else{
			LOG(LOG_ERR, "Failed to load \"%s\"\n", SC_LIB_XVR_SC);
		}
	}
}