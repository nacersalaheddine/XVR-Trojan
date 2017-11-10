#include <stdio.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "types.h"
#include "logger.h"
#include "net/interface.h"
#include "server/hdd/hdd.h"
#include "geoIP.h"

#define LOG_CONFIG_FILE "console.cfg"

#define LOG_OUT_LEN 124
#define LOG_DEF_OUT_SUCC "[+]"
#define LOG_DEF_OUT_INFO "[~]"
#define LOG_DEF_OUT_WAR "[!]"
#define LOG_DEF_OUT_ERR "[-]"
#define LOG_DEF_OUT_CRITICAL "[CRITICAL]"
#define LOG_DEF_OUT_TABLE " - "

char log_out_succ[LOG_OUT_LEN];
char log_out_info[LOG_OUT_LEN];
char log_out_war[LOG_OUT_LEN];
char log_out_err[LOG_OUT_LEN];
char log_out_critical[LOG_OUT_LEN];
char log_out_table[LOG_OUT_LEN];
char log_Username[LOG_USERNAME_MAX];
uint8 log_PrgsChar;

int log_op_Color = 0;
int log_op_Time = 0;
char log_orgName[LOG_OUT_LEN];
HANDLE log_con;

uint8 log_colorList[LOG_COLOR_COUNT];

void LOG_Init(void)
{
	memset(log_colorList, 0xF, LOG_COLOR_COUNT);
	log_colorList[LOG_COLOR_TIME] = 0x07;
	log_colorList[LOG_COLOR_SUCC] = 0x0A;
	log_colorList[LOG_COLOR_INFO] = 0x0B;
	log_colorList[LOG_COLOR_WAR] = 0x0E;
	log_colorList[LOG_COLOR_ERR] = 0x0C;
	log_colorList[LOG_COLOR_CRITICAL] = 0x0C;
	log_colorList[LOG_COLOR_USERNAME] = 0x04;
	log_colorList[LOG_COLOR_USERNAME_SEP] = 0x0D;
	log_colorList[LOG_COLOR_HDD_PATH] = 0x0B;
	log_colorList[LOG_COLOR_DISKLIST_REMOVABLE] = 0x0D;
	log_colorList[LOG_COLOR_DISKLIST_FIXED] = 0x0B;
	log_colorList[LOG_COLOR_DISKLIST_UNKNOWN] = 0x0C;
	log_colorList[LOG_COLOR_LS_FILE] = 0x0B;
	log_colorList[LOG_COLOR_LS_FOLDER] = 0x0D;
	log_colorList[LOG_COLOR_PRGS_BAR_DEFAULT] = 0x0F;
	log_colorList[LOG_COLOR_PRGS_BAR_HALF] = 0x0E;
	log_colorList[LOG_COLOR_PRGS_BAR_FULL] = 0x0A;

	memset(log_out_succ, 0, LOG_OUT_LEN);
	memset(log_out_info, 0, LOG_OUT_LEN);
	memset(log_out_war, 0, LOG_OUT_LEN);
	memset(log_out_err, 0, LOG_OUT_LEN);
	memset(log_out_critical, 0, LOG_OUT_LEN);
	memset(log_out_table, 0, LOG_OUT_LEN);

	strcpy(log_out_succ, LOG_DEF_OUT_SUCC);
	strcpy(log_out_info, LOG_DEF_OUT_INFO);
	strcpy(log_out_war, LOG_DEF_OUT_WAR);
	strcpy(log_out_err, LOG_DEF_OUT_ERR);
	strcpy(log_out_critical, LOG_DEF_OUT_CRITICAL);
	strcpy(log_out_table, LOG_DEF_OUT_TABLE);
	log_PrgsChar = 0xFE;

	log_con = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO coninfo;
	
	if(GetConsoleScreenBufferInfo(log_con, &coninfo))
	{
		log_colorList[LOG_COLOR_ORIGINAL] = coninfo.wAttributes;
	}

	memset(log_orgName, 0, LOG_OUT_LEN);

	if(!GetConsoleTitleA(log_orgName, LOG_OUT_LEN - 1))
	{
		strcpy(log_orgName, "Cmd");
	}

	ulong userlen = LOG_USERNAME_MAX - 1;
	memset(log_Username, 0, LOG_USERNAME_MAX);

	if(!GetUserName(log_Username, &userlen))
	{
		strcpy(log_Username, LOG_DEF_USERNAME);
	}

	LOG_TitleDefault();
}

void LOG_LoadConfig_OutError(int rv)
{
	if(rv)
	{
		LOG(LOG_SUCC, "Loaded \"%s\"\n", LOG_CONFIG_FILE);

		return;
	}

	LOG(LOG_WAR, "Didn't find \"%s\"\n", LOG_CONFIG_FILE);
	LOG(LOG_TABLE, "Default colors and settings set in \"%s\"\n", LOG_CONFIG_FILE);
}

int LOG_loadValue(char* s)
{
	if(strlen(s) > 2)
	{
		if(s[0] == '0')
		{
			if(s[1] == 'x' || s[1] == 'X')
			{
				return strtol(s + 2, NULL, 16);
			}
		}
	}

	return atoi(s);
}

int LOG_LoadConfig(void)
{
	int rv = 1;
	FILE *f = fopen(LOG_CONFIG_FILE, "r");

	if(!f)
	{
		f = fopen(LOG_CONFIG_FILE, "w");
		fprintf(f, "MSG_SUCC=%s\n", LOG_DEF_OUT_SUCC);
		fprintf(f, "MSG_INFO=%s\n", LOG_DEF_OUT_INFO);
		fprintf(f, "MSG_WAR=%s\n", LOG_DEF_OUT_WAR);
		fprintf(f, "MSG_ERR=%s\n", LOG_DEF_OUT_ERR);
		fprintf(f, "MSG_CRITICAL=%s\n", LOG_DEF_OUT_CRITICAL);
		fprintf(f, "MSG_TABLE=%s\n", LOG_DEF_OUT_TABLE);
		fprintf(f, "HDD_VISABLE_SIZE=%d\n", hdd_VisableSize);
		fprintf(f, "PRGS_CHAR=0x%X\n", log_PrgsChar);

		fprintf(f, "#Colors\n");
		fprintf(f, "TEXT=0x%X\n", log_colorList[LOG_COLOR_TEXT]);
		fprintf(f, "TIME=0x%X\n", log_colorList[LOG_COLOR_TIME]);
		fprintf(f, "SUCC=0x%X\n", log_colorList[LOG_COLOR_SUCC]);
		fprintf(f, "INFO=0x%X\n", log_colorList[LOG_COLOR_INFO]);
		fprintf(f, "WAR=0x%X\n", log_colorList[LOG_COLOR_WAR]);
		fprintf(f, "ERR=0x%X\n", log_colorList[LOG_COLOR_ERR]);
		fprintf(f, "CRITICAL=0x%X\n", log_colorList[LOG_COLOR_CRITICAL]);
		fprintf(f, "USERNAME=0x%X\n", log_colorList[LOG_COLOR_USERNAME]);
		fprintf(f, "USERNAME_SEP=0x%X\n", log_colorList[LOG_COLOR_USERNAME_SEP]);
		fprintf(f, "HDD_PATH=0x%X\n", log_colorList[LOG_COLOR_HDD_PATH]);
		fprintf(f, "DISKLIST_NO_ROOT_DIR=0x%X\n", log_colorList[LOG_COLOR_DISKLIST_NO_ROOT_DIR]);
		fprintf(f, "DISKLIST_REMOVABLE=0x%X\n", log_colorList[LOG_COLOR_DISKLIST_REMOVABLE]);
		fprintf(f, "DISKLIST_FIXED=0x%X\n", log_colorList[LOG_COLOR_DISKLIST_FIXED]);
		fprintf(f, "DISKLIST_REMOTE=0x%X\n", log_colorList[LOG_COLOR_DISKLIST_REMOTE]);
		fprintf(f, "DISKLIST_CDROM=0x%X\n", log_colorList[LOG_COLOR_DISKLIST_CDROM]);
		fprintf(f, "DISKLIST_RAMDISK=0x%X\n", log_colorList[LOG_COLOR_DISKLIST_RAMDISK]);
		fprintf(f, "DISKLIST_UNKNOWN=0x%X\n", log_colorList[LOG_COLOR_DISKLIST_UNKNOWN]);
		fprintf(f, "LS_FILE=0x%X\n", log_colorList[LOG_COLOR_LS_FILE]);
		fprintf(f, "LS_FOLDER=0x%X\n", log_colorList[LOG_COLOR_LS_FOLDER]);
		fprintf(f, "LS_UNKNOWN=0x%X\n", log_colorList[LOG_COLOR_LS_UNKNOWN]);
		fprintf(f, "PRGS_DEFAULT=0x%X\n", log_colorList[LOG_COLOR_PRGS_BAR_DEFAULT]);
		fprintf(f, "PRGS_HALF=0x%X\n", log_colorList[LOG_COLOR_PRGS_BAR_HALF]);
		fprintf(f, "PRGS_FULL=0x%X\n", log_colorList[LOG_COLOR_PRGS_BAR_FULL]);
		

		fclose(f);
		rv--;
	}else{
		char str[256];
		
		while(fgets(str, 256, f) != NULL)
		{
			if(str[0] == '#') //comma
			{
				continue;
			}

			str[strlen(str) - 1] = '\0';

			if(strncmp(str, "MSG_SUCC=", 9) == 0)
			{
				strcpy(log_out_succ, str + 9);
			}else if(strncmp(str, "MSG_INFO=", 9) == 0){
				strcpy(log_out_info, str + 9);
			}else if(strncmp(str, "MSG_WAR=", 8) == 0){
				strcpy(log_out_war, str + 8);
			}else if(strncmp(str, "MSG_ERR=", 8) == 0){
				strcpy(log_out_err, str + 8);
			}else if(strncmp(str, "MSG_CRITICAL=", 13) == 0){
				strcpy(log_out_critical, str + 13);
			}else if(strncmp(str, "MSG_TABLE=", 10) == 0){
				strcpy(log_out_table, str + 10);
			}else if(strncmp(str, "HDD_VISABLE_SIZE=", 17) == 0){
				hdd_VisableSize = LOG_loadValue(str + 17); //atoi(str + 17);
			}else if(strncmp(str, "PRGS_CHAR=", 10) == 0){
				log_PrgsChar = LOG_loadValue(str + 10) & 0xFF;
			//Colors
			}else if(strncmp(str, "TEXT=", 5) == 0){
				log_colorList[LOG_COLOR_TEXT] =  LOG_loadValue(str + 5);
			}else if(strncmp(str, "TIME=", 5) == 0){
				log_colorList[LOG_COLOR_TIME] =  LOG_loadValue(str + 5);
			}else if(strncmp(str, "SUCC=", 5) == 0){
				log_colorList[LOG_COLOR_SUCC] =  LOG_loadValue(str + 5);
			}else if(strncmp(str, "INFO=", 5) == 0){
				log_colorList[LOG_COLOR_INFO] =  LOG_loadValue(str + 5);
			}else if(strncmp(str, "WAR=", 4) == 0){
				log_colorList[LOG_COLOR_WAR] =  LOG_loadValue(str + 4);
			}else if(strncmp(str, "ERR=", 4) == 0){
				log_colorList[LOG_COLOR_ERR] =  LOG_loadValue(str + 4);
			}else if(strncmp(str, "CRITICAL=", 9) == 0){
				log_colorList[LOG_COLOR_CRITICAL] =  LOG_loadValue(str + 9);
			}else if(strncmp(str, "USERNAME=", 9) == 0){
				log_colorList[LOG_COLOR_USERNAME] =  LOG_loadValue(str + 9);
			}else if(strncmp(str, "USERNAME_SEP=", 13) == 0){
				log_colorList[LOG_COLOR_USERNAME_SEP] =  LOG_loadValue(str + 13);
			}else if(strncmp(str, "HDD_PATH=", 9) == 0){
				log_colorList[LOG_COLOR_HDD_PATH] =  LOG_loadValue(str + 9);
			}else if(strncmp(str, "DISKLIST_NO_ROOT_DIR=", 21) == 0){
				log_colorList[LOG_COLOR_DISKLIST_NO_ROOT_DIR] =  LOG_loadValue(str + 21);
			}else if(strncmp(str, "DISKLIST_REMOVABLE=", 19) == 0){
				log_colorList[LOG_COLOR_DISKLIST_REMOVABLE] =  LOG_loadValue(str + 19);
			}else if(strncmp(str, "DISKLIST_FIXED=", 15) == 0){
				log_colorList[LOG_COLOR_DISKLIST_FIXED] =  LOG_loadValue(str + 15);
			}else if(strncmp(str, "DISKLIST_REMOTE=", 16) == 0){
				log_colorList[LOG_COLOR_DISKLIST_REMOTE] =  LOG_loadValue(str + 16);
			}else if(strncmp(str, "DISKLIST_CDROM=", 15) == 0){
				log_colorList[LOG_COLOR_DISKLIST_CDROM] =  LOG_loadValue(str + 15);
			}else if(strncmp(str, "DISKLIST_RAMDISK=", 17) == 0){
				log_colorList[LOG_COLOR_DISKLIST_RAMDISK] =  LOG_loadValue(str + 17);
			}else if(strncmp(str, "DISKLIST_UNKNOWN=", 17) == 0){
				log_colorList[LOG_COLOR_DISKLIST_UNKNOWN] =  LOG_loadValue(str + 17);
			}else if(strncmp(str, "LS_FILE=", 8) == 0){
				log_colorList[LOG_COLOR_LS_FILE] =  LOG_loadValue(str + 8);
			}else if(strncmp(str, "LS_FOLDER=", 10) == 0){
				log_colorList[LOG_COLOR_LS_FOLDER] =  LOG_loadValue(str + 10);
			}else if(strncmp(str, "LS_UNKNOWN=", 11) == 0){
				log_colorList[LOG_COLOR_LS_UNKNOWN] =  LOG_loadValue(str + 11);
			}else if(strncmp(str, "PRGS_DEFAULT=", 13) == 0){
				log_colorList[LOG_COLOR_PRGS_BAR_DEFAULT] =  LOG_loadValue(str + 13);
			}else if(strncmp(str, "PRGS_HALF=", 10) == 0){
				log_colorList[LOG_COLOR_PRGS_BAR_HALF] =  LOG_loadValue(str + 10);
			}else if(strncmp(str, "PRGS_FULL=", 10) == 0){
				log_colorList[LOG_COLOR_PRGS_BAR_FULL] =  LOG_loadValue(str + 10);
			}
		}
	
		fclose(f);
	}

	LOG_SetColor(LOG_COLOR_TEXT);
	LOG_CursorStatus(0);

	system("CLS");

	return rv;
}

void LOG_SetColor(int idx)
{
	if(!log_op_Color)
	{
		return;
	}

	if(idx < 0 || idx > LOG_COLOR_COUNT - 1)
	{
		return;
	}

	SetConsoleTextAttribute(log_con, log_colorList[idx]);
}

void LOG_CursorStatus(int of)
{
	CONSOLE_CURSOR_INFO cinfo;
	cinfo.dwSize = 1;
	cinfo.bVisible = of;
	
	SetConsoleCursorInfo(log_con, &cinfo);
}

void LOG_PrintTime(void)
{
	if(!log_op_Time)
	{
		return;
	}

	time_t rtime;
	struct tm *tm;
	time(&rtime);
	tm = localtime(&rtime);

	LOG_SetColor(LOG_COLOR_TIME);
	printf("[%02d\\%02d\\%04d][%02d:%02d:%02d]", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec);
}

void LOG(int logType, char* format, ...)
{
	va_list vl;
	va_start(vl, format);

	LOG_PrintTime();

	switch(logType)
	{
		case LOG_SUCC:
			LOG_SetColor(LOG_COLOR_SUCC);
			printf(log_out_succ);
			break;
		case LOG_INFO:
			LOG_SetColor(LOG_COLOR_INFO);
			printf(log_out_info);
			break;
		case LOG_WAR:
			LOG_SetColor(LOG_COLOR_WAR);
			printf(log_out_war);
			break;
		case LOG_ERR:
			LOG_SetColor(LOG_COLOR_ERR);
			printf(log_out_err);
			break;
		case LOG_CRITICAL:
			LOG_SetColor(LOG_COLOR_CRITICAL);
			printf(log_out_critical);
			break;
		case LOG_TABLE:
			LOG_SetColor(LOG_COLOR_TEXT);
			printf(log_out_table);
			break;
		default:
			break;
	}

	if(logType != LOG_CRITICAL)
	{
		LOG_SetColor(LOG_COLOR_TEXT);
	}

	vprintf(format, vl);

	if(logType == LOG_CRITICAL)
	{
		LOG_SetColor(LOG_COLOR_TEXT);
	}

	va_end(vl);
}

void LOG_USER(void)
{
	putchar(' ');
	LOG_SetColor(LOG_COLOR_USERNAME);
	printf("%s", log_Username);
	LOG_SetColor(LOG_COLOR_USERNAME_SEP);
	printf(":# ");
	LOG_SetColor(LOG_COLOR_TEXT);
}

void LOG_USER_MASTER(void)
{
	putchar(' ');
	LOG_SetColor(LOG_COLOR_HDD_PATH);

	int wg;
	char* path = hdd_GetVisable(&wg);

	if(wg)
	{
		printf("~%s", path);
	}else{
		printf("%s", path);
	}

	putchar(' ');
	LOG_SetColor(LOG_COLOR_USERNAME);
	printf("%s", log_Username);
	LOG_SetColor(LOG_COLOR_USERNAME_SEP);
	printf(":$ ");
	LOG_SetColor(LOG_COLOR_TEXT);
}

void LOG_TitleMaster(void)
{	
	char buff[50];
	sprintf(buff, "Xvr Ping: %d\n", net_Ping);
	SetConsoleTitle(buff);
}

void LOG_TitleDefault(void)
{
	SetConsoleTitle("Xvr");
}

void LOG_ResetTitle(void)
{
	SetConsoleTitle(log_orgName);
}

void LOG_TablePrint(int max, char* format, ...)
{
	va_list vl;
	va_start(vl, format);

	int i;
	int len = vprintf(format, vl);

	for(i = len; i < max; i++)
	{
		putchar(' ');
	}

	va_end(vl);
}