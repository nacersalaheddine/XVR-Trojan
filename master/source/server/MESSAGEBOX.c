#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "server/serv_cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "logger.h"
#include "types.h"
#include "cmp.h"

#define SERV_MESSAGEBOX_MBOX_FAILED 0xF
#define SERV_MESSAGEBOX_MBOX_MAX 200

typedef struct _serv_mbox_info
{
	uint8 type;
	char title[SERV_MESSAGEBOX_MBOX_MAX];
	char msg[SERV_MESSAGEBOX_MBOX_MAX];
}SERV_MBOX_INFO;

int serv_MESSAGEBOX(char* msg)
{
	int stIdx = 0;
	char* argTitle;
	char* argMsg;

	if(!(stIdx = serv_cmds_ExportArg(&msg, &argTitle, 0)))
	{
		LOG(LOG_ERR, "Invalid arguments!\n");
		free(msg);

		return SERV_CMDS_DONT_SEEDUP;
	}
	
	if(!(stIdx = serv_cmds_ExportArg(&msg, &argMsg, stIdx)))
	{
		LOG(LOG_ERR, "Invalid arguments!\n");
		free(argTitle);
		free(msg);

		return SERV_CMDS_DONT_SEEDUP;
	}

	if(strlen(argTitle) >= SERV_MESSAGEBOX_MBOX_MAX || strlen(argMsg) >= SERV_MESSAGEBOX_MBOX_MAX)
	{
		LOG(LOG_ERR, "Title or the message are too long! Max size %d\n", SERV_MESSAGEBOX_MBOX_MAX);
		free(argTitle);
		free(argMsg);
		free(msg);

		return SERV_CMDS_DONT_SEEDUP;
	}

	int len = strlen(msg + stIdx);
	char* _type = malloc(len + sizeof(char));
	memset(_type, 0, len + sizeof(char));
	strcpy(_type, msg + stIdx);
	free(msg);

	SERV_MBOX_INFO mInfo;
	memset(&mInfo, 0, sizeof(SERV_MBOX_INFO));
	strcpy(mInfo.title, argTitle);
	strcpy(mInfo.msg, argMsg);

	free(argTitle);
	free(argMsg);

	if(cmp_cmp(_type, "error") || cmp_cmp(_type, "err") | cmp_cmp(_type, "x"))
	{
		mInfo.type = MB_ICONERROR;
	}else if(cmp_cmp(_type, "warning") | cmp_cmp(_type, "war") | cmp_cmp(_type, "!")){
		mInfo.type = MB_ICONWARNING;
	}else if(cmp_cmp(_type, "question") | cmp_cmp(_type, "que") | cmp_cmp(_type, "?")){
		mInfo.type = MB_ICONQUESTION;
	}else if(cmp_cmp(_type, "info") | cmp_cmp(_type, "i")){
		mInfo.type = MB_ICONINFORMATION;
	}else if(cmp_cmp(_type, "none") | cmp_cmp(_type, "non")){
		mInfo.type = 0x0;
	}else{
		LOG(LOG_ERR, "Invalid message type!\n");
		LOG(LOG_INFO, "You can use:\n");
		LOG(LOG_TABLE, "error; err; x\n");
		LOG(LOG_TABLE, "warning; war; !\n");
		LOG(LOG_TABLE, "question; que; ?\n");
		LOG(LOG_TABLE, "info; i\n");
		LOG(LOG_TABLE, "none; non\n");

		free(_type);

		return SERV_CMDS_DONT_SEEDUP;
	}

	free(_type);

	if(net_SendCmd((uint8*)&mInfo, sizeof(SERV_MBOX_INFO), SERV_CMDS_MESSAGE_BOX) < 1)
	{
		net_PrintLastError();
		
		return SERV_CMDS_BREAK;
	}

	uint8* rmsg;

	if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
	{
		net_PrintLastError();
		
		return SERV_CMDS_BREAK;
	}

	if(rmsg[0] != SERV_CMDS_MESSAGE_BOX)
	{
		LOG(LOG_ERR, "Failed to create MessageBox!\n");
	}else{
		putchar('\b');
	}

	free(rmsg);
	
	return SERV_CMDS_GOOD;
}