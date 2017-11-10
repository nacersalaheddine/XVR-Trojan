#ifndef __CMDS_H
#define __CMDS_H

#define CMDS_SEND 0x1
#define CMDS_INFO 0x2
#define CMDS_TERMINATE 0x3
#define CMDS_SYSTEM 0x4
#define CMDS_TASK_LIST 0x5
#define CMDS_TASK_KILL 0x6
#define CMDS_DISK_LIST 0x7
#define CMDS_HDD_CD 0x8
#define CMDS_HDD_LS 0x9
#define CMDS_HDD_REMOVE 0xB
#define CMDS_FILE_SEND 0xC
#define CMDS_FILE_GET 0xD
#define CMDS_MESSAGE_BOX 0xE
#define CMDS_SELF_DEL 0xF
#define CMDS_SCREEN_GET 0x10
#define CMDS_KEYLOGGER_INFO 0x11
#define CMDS_KEYLOGGER_CLEAR 0x12
#define CMDS_KEYLOGGER_GET 0x13
#define CMDS_SCREEN_CAP 0x14

extern int cmds_Translate(unsigned char* msg);
extern int cmds_SEND(unsigned char* msg);
extern int cmds_TERMINATE(void);
extern int cmds_INFO(void);
extern int cmds_SYSTEM(unsigned char* msg);
extern int cmds_TASKLIST(void);
extern int cmds_TASKKILL(unsigned char* msg);
extern int cmds_DISKLIST(void);
extern int cmds_CD(unsigned char* msg);
extern int cmds_LS(unsigned char* msg);
extern int cmds_REMOVE(unsigned char* msg);
extern int cmds_FILESEND(unsigned char* msg);
extern int cmds_FILEGET(unsigned char* msg);
extern int cmds_MESSAGEBOX(unsigned char* msg);
extern int cmds_SELFDEL(void);
extern int cmds_SCREENGET(unsigned char* msg);
extern int cmds_KEYLOGGER_INFO(void);
extern int cmds_KEYLOGGER_CLEAR(void);
extern int cmds_SCREENCAP(unsigned char* msg);
extern int cmds_KEYLOGGER_GET(void);

#endif