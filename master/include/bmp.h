#ifndef __BMP_H
#define __BMP_H

#include <stdio.h>

#define BITMAP_INFO_HEADER BITMAPINFOHEADER
#define BITMAP_FILE_HEADER BITMAPFILEHEADER

#define BMP_GET_HEADERSIZE sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER)

extern void bmp_WriteHeader(FILE *f, unsigned int width, unsigned int height);
extern int bmp_CreateMemBmp(unsigned char** mem, unsigned int width, unsigned int height);

#endif