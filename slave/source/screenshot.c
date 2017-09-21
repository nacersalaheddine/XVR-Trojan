#include <stdlib.h>
#include <windows.h>
#include "types.h"
#include "screenshot.h"

HDC screenshot_backBuff;

void screenshot_Init(void)
{
	screenshot_backBuff = GetDC(GetDesktopWindow());
}

uint8* screenshot_Take(double mX, double mY, OUT_INT width, OUT_INT height)
{
	if(mX <= 1 || mY <= 1)
	{
		if(mX <= 1)
		{
			mX = 10;
		}

		if(mY <= 1)
		{
			mY = 10;
		}
	}

	if(!screenshot_backBuff)
	{
		return NULL;
	}

	int _scWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	int _scHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);

	int scWidth = (int)((double)_scWidth * (mX / 100.0));
	int scHeight = (int)((double)_scHeight * (mY / 100.0));
	
	*width = scWidth;
	*height = scHeight;

	HDC frontBuff = CreateCompatibleDC(screenshot_backBuff);
	HBITMAP backBM = CreateCompatibleBitmap(screenshot_backBuff, scWidth, scHeight);
	HBITMAP oldBackBM = (HBITMAP)SelectObject(frontBuff, backBM);
	SetStretchBltMode(frontBuff, HALFTONE);
	StretchBlt(frontBuff, 0, 0, scWidth, scHeight, screenshot_backBuff, 0, 0, _scWidth, _scHeight, SRCCOPY);

	BITMAPINFOHEADER bmpInfoHeader = { sizeof(BITMAPINFOHEADER) };
	bmpInfoHeader.biWidth = scWidth;
	bmpInfoHeader.biHeight = scHeight;
	bmpInfoHeader.biPlanes = 1;
	bmpInfoHeader.biBitCount = 24;

	uint8* rdata = malloc((scWidth * scHeight * 3) + sizeof(uint8));
	int rv = GetDIBits(screenshot_backBuff, backBM, 0, scHeight, rdata, (BITMAPINFO*)&bmpInfoHeader, DIB_RGB_COLORS);

	DeleteObject(backBM);
	DeleteObject(oldBackBM);
	DeleteDC(frontBuff);

	if(rv != scHeight)
	{
		free(rdata);

		return NULL;
	}

	return rdata;
}

