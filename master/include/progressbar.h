#ifndef __PROGRESSBAR_H
#define __PROGRESSBAR_H

#define PROGRESSBAR_LEN 30.0
#define PROGRESSBAR_TIMER 1000

#define COLOR_WHIE_BACK 0xF0
#define COLOR_RESET_BACK 0x0F

#define PROGRESSBAR_CLEAR() progressbar_Max = 0; progressbar_Index = 0

extern int progressbar_Running;
extern unsigned long progressbar_Max;
extern unsigned long progressbar_Index;
extern void progressbar_Start(void);
extern void progressbar_WaitToStart(void);
extern void progressbar_Stop(void);
extern void progressbar_CriticalStop(void);

#endif