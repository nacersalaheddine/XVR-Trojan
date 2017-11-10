#ifndef __PROGRESSBAR_H
#define __PROGRESSBAR_H

#define PROGRESSBAR_OPT_DONT_USE "-noprgs"
#define PROGRESSBAR_TYPE_SIMPLE 0
#define PROGRESSBAR_TYPE_FILE 1
#define PROGRESSBAR_LEN 30.0

extern int prgs_op_Use;
extern void progressbar_Create(unsigned long long maxVal, int type);
extern void progressbar_Update(unsigned long long val);
extern void progressbar_End(void);

#endif