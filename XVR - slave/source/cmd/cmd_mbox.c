#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <dirent.h>
#include "types.h"
#include "net/interface.h"
#include "net/commands.h"

#define MBOX_MB_PLAIN 0x0
#define MBOX_MB_ERROR 0x1
#define MBOX_MB_QUESTION 0x2
#define MBOX_MB_WARNING 0x3
#define MBOX_MB_INFO 0x4

typedef struct 
{
	int type;
	char* msg;
	char* title;
}thread_args_mbox;

int thread_mbox(void* args)
{
	thread_args_mbox* taps = args;
	MessageBox(NULL, taps->msg, taps->title, taps->type);
	free(taps->msg);
	free(taps->title);
	free(taps);

	return 0;
}

int net_cmd_mbox(char* msg, int msgLen)
{
	int type = *msg++;
	int spliter = *msg++;

	if(type == MBOX_MB_ERROR)
	{
		type = MB_ICONERROR | MB_OK;
	}else if(type == MBOX_MB_QUESTION){
		type = MB_ICONQUESTION | MB_OK;
	}else if(type == MBOX_MB_WARNING){
		type = MB_ICONWARNING | MB_OK;
	}else if(type == MBOX_MB_INFO){
		type = MB_ICONINFORMATION | MB_OK;
	}else{
		type = MB_OK;
	}

	thread_args_mbox* taps = malloc(sizeof(taps) + 3 + (spliter + 1 + (strlen(msg) - spliter)));
	taps->type = type;
	taps->title = malloc(spliter + 1);
	taps->msg = malloc(strlen(msg) - spliter);
	memset(taps->title, 0, spliter + 1);
	memset(taps->msg, 0, strlen(msg) - spliter);
	strncpy(taps->title, msg, spliter);
	strncpy(taps->msg, msg + spliter, strlen(msg) - spliter);
	msg -= 2;

	CreateThread(NULL, 0, thread_mbox, taps, 0,  NULL);
	
	return 1;
}