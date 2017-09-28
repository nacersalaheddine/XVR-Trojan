#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "types.h"
#include "logger.h"
#include "bmp.h"

int bmp_Create(char* fname, void* data, int width, int height, int hidden)
{
	BITMAP_INFO_HEADER bmpInfo = { sizeof(BITMAP_INFO_HEADER) };
	bmpInfo.biSizeImage = width * height * 3;
	bmpInfo.biWidth = width;
	bmpInfo.biHeight = height;
	bmpInfo.biPlanes = 1;
	bmpInfo.biBitCount = 24;
	bmpInfo.biCompression = BI_RGB;

	BITMAP_FILE_HEADER bmpFile;
	
	int bitsOff = sizeof(BITMAP_FILE_HEADER) + bmpInfo.biSize;
	long bmpFsize = bitsOff + bmpInfo.biSizeImage;

	bmpFile.bfType = 'B' + ('M' << 8);
	bmpFile.bfOffBits = bitsOff;
	bmpFile.bfSize = bmpFsize;
	bmpFile.bfReserved1 = 0;
	bmpFile.bfReserved2 = 0;

	if(!hidden)
	{
		LOG(LOG_INFO, "Creating BMP!\n");
	}

	FILE *f;

	if(!fname)
	{
		f = fopen("SCSHOT_NO_NAME.bmp", "wb");
	}else{
		f = fopen(fname, "wb");
	}

	if(!f)
	{
		LOG(LOG_ERR, "Failed to create BMP!\n");
		
		return 0;
	}

	uint32 countBmpFile = fwrite(&bmpFile, 1, sizeof(BITMAP_FILE_HEADER), f);
	uint32 countBmpInfo = fwrite(&bmpInfo, 1, sizeof(BITMAP_INFO_HEADER), f);
	uint32 countData = fwrite(data, 1, bmpInfo.biSizeImage, f);

	fclose(f);

	if(!countBmpFile || !countBmpInfo || !countData)
	{
		LOG(LOG_ERR, "Failed to create BMP!\n");

		return 0;
	}

	uint32 headerSize = countBmpFile + countBmpInfo;

	if(!hidden)
	{
		LOG(LOG_INFO, "Header: %d %s\n", (headerSize / 1024) < 1 ? headerSize : headerSize / 1024, (headerSize / 1024) < 1 ? "B" : "KB");
		LOG(LOG_INFO, "Data: %d %s\n", (countData / 1024) < 1 ? countData : countData / 1024, (countData / 1024) < 1 ? "B" : "KB");
		LOG(LOG_SUCC, "Done!\n");
	}

	return 1;
}