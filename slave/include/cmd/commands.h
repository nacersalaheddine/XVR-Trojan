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

#endif