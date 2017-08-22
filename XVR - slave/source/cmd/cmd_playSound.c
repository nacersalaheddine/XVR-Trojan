#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <dirent.h>
#include "types.h"
#include "net/interface.h"
#include "net/commands.h"

typedef struct 
{
	char* sound[256];
}thread_args_ps;

int thread_playSound(void* args)
{
	thread_args_ps* taps = args;

	mciSendString(taps->sound, NULL, 0, NULL);
	free(taps);

	return 0;
}

int net_cmd_PlaySound(char* msg, int msgLen)
{
	thread_args_ps* taps = malloc(sizeof(taps));
	strcpy(taps->sound, msg);
	CreateThread(NULL, 0, thread_playSound, taps, 0,  NULL);

	return 1;
}