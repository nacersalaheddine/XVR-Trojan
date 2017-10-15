#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "logger.h"
#include "types.h"
#include "cmd/hdd/path.h"

int log_Color = 0;
int log_Time = 0;
char log_username[LOG_MAX_NAME_LEN + 1];
HANDLE hcon;
									//  0     1     2     3     4     5     6     7     8     9     10    11    12    13    14
uint16 log_colorPalette[LOG_COLOR_LIST_SIZE] = { 0x07, 0x0C, 0x0E, 0x0A, 0x0F, 0x09, 0x0D, 0xF0, 0xE0, 0xA0, 0x0B, 0x0D, 0x07, 0x0B, 0x0D};

void LOG_LoadConfing(void)
{
	FILE *f = fopen("colors.cfg", "r");

	if(!f)
	{
		LOG(LOG_ERR, "Didn't find \"colors.cfg\"\n");
		LOG(LOG_INFO, "Colors are set to default and saved!\n");

		f = fopen("colors.cfg", "w");
		fprintf(f, "TIME=%X\n", log_colorPalette[LOG_COLOR_TIME]);
		fprintf(f, "ERROR=%X\n", log_colorPalette[LOG_COLOR_ERROR]);
		fprintf(f, "INFO=%X\n", log_colorPalette[LOG_COLOR_INFO]);
		fprintf(f, "SUCC=%X\n", log_colorPalette[LOG_COLOR_SUCC]);
		fprintf(f, "TEXT=%X\n", log_colorPalette[LOG_COLOR_TEXT]);
		fprintf(f, "USERNAME=%X\n", log_colorPalette[LOG_COLOR_USERNAME]);
		fprintf(f, "USERNAME_SEP=%X\n", log_colorPalette[LOG_COLOR_USERNAME_SEP]);
		fprintf(f, "PRGS_DEFAULT=%X\n", log_colorPalette[LOG_COLOR_PRGS_DEFAULT]);
		fprintf(f, "PRGS_HALF=%X\n", log_colorPalette[LOG_COLOR_PRGS_HALF]);
		fprintf(f, "PRGS_FULL=%X\n", log_colorPalette[LOG_COLOR_PRGS_FULL]);
		fprintf(f, "LS_FILE=%X\n", log_colorPalette[LOG_COLOR_LS_FILE]);
		fprintf(f, "LS_FOLDER=%X\n", log_colorPalette[LOG_COLOR_LS_FOLDER]);
		fprintf(f, "LS_UNWN=%X\n", log_colorPalette[LOG_COLOR_LS_UNWN]);
		fprintf(f, "DISK_LIST_FIXED=%X\n", log_colorPalette[LOG_COLOR_DISK_LIST_FIXED]);
		fprintf(f, "DISK_LIST_RMOVABLE=%X\n", log_colorPalette[LOG_COLOR_DISK_LIST_REMOVABLE]);

		fclose(f);

		return;
	}

	char str[256];

	while(fgets(str, 256, f) != NULL)
	{
		if(str[0] == '#')
		{
			continue;
		}

		if(strncmp(str, "TIME=", 5) == 0)
		{
			log_colorPalette[LOG_COLOR_TIME] = strtol(str + 5, NULL, 16);
		}else if(strncmp(str, "ERROR=", 6) == 0){
			log_colorPalette[LOG_COLOR_ERROR] = strtol(str + 6, NULL, 16);
		}else if(strncmp(str, "INFO=", 5) == 0){
			log_colorPalette[LOG_COLOR_INFO] = strtol(str + 5, NULL, 16);
		}else if(strncmp(str, "SUCC=", 5) == 0){
			log_colorPalette[LOG_COLOR_SUCC] = strtol(str + 5, NULL, 16);
		}else if(strncmp(str, "TEXT=", 5) == 0){
			log_colorPalette[LOG_COLOR_TEXT] = strtol(str + 5, NULL, 16);
		}else if(strncmp(str, "USERNAME=", 9) == 0){
			log_colorPalette[LOG_COLOR_USERNAME] = strtol(str + 9, NULL, 16);
		}else if(strncmp(str, "USERNAME_SEP=", 13) == 0){
			log_colorPalette[LOG_COLOR_USERNAME_SEP] = strtol(str + 13, NULL, 16);
		}else if(strncmp(str, "PRGS_DEFAULT=", 13) == 0){
			log_colorPalette[LOG_COLOR_PRGS_DEFAULT] = strtol(str + 13, NULL, 16);
		}else if(strncmp(str, "PRGS_HALF=", 10) == 0){
			log_colorPalette[LOG_COLOR_PRGS_HALF] = strtol(str + 10, NULL, 16);
		}else if(strncmp(str, "PRGS_FULL=", 10) == 0){
			log_colorPalette[LOG_COLOR_PRGS_FULL] = strtol(str + 10, NULL, 16);
		}else if(strncmp(str, "LS_FILE=", 8) == 0){
			log_colorPalette[LOG_COLOR_LS_FILE] = strtol(str + 8, NULL, 16);
		}else if(strncmp(str, "LS_FOLDER=", 10) == 0){
			log_colorPalette[LOG_COLOR_LS_FOLDER] = strtol(str + 10, NULL, 16);
		}else if(strncmp(str, "LS_UNWN=", 8) == 0){
			log_colorPalette[LOG_COLOR_LS_UNWN] = strtol(str + 8, NULL, 16);
		}else if(strncmp(str, "DISK_LIST_FIXED=", 16) == 0){
			log_colorPalette[LOG_COLOR_DISK_LIST_FIXED] = strtol(str + 16, NULL, 16);
		}else if(strncmp(str, "DISK_LIST_RMOVABLE=", 19) == 0){
			log_colorPalette[LOG_COLOR_DISK_LIST_REMOVABLE] = strtol(str + 19, NULL, 16);
		}
	}

	LOG(LOG_SUCC, "Found \"colors.cfg\" and loaded!\n");

	fclose(f);
}

void LOG_plus_SetColor(uint16 color)
{
	SetConsoleTextAttribute(hcon, color);
}

void LOG_Init(void)
{
	uint64 log_username_len = LOG_MAX_NAME_LEN;
	memset(log_username, 0, LOG_MAX_NAME_LEN + 1);

	if(!GetUserName(log_username, &log_username_len))
	{
		strcpy(log_username, LOG_DEFAULT_USERNAME);
	}

	hdd_Path_Inited = 1;
	strcpy(hdd_Path, "C:\\");

	hcon = GetStdHandle(STD_OUTPUT_HANDLE);

	LOG_plus_SetColor(0x0F);
}

void LOG_USERNAME(void)
{
	if(log_Color)
	{
		SetConsoleTextAttribute(hcon, log_colorPalette[LOG_COLOR_USERNAME]);
		printf("\n%s", log_username);
		SetConsoleTextAttribute(hcon, log_colorPalette[LOG_COLOR_USERNAME_SEP]);
		printf(" ># ");
		SetConsoleTextAttribute(hcon, log_colorPalette[LOG_COLOR_TEXT]);		
	}else{
		printf("\n%s ># ", log_username);
	}
}

void LOG_SERVER_USERNAME(void)
{
	if(log_Color)
	{
		SetConsoleTextAttribute(hcon, log_colorPalette[LOG_COLOR_USERNAME]);
		printf("\nMASTER %s", log_username);

		if(hdd_Path_Inited)
		{
			SetConsoleTextAttribute(hcon, log_colorPalette[LOG_COLOR_TEXT]);
			printf(" %s", hdd_Path);
		}

		SetConsoleTextAttribute(hcon, log_colorPalette[LOG_COLOR_USERNAME_SEP]);
		printf(" >$ ");
		SetConsoleTextAttribute(hcon, log_colorPalette[LOG_COLOR_TEXT]);		
	}else{
		if(hdd_Path_Inited)
		{
			printf("\nMASTER %s %s >$ ", log_username, hdd_Path);
		}else{
			printf("\nMASTER %s >$ ", log_username);
		}
	}
}

void LOG(int logType, char* format, ...)
{
	va_list vl;
	va_start(vl, format);

	if(log_Time)
	{
		time_t rtime;
		struct tm *tm;
		time(&rtime);
		tm = localtime(&rtime);

		if(log_Color)
		{
			SetConsoleTextAttribute(hcon, log_colorPalette[LOG_COLOR_TIME]);
		}

		printf("[%02d\\%02d\\%04d][%02d:%02d:%02d]", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec);
	}

	if(log_Color)
	{
		uint16 _color = LOG_COLOR_TEXT;

		if(logType == LOG_SUCC)
		{
			_color = LOG_COLOR_SUCC;
		}else if(logType == LOG_ERR){
			_color = LOG_COLOR_ERROR;
		}else if(logType == LOG_INFO){
			_color = LOG_COLOR_INFO;
		}

		SetConsoleTextAttribute(hcon, log_colorPalette[_color]);
	}

	printf("[%c]", logType);

	if(log_Color)
	{
		SetConsoleTextAttribute(hcon, log_colorPalette[LOG_COLOR_TEXT]);
	}

	vprintf(format, vl);
	va_end(vl);
}