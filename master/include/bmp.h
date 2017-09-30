#ifndef __BMP_H
#define __BMP_H

#define BITMAP_INFO_HEADER BITMAPINFOHEADER
#define BITMAP_FILE_HEADER BITMAPFILEHEADER

extern int bmp_isCompressed;
extern void bmp_UncompressData(unsigned char** data, int width, int height);
extern int bmp_Create(char* fname, void* data, int width, int height, int hidden);

#endif