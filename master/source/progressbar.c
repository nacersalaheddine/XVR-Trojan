#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "progressbar.h"
#include "logger.h"
#include "types.h"

int prgs_type = PROGRESSBAR_TYPE_SIMPLE;
int prgs_done = 0;
int prgs_lastPercent = -1;
int prgs_percent = 0;
double prgs_prgs = 0;
uint64 prgs_max = 0;
uint64 prgs_index = 0;

int prgs_op_Use = 1;

COORD prgs_curStatus[3]; //Start, data types, line Y, end
HANDLE prgs_hand;

/*looks ugly but its faster than"\b" & "\r" */

void progressbar_printBar(void)
{
	if(prgs_percent < 50)
	{
		LOG_SetColor(LOG_COLOR_PRGS_BAR_DEFAULT);
	}else if(prgs_percent < 80){
		LOG_SetColor(LOG_COLOR_PRGS_BAR_HALF);
	}else{
		LOG_SetColor(LOG_COLOR_PRGS_BAR_FULL);
	}

	int i;

	SetConsoleCursorPosition(prgs_hand, prgs_curStatus[0]);

	for(i = 0; i != (int)prgs_prgs; i++)
	{
		putchar(log_PrgsChar);
	}
}

void progressbar_print(void)
{
	if(!prgs_op_Use)
	{
		return;
	}
	
	if(prgs_done)
	{
		return;
	}

	progressbar_printBar();
	
	SetConsoleCursorPosition(prgs_hand, prgs_curStatus[1]);

	LOG_SetColor(LOG_COLOR_TEXT);
	LOG_TablePrint(5, "%d%% ", prgs_percent);

	if(prgs_type == PROGRESSBAR_TYPE_FILE)
	{
		char tp = 0;
		double cval = (double)prgs_index;
		double cmax = (double)prgs_max;

		if(prgs_max / (1024 * 1024 * 1024))
		{
			tp = 'G';
			cval = (double)prgs_index / (1024.0 * 1024.0 * 1024.0);
			cmax = (double)prgs_max / (1024.0 * 1024.0 * 1024.0);
		}else if(prgs_max / (1024 * 1024)){
			tp = 'M';
			cval = (double)prgs_index / (1024.0 * 1024.0);
			cmax = (double)prgs_max / (1024.0 * 1024.0);
		}else if(prgs_max / 1024){
			tp = 'K';
			cval = (double)prgs_index / 1024.0;
			cmax = (double)prgs_max / 1024.0;
		}
		
		if(tp)
		{
			LOG_TablePrint(17, "%.1lf/%.1lf %cB", cval, cmax, tp);
		}else{
			LOG_TablePrint(17, "%.1lf/%.1lf B", cval, cmax);
		}
	}	
}

void progressbar_Update(uint64 val)
{
	if(!prgs_op_Use)
	{
		return;
	}

	if(prgs_done)
	{
		return;
	}

	prgs_index += val;

	if(prgs_index >= prgs_max)
	{
		progressbar_End();

		return;
	}

	prgs_prgs = (double)((double)prgs_index / (double)prgs_max) * PROGRESSBAR_LEN;
	prgs_percent = (int)(((double)prgs_index / (double)prgs_max) * 100.0);

	if(prgs_percent < prgs_lastPercent)
	{
		prgs_percent = 0;
	}

	progressbar_print();
	prgs_lastPercent = prgs_percent;
	prgs_percent = 0;
}

void progressbar_End(void)
{
	if(!prgs_op_Use)
	{
		return;
	}

	if(prgs_done)
	{
		return;
	}

	prgs_percent = 100;
	prgs_prgs = PROGRESSBAR_LEN;
	prgs_index = prgs_max;
	progressbar_print();
	prgs_done = 1;

	SetConsoleCursorPosition(prgs_hand, prgs_curStatus[2]);

	LOG_NEWLINE();
}

void progressbar_Create(uint64 maxVal, int type)
{
	if(!prgs_op_Use)
	{
		return;
	}

	LOG_CursorStatus(0);

	prgs_done = 0;
	prgs_lastPercent = -1;
	prgs_percent = 0;
	prgs_prgs = 0;
	prgs_max = maxVal;
	prgs_index = 0;
	prgs_type = type;

	CONSOLE_SCREEN_BUFFER_INFO coninfo;

	if(!prgs_hand)
	{
		prgs_hand = GetStdHandle(STD_OUTPUT_HANDLE);
	}
	
	GetConsoleScreenBufferInfo(prgs_hand, &coninfo);

	prgs_curStatus[0].X = 3;
	prgs_curStatus[0].Y = coninfo.dwCursorPosition.Y;
	prgs_curStatus[1].X = 3 + 3 + (int)PROGRESSBAR_LEN;
	prgs_curStatus[1].Y = coninfo.dwCursorPosition.Y;
	prgs_curStatus[2].X = 3 + 3 + (int)PROGRESSBAR_LEN + 20;
	prgs_curStatus[2].Y = coninfo.dwCursorPosition.Y;
	
	int i;
	printf("  [");

	for(i = 0; i != PROGRESSBAR_LEN; i++)
	{
		putchar(' ');
	}

	printf("] ");
}