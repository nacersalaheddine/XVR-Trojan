#ifndef __SCREENSHOT_H
#define __SCREENSHOT_H

#include <windows.h>

typedef void* (WINAPI *__SelectObject)(void*, void*);
typedef void* (WINAPI *__CreateCompatibleDC)(void*);
typedef void* (WINAPI *__CreateCompatibleBitmap)(void*, int, int);
typedef int (WINAPI *__SetStretchBltMode)(void*, int);
typedef int (WINAPI *__StretchBlt)(void*, int, int, int, int, void*, int, int, int, int, unsigned long);
typedef int (WINAPI *__GetDIBits)(void*, void*, unsigned int, unsigned int, void*, LPBITMAPINFO, unsigned int);
typedef int (WINAPI *__DeleteObject)(void*);
typedef int (WINAPI *__DeleteDC)(void*);

extern void screenshot_Init(void);
extern unsigned int screenshot_Calculate(unsigned int* scW, unsigned int* scH, unsigned int perW, unsigned int perH);
extern unsigned int screenshot_Take(unsigned int perW, unsigned int perH, unsigned char** data);

#endif