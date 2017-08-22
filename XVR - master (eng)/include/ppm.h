#ifndef __PPM_H
#define __PPM_H

extern FILE *ppm_f;
extern void ppm_Close(void);
extern int ppm_Create(char* fpath, int W, int H);
extern int ppm_Append(unsigned char* cont, int contLen);

#endif