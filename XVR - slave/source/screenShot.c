#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

HDC backBuff;
HDC frontBuff;
HBITMAP backBM;
HBITMAP oldBackBM;

#define RGB_B(C) C & 0xFF
#define RGB_G(C) (C >> 8) & 0xFF
#define RGB_R(C) (C >> 16) & 0xFF

char* screenshot_Make(double mx, double my)
{
	int screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN); 
	int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN); 
	
	backBuff = GetDC(GetDesktopWindow());
	frontBuff = CreateCompatibleDC(backBuff);

	int matrix_x = screenWidth * mx;
	int matrix_y = screenHeight * my;

	backBM = CreateCompatibleBitmap(backBuff, matrix_x, matrix_y);
	oldBackBM = (HBITMAP)SelectObject(frontBuff, backBM);
	SetStretchBltMode(frontBuff, HALFTONE);
	StretchBlt(frontBuff, 0, 0, matrix_x, matrix_y, backBuff, 0, 0, screenWidth, screenHeight, SRCCOPY);
	//BitBlt(frontBuff, 0, 0, matrix, matrix, backBuff, 0, 0, SRCCOPY);

	int x;
	int y;
	char* appdata_path = getenv("APPDATA");
     char* screen_path = malloc(strlen(appdata_path) + 5 + sizeof(char));
	sprintf(screen_path, "%s\\test.ppm", appdata_path);
	FILE *f = fopen(screen_path, "wb");
	fprintf(f, "P6\n%d %d\n255\n", matrix_x, matrix_y);
	free(appdata_path);

	for(y = 0; y != matrix_y; y++)
	{
		for(x = 0; x != matrix_x; x++)
		{
			unsigned char colorarr[3];
			int color = GetPixel(frontBuff, x, y);
			colorarr[0] = RGB_B(color);
			colorarr[1] = RGB_G(color);
			colorarr[2] = RGB_R(color);
			fwrite(colorarr, 1, 3, f);
		}
	}

	fclose(f);

	return screen_path;
}