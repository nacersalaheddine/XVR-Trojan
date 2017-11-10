#ifndef __WINMAIN_H
#define __WINMAIN_H

#define WINMAIN_SIGIL_LEN 7
#define WINMAIN_SIGIL "\x74\x69\x7E\x6D\x68\x63\x7F"

#include <stdio.h>

extern int winmain_IsInfected(char* path, FILE** of);
extern void winmain_Move(FILE *f);

#endif