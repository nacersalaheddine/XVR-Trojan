#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "logger.h"
#include "types.h"

int log_Color = 0;
int log_Time = 0;
char log_username[LOG_MAX_NAME_LEN + 1];
HANDLE hcon;
									//  0     1     2     3     4     5     6     7     8     9
uint16 log_colorPalette[LOG_COLOR_LIST_SIZE] = { 0x07, 0x0C, 0x0E, 0x0A, 0x0F, 0x09, 0x0D, 0xF0, 0xE0, 0xA0 };

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
		printf(" >$ ");
		SetConsoleTextAttribute(hcon, log_colorPalette[LOG_COLOR_TEXT]);		
	}else{
		printf("\n%s >$ ", log_username);
	}
}

void LOG_SERVER_USERNAME(void)
{
	if(log_Color)
	{
		SetConsoleTextAttribute(hcon, log_colorPalette[LOG_COLOR_USERNAME]);
		printf("\nMASTER %s", log_username);
		SetConsoleTextAttribute(hcon, log_colorPalette[LOG_COLOR_USERNAME_SEP]);
		printf(" >$ ");
		SetConsoleTextAttribute(hcon, log_colorPalette[LOG_COLOR_TEXT]);		
	}else{
		printf("\nMASTER %s >$ ", log_username);
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