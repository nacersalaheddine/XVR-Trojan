#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "version.h"
#include "logger.h"
#include "cmp.h"
#include "SCL2.h"
#include "sc/sc.h"
#include "whitelist.h"
#include "blocklist.h"
#include "server.h"
#include "commands/cmds.h"
#include "geoIP.h"
#include "progressbar.h"
#include "input.h"

void main_printStuff(void)
{
	printf("\n                      /#\\\n                     /##=\\\n                    /##===\\\n                  /###==#===\\       Made by Xerados\n                /####===##====\\     Ver: %s\n              /#####====###=====\\\n             |#####=====####=====|\n             |#####=====####=====|\n              \\\\####=   ###=====//\n                     |##=|\n                   |####===|\n\n", VERSION);
}

void main_onExit(void)
{
	LOG_NEWLINE();
	LOG(LOG_WAR, "Destroying SCL2...\n");
	SCL2_Destroy();

	LOG(LOG_WAR, "Destroying server...\n");
	server_Cleanup();

	LOG(LOG_WAR, "Freeing libraries...\n");
	SC_FreeLibrary();

	LOG(LOG_WAR, "Restroring console colors...\n");
	LOG_SetColor(LOG_COLOR_ORIGINAL);
	LOG_CursorStatus(1);
	LOG_ResetTitle();

	LOG(LOG_SUCC, "Stoped!\n");

	exit(0);
}

int main(int argc, char* args[])
{
	if(!GetStdHandle(STD_OUTPUT_HANDLE))
	{
		AllocConsole();
	}

	atexit(main_onExit);
	LOG_Init();

	int i;
	
	for(i = 1; i != argc; i++)
	{
		if(cmp_cmp(args[i], LOG_OPT_COLOR))
		{
			log_op_Color = 1;
			continue;
		}else if(cmp_cmp(args[i], LOG_OPT_TIME)){
			log_op_Time = 1;
			continue;
		}else if(cmp_cmp(args[i], PROGRESSBAR_OPT_DONT_USE)){
			prgs_op_Use = 0;
			continue;
		}
	}


	int rvlog = LOG_LoadConfig();

	main_printStuff();
	LOG_LoadConfig_OutError(rvlog);

	_scl_info SCL_INFO;
	memset(&SCL_INFO, 0, sizeof(_scl_info));
	SCL_INFO.size = sizeof(_scl_info);
	SCL_INFO.seed = 0xFF83DA;
	SCL_INFO.upSeed = 0x3FD3F;
	SCL_INFO.upMax = 0xFF;
	SCL_INFO.randSalt = 0x10FA;
	SCL_INFO.ranHelper = 0x2F;

	int sclKeyLen = sizeof(_scl_key) + SCL2_DEF_KEY_LEN + SCL2_DEF_KEY_LEN; //struct size + key size + salt size
	_scl_key *SCL_KEY = (_scl_key*)malloc(sclKeyLen);
	memset(SCL_KEY, 0, sclKeyLen);

	SCL_KEY->size = sclKeyLen;
	SCL_KEY->times = 15;
	SCL_KEY->keyLen = SCL2_DEF_KEY_LEN;
	SCL_KEY->key = malloc(SCL2_DEF_KEY_LEN);
	SCL_KEY->salt = malloc(SCL2_DEF_KEY_LEN);
	memcpy(SCL_KEY->key, SCL2_DEF_KEY_KEY, SCL2_DEF_KEY_LEN);
	memcpy(SCL_KEY->salt, SCL2_DEF_KEY_SALT, SCL2_DEF_KEY_LEN);
	SCL2_Init(SCL_INFO, &SCL_KEY);
	SCL2_PrintInfo();
	
	SC_LoadLibrary();
	whitelist_Init();
	blocklist_Init();

	LOG(LOG_INFO, "Starting WSA...\n");

	int rv = server_WsaInit();

	if(rv != SERVER_NO_ERROR)
	{
		LOG(LOG_CRITICAL, "Failed to start WSA!\n");

		exit(-2);
	}

	if(geoIP_IsInUse)
	{
		LOG(LOG_INFO, "GeoIP:\n");
		LOG(LOG_TABLE, "Using: %s\n", GEOIP_HOST);

		rv = geoIP_Init();

		if(rv != GEOIP_NO_ERROR)
		{
			LOG(LOG_ERR, "Failed to prepare GeoIP!\n");
		}
	}

	LOG(LOG_WAR, "Your console font my glitch the progressbar!\n");
	LOG(LOG_TABLE, "If happend chage your font or the char.\n");

	if(!rvlog)
	{
		LOG_NEWLINE();
		LOG(LOG_WAR, "First time ? Use \"help\" for help\n");
	}

	input_Gets_Init();

	cmds_Loop();

	exit(0);
}
