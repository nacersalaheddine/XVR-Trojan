#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "progressbar.h"
#include "logger.h"
#include "types.h"

int progressbar_ThreadAlive = 0;
int progressbar_Running = 0;
uint64 progressbar_Max = 0;
uint64 progressbar_Index = 0;

uint64 __stdcall progressbar_thread(void* args)
{
	int i;
	int printedChars = 0;
	int percent = 0;
	double prgs = 0;
	char buffer[100];
	char space = ' ';

	if(!log_Color)
	{
		space = '=';
	}

	char* dataType[2] = { "B", "KB" };
	int maxDataType = (progressbar_Max / 1024) < 1 ? 0 : 1;
	int prgsDataType = 0;

	uint64 maxConverted = 0;
	uint64 prgsIndexConverted = 0;
	uint8 color = 0x0F;

	progressbar_Running = 1;

	putchar('\n');

	while(progressbar_Running)
	{
		Sleep(PROGRESSBAR_TIMER);

		prgs = (double)((double)progressbar_Index / (double)progressbar_Max) * PROGRESSBAR_LEN;
		percent = (int)(((double)progressbar_Index / (double)progressbar_Max) * 100.0);
		maxConverted = (progressbar_Max / 1024) < 1 ? progressbar_Max : progressbar_Max / 1024;
		prgsIndexConverted = (progressbar_Index / 1024) < 1 ? progressbar_Index : progressbar_Index / 1024;
		prgsDataType = (progressbar_Index / 1024) < 1 ? 0 : 1;

		if(log_Color)
		{
			if(percent >= 90)
			{
				color = LOG_COLOR_PRGS_FULL;
			}else if(percent >= 50){
				color = LOG_COLOR_PRGS_HALF;
			}else{
				color = LOG_COLOR_PRGS_DEFAULT;
			}
		}

		if(printedChars != 0)
		{
			sprintf(buffer, "\r   ");

			for(i = 0; i != printedChars + 1; i++)
			{
				sprintf(buffer, " ");
			}

			sprintf(buffer, "\r");
			printf("%s", buffer);
			memset(buffer, 0, 100);
		}

		if((int)prgs >= PROGRESSBAR_LEN)
		{
			break;
		}

		printedChars = 1;
		printf("   |");

		if(log_Color)
		{
			LOG_plus_SetColor(log_colorPalette[color]);
		}

		for(i = 0; i != (int)prgs; i++)
		{
			putchar(space);
			printedChars++;
		}

		if(log_Color)
		{
			LOG_plus_SetColor(log_colorPalette[LOG_COLOR_TEXT]);
		}

		for(i = 0; i != (PROGRESSBAR_LEN + 1) - printedChars; i++)
		{
			putchar(' ');
		}

		printedChars += printf("| %03d%% %lu %s/%lu %s", percent, prgsIndexConverted, dataType[prgsDataType], maxConverted, dataType[maxDataType]);
		fflush(stdout);
	}

	if(progressbar_Running)
	{
		printf("   |");
		
		if(log_Color)
		{
			LOG_plus_SetColor(log_colorPalette[LOG_COLOR_PRGS_FULL]);
		}

		for(i = 0; i != PROGRESSBAR_LEN; i++)
		{
			putchar(space);
		}

		if(log_Color)
		{
			LOG_plus_SetColor(log_colorPalette[LOG_COLOR_TEXT]);
		}

		printf("| 100%% %lu %s/%lu %s\n", maxConverted, dataType[maxDataType], maxConverted, dataType[maxDataType]);
		fflush(stdout);
	}

	progressbar_Running = 0;
	progressbar_ThreadAlive = 0;
	
	return 0;
}

void progressbar_Start(void)
{
	progressbar_Index = 0;
	progressbar_ThreadAlive = 1;
	
	if(!CreateThread(NULL, 0, progressbar_thread, NULL, 0, NULL))
	{
		LOG(LOG_ERR, "Failed to create progressbar!\n");
	}
}

void progressbar_WaitToStart(void)
{
	while(!progressbar_Running){};
}

void progressbar_Stop(void)
{
	while(progressbar_Running){};
	putchar('\n');
}

void progressbar_CriticalStop(void)
{
	progressbar_Running = 0;

	while(progressbar_ThreadAlive){};
	puts("\n");
}