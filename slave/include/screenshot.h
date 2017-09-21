#ifndef __SCREENSHOT_H
#define __SCREENSHOT_H

extern void screenshot_Init();
extern unsigned char* screenshot_Take(double mX, double mY, int* width, int* height);

#endif