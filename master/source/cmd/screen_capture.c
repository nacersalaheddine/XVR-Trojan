#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "types.h"
#include "logger.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"
#include "bmp.h"

#define COMMAND_SCREEN_CAP_DATA 0xA3
#define COMMAND_SCREEN_CAP_END 0xD3
#define COMMAND_SCREEN_CAP_STOP 0xBC
#define COMMAND_SCREEN_CAP_CONTINUE 0xCB
#define COMMAND_SCREEN_CAP_KEY_ESC 0x1B
#define COMMAND_SCREEN_CAP_DEFAULT_INTERVAL 1000

#define COMMAND_SCREEN_CAP_TEMP_FILE "screen_cap_temp"

//i don't know JavaScript and i don't know how this works
char* command_Screen_CaptureView = "<html>\n<head>\n<title>XVR-ScreenCap</title>\n</head>\n<body>\n<img src=\"ScreenCap.bmp\" id=\"SC\">\n\n<script language=\"javascript\" type=\"text/javascript\" >\nfunction ChangeMedia()\n{\nvar img = document.getElementById(\'SC\')\nimg.src = img.src + \'?\' + Math.random();\n}\nsetInterval(\"ChangeMedia()\", 400);\n</script>\n</body>\n</html>\n";

int command_Screen_Capture_PlaceView(void)
{
	LOG(LOG_INFO, "Creating \"ScreenCapture.html\"...");

	FILE *f = fopen("ScreenCapture.html", "w");

	if(!f)
	{
		LOG(LOG_ERR, "Failed to create \"ScreenCapture.html\"\n");

		return 0;
	}

	fprintf(f, "%s", command_Screen_CaptureView);
	fclose(f);

	LOG(LOG_SUCC, "Created \"ScreenCapture.html\"\n");
	system("ScreenCapture.html");

	return 1;
}

int command_Screen_Capture(char* msg, int len)
{
	int argsIndex = 0;
	char* prntWidth;

	argsIndex = commands_ExportArg(&msg, &prntWidth, 0);

	if(!argsIndex)
	{
		free(prntWidth);
		free(msg);

		return COMMANDS_UNKNOW_COMMAND;
	}

	char* prntHeight;

	argsIndex = commands_ExportArg(&msg, &prntHeight, argsIndex);

	if(!argsIndex)
	{
		free(prntHeight);
		free(msg);

		return COMMANDS_UNKNOW_COMMAND;
	}

	uint32 interval = atoi(msg + argsIndex);
	uint32 scWidth = atoi(prntWidth);
	uint32 scHeight = atoi(prntHeight);
	free(msg);
	free(prntWidth);
	free(prntHeight);

	if(interval < 0)
	{
		interval = COMMAND_SCREEN_CAP_DEFAULT_INTERVAL;

		LOG(LOG_ERR, "Interval is invalid!\n");
		LOG(LOG_INFO, "Interval is set to default (%d)\n", COMMAND_SCREEN_CAP_DEFAULT_INTERVAL);
	}

	if(!command_Screen_Capture_PlaceView())
	{
		return COMMANDS_SUCC;
	}

	uint8 s_size[13];
	s_size[0] = (scWidth) & 0xFF;
	s_size[1] = (scWidth >> 8) & 0xFF;
	s_size[2] = (scWidth >> 16) & 0xFF;
	s_size[3] = (scWidth >> 24) & 0xFF;
	s_size[4] = (scHeight) & 0xFF;
	s_size[5] = (scHeight >> 8) & 0xFF;
	s_size[6] = (scHeight >> 16) & 0xFF;
	s_size[7] = (scHeight >> 24) & 0xFF;
	s_size[8] = (interval) & 0xFF;
	s_size[9] = (interval >> 8) & 0xFF;
	s_size[10] = (interval >> 16) & 0xFF;
	s_size[11] = (interval >> 24) & 0xFF;
	s_size[12] = 0;

	int rv = net_SendCmd(s_size, 13, COMMANDS_SCREEN_CAPTURE);

	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to send!\n");
		
		return NET_LOST_CONNECTION;
	}

	uint8* rmsg;

	rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES);

	if(rv == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to receive!\n");
		free(rmsg);

		return NET_LOST_CONNECTION;

	}else if(rv == NET_TIMED_OUT){
		LOG(LOG_ERR, "Time out!\n");
		free(rmsg);

		return NET_LOST_CONNECTION;
	}

	if(rmsg[0] != COMMANDS_APPROVE)
	{
		LOG(LOG_ERR, "Failed to take capture!\n");
		free(rmsg);

		return COMMANDS_SUCC;
	}

	scWidth = (rmsg[1] & 0xFF) | (rmsg[2] & 0xFF) << 8 | (rmsg[3] & 0xFF) << 16 | (rmsg[4] & 0xFF) << 24;
	scHeight = (rmsg[5] & 0xFF) | (rmsg[6] & 0xFF) << 8 | (rmsg[7] & 0xFF) << 16 | (rmsg[8] & 0xFF) << 24;
	uint32 size = scWidth * scHeight * 3;
	free(rmsg);

	LOG(LOG_INFO, "Stream width: %d\n", scWidth);
	LOG(LOG_INFO, "Stream height: %d\n", scHeight);
	LOG(LOG_INFO, "Frame size: %d %s\n", (size / 1024) < 1 ? size : size / 1024, (size / 1024) < 1 ? "B" : "KB");
	LOG(LOG_INFO, "Interval: %d\n", interval);
	LOG(LOG_INFO, "Hold \"ESC\" to stop capturing!\n");	

	int frames = 0;
	FILE *f;

	while(1)
	{
		if((GetKeyState(COMMAND_SCREEN_CAP_KEY_ESC) < 0))
		{
			if(net_SendCmd((uint8*)" ", 1, COMMAND_SCREEN_CAP_STOP) == NET_LOST_CONNECTION)
			{
				LOG(LOG_ERR, "Failed to stop capture!\n");
			}

			LOG(LOG_INFO, "Total frames: %d\n", frames);
			LOG(LOG_SUCC, "Stopped!\n");

			return COMMANDS_SUCC;
		}

		if(net_SendCmd((uint8*)" ", 1, COMMAND_SCREEN_CAP_CONTINUE) == NET_LOST_CONNECTION)
		{
			LOG(LOG_ERR, "Failed to continue capturing\n");

			return NET_LOST_CONNECTION;
		}

		Sleep(interval);

		f = fopen(COMMAND_SCREEN_CAP_TEMP_FILE, "wb");

		if(!f)
		{
			LOG(LOG_ERR, "Failed to create %s\n", COMMAND_SCREEN_CAP_TEMP_FILE);
			free(rmsg);

			return NET_LOST_CONNECTION;
		}

		while((rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES)) > 0)
		{
			if(rmsg[0] == COMMAND_SCREEN_CAP_END)
			{
				break;
			}else if(rmsg[0] == COMMAND_SCREEN_CAP_DATA){
				fwrite(rmsg + 1, 1, rv - 1, f);
			}

			free(rmsg);

			if(net_SendCmd((uint8*)" ", 1, 0) == NET_LOST_CONNECTION)
			{
				fclose(f);
				remove(COMMAND_SCREEN_CAP_TEMP_FILE);

				return NET_LOST_CONNECTION;
			}
		}

		fclose(f);
		free(rmsg);

		if(rv == NET_LOST_CONNECTION)
		{
			LOG(LOG_ERR, "Failed to receive!\n");
			remove(COMMAND_SCREEN_CAP_TEMP_FILE);

			return NET_LOST_CONNECTION;

		}else if(rv == NET_TIMED_OUT){
			LOG(LOG_ERR, "Time out!\n");		
			remove(COMMAND_SCREEN_CAP_TEMP_FILE);

			return NET_LOST_CONNECTION;
		}

		f = fopen(COMMAND_SCREEN_CAP_TEMP_FILE, "rb");

		if(!f)
		{
			LOG(LOG_ERR, "Failed to open temp file %s\n", COMMAND_SCREEN_CAP_TEMP_FILE);

			return COMMANDS_SUCC;
		}

		fseek(f, 0, SEEK_END);
		uint32 fsize = ftell(f);
		fseek(f, 0, SEEK_SET);

		uint8* imageData = malloc(fsize + sizeof(uint8));

		if(!fread(imageData, 1, fsize, f))
		{
			LOG(LOG_ERR, "Failed to read temp file %s\n", COMMAND_SCREEN_CAP_TEMP_FILE);
			free(imageData);

			return COMMANDS_SUCC;
		}

		fclose(f);
		remove(COMMAND_SCREEN_CAP_TEMP_FILE);

		bmp_Create("ScreenCap.bmp", imageData, scWidth, scHeight, 1);
		frames++;

		free(imageData);
	}

	return NET_LOST_CONNECTION;
}
