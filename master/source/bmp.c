#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "types.h"
#include "bmp.h"

void bmp_WriteHeader(FILE *f, uint32 width, uint32 height)
{
	BITMAP_INFO_HEADER bmpInfo = { sizeof(BITMAP_INFO_HEADER) };
	bmpInfo.biSizeImage = width * height * 3;
	bmpInfo.biWidth = width;
	bmpInfo.biHeight = height;
	bmpInfo.biPlanes = 1;
	bmpInfo.biBitCount = 24;
	bmpInfo.biCompression = BI_RGB;

	int bitsOff = sizeof(BITMAP_FILE_HEADER) + bmpInfo.biSize;
	uint32 bmpFSize = bitsOff + bmpInfo.biSizeImage;

	BITMAP_FILE_HEADER bmpFile;
	bmpFile.bfType = 'B' + ('M' << 8);
	bmpFile.bfOffBits = bitsOff;
	bmpFile.bfSize = bmpFSize;
	bmpFile.bfReserved1 = 0;
	bmpFile.bfReserved2 = 0;

	fwrite(&bmpFile, 1, sizeof(BITMAP_FILE_HEADER), f);
	fwrite(&bmpInfo, 1, sizeof(BITMAP_INFO_HEADER), f);
}

int bmp_CreateMemBmp(OUT_USTRP mem, uint32 width, uint32 height)
{
	BITMAP_INFO_HEADER bmpInfo = { sizeof(BITMAP_INFO_HEADER) };
	bmpInfo.biSizeImage = width * height * 3;
	bmpInfo.biWidth = width;
	bmpInfo.biHeight = height;
	bmpInfo.biPlanes = 1;
	bmpInfo.biBitCount = 24;
	bmpInfo.biCompression = BI_RGB;

	int bitsOff = sizeof(BITMAP_FILE_HEADER) + bmpInfo.biSize;
	uint32 bmpFSize = bitsOff + bmpInfo.biSizeImage;

	BITMAP_FILE_HEADER bmpFile;
	bmpFile.bfType = 'B' + ('M' << 8);
	bmpFile.bfOffBits = bitsOff;
	bmpFile.bfSize = bmpFSize;
	bmpFile.bfReserved1 = 0;
	bmpFile.bfReserved2 = 0;

	*mem = malloc(bmpFSize);
	memcpy(*mem, &bmpFile, sizeof(BITMAP_FILE_HEADER));
	memcpy(*mem + sizeof(BITMAP_FILE_HEADER), &bmpInfo, sizeof(BITMAP_INFO_HEADER));
	
	return sizeof(BITMAP_FILE_HEADER) + sizeof(BITMAP_INFO_HEADER);
}