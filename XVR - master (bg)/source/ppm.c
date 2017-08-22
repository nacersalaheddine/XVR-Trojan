#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "types.h"

FILE *ppm_f;

void ppm_Close(void)
{
	fclose(ppm_f);
}

int ppm_Create(char* fpath, int W, int H)
{
	if(ppm_f)
	{
		fclose(ppm_f);
	}

	if(!(ppm_f = fopen(fpath, "wb")))
	{
		return -1;
	}

	fprintf(ppm_f, "P6\n%d %d\n255\n", W, H);

	return 1;
}

int ppm_Append(unsigned char* cont, int contLen)
{
	return fwrite(cont, 1, contLen, ppm_f);
}