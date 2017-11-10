#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "logger.h"
#include "input.h"
#include "types.h"

#define INPUT_GETS_LIST_MAX 30
#define INPUT_GETS_MAX 1024
#define INPUT_TAB_SIZE 4

#define INPUT_CH_BREAK 0x3 //ctrl + c
#define INPUT_CH_NEWLINE 0xD //enter
#define INPUT_CH_BACKSPACE 0x8
#define INPUT_CH_TAB 0x9
#define INPUT_CH_ESC 0x1B
#define INPUT_CH_ARR 0xE0
#define INPUT_CH_ARR_UP 0x48
#define INPUT_CH_ARR_DOWN 0x50
#define INPUT_CH_ARR_LEFT 0x4B
#define INPUT_CH_ARR_RIGHT 0x4D

int input_gets_list_Index = 0;
char input_gets_list[INPUT_GETS_LIST_MAX][INPUT_GETS_MAX];

void input_Gets_Init(void)
{
	int i;

	for(i = 0; i != INPUT_GETS_LIST_MAX; i++)
	{
		memset(input_gets_list[i], 0, INPUT_GETS_MAX);
	}
}

void input_Gets_store(char* str)
{
	if(input_gets_list_Index < INPUT_GETS_LIST_MAX)
	{
		memset(input_gets_list[input_gets_list_Index], 0, INPUT_GETS_MAX);
		strcpy(input_gets_list[input_gets_list_Index], str);
		input_gets_list_Index++;

		return;
	}

	int i;
	memset(input_gets_list[input_gets_list_Index], 0, INPUT_GETS_MAX);
	strcpy(input_gets_list[input_gets_list_Index], str);

	for(i = 0; i < INPUT_GETS_LIST_MAX; i++)
	{
		memset(input_gets_list[i], 0, INPUT_GETS_MAX);
		strcpy(input_gets_list[i], input_gets_list[i + 1]);
	}
}

void input_Gets_spec(OUT_STRP _text, OUT_INT i, OUT_INT li)
{
	char* text = *_text;
	uint8 ch = getch() & 0xFF;

	if(ch == INPUT_CH_ARR_LEFT)
	{
		if(*i - 1 < 0)
		{
			return;
		}

		putchar('\b');
		*i -= 1;

		return;
	}else if(ch == INPUT_CH_ARR_RIGHT){
		if(*i + 1 >= INPUT_GETS_MAX)
		{
			return;
		}

		if(*i >= strlen(text))
		{
			return;
		}

		putchar(text[*i]);
		*i += 1;

		return;
	}else if(ch == INPUT_CH_ARR_DOWN){
		if(*li - 1 < 0)
		{
			return;
		}

		LOG_CursorStatus(0);

		int y;
		int tlen = strlen(text);

		for(y = 0; y != tlen; y++)
		{
			putchar('\b');
			putchar(' ');
			putchar('\b');
		}

		*li -= 1;
		memset(text, 0, tlen);
		strcpy(text, input_gets_list[*li]);
		printf("%s", text);
		*i = strlen(text);

		LOG_CursorStatus(1);
	}else if(ch == INPUT_CH_ARR_UP){
		if(*li + 1 >= input_gets_list_Index)
		{
			return;
		}

		LOG_CursorStatus(0);
		
		int y;
		int tlen = strlen(text);

		for(y = 0; y != tlen; y++)
		{
			putchar('\b');
			putchar(' ');
			putchar('\b');
		}

		*li += 1;
		memset(text, 0, tlen);
		strcpy(text, input_gets_list[*li]);
		printf("%s", text);
		*i = strlen(text);

		LOG_CursorStatus(1);
	}
}

char* input_Gets(void)
{
	int i = 0;
	int y = 0;
	int listSec = input_gets_list_Index;
	int arrUse = 0;
	uint8 ch = 0;
	char* text = malloc(INPUT_GETS_MAX);
	memset(text, 0, INPUT_GETS_MAX);

	fflush(stdin);
	LOG_CursorStatus(1);

	while(i >= 0 && i < INPUT_GETS_MAX)
	{
		if(arrUse)
		{
			input_Gets_spec(&text, &i, &listSec);
			arrUse = 0;

			continue;
		}

		ch = getch() & 0xFF;

		if(ch == INPUT_CH_BREAK)
		{
			LOG_NEWLINE();
			free(text);

			exit(0);
		}else if(ch == INPUT_CH_ARR){
			arrUse++;

			continue;
		}else if(ch == INPUT_CH_ESC){
			memset(text, 0, INPUT_GETS_MAX);

			while(i > 0)
			{
				putchar('\b');
				putchar(' ');
				putchar('\b');
				i--;
			}

			continue;
		}

		if(ch == INPUT_CH_NEWLINE)
		{
			LOG_NEWLINE();

			if(i > 0)
			{
				input_Gets_store(text);
			}

			break;
		}else if(ch == INPUT_CH_TAB){
			y = i + INPUT_TAB_SIZE;

			if(y >= INPUT_GETS_MAX)
			{
				y = INPUT_GETS_MAX;
			}

			while(i < y)
			{
				putchar(' ');
				text[i++] = ' ';
			}

			continue;
		}else if(ch == INPUT_CH_BACKSPACE){
			if(i - 1 < 0)
			{
				continue;
			}

			if(i < strlen(text))
			{
				int clen = strlen(text + i);
				
				LOG_CursorStatus(0);
				char* temp = malloc(clen + sizeof(char));
				memset(temp, 0, clen + sizeof(char));
				strcpy(temp, text + i);
				strcpy(text + i - 1, temp);
				free(temp);

				putchar('\b');

				for(y = 0; y <= clen + 1; y++)
				{
					putchar(' ');
				}

				for(y = 0; y <= clen + 1; y++)
				{
					putchar('\b');
				}

				printf("%s", text + i - 1);

				for(y = 0; y < clen; y++)
				{
					putchar('\b');
				}

				i--;
	
				LOG_CursorStatus(1);
			}else{
				i--;
				text[i] = 0x0;
				putchar('\b');
				putchar(' ');
				putchar('\b');
			}

			continue;
		}

		if(i < strlen(text))
		{
			int clen = strlen(text + i);

			if(i + clen + i >= INPUT_GETS_MAX)
			{
				continue;
			}

			LOG_CursorStatus(0);
			char* temp = malloc(clen + sizeof(char));
			memset(temp, 0, clen + sizeof(char));
			strcpy(temp, text + i);
			strcpy(text + i + 1, temp);
			free(temp);
			text[i++] = ch;

			printf("%c%s", ch, text + i);

			for(y = 0; y < clen; y++)
			{
				putchar('\b');
			}

			LOG_CursorStatus(1);
			
			continue;
		}
		
		putchar(ch);
		text[i++] = ch;
	}

	LOG_CursorStatus(0);

	return text;
}

int input_Get(int type, OUT_STRP str)
{
	if(type == INPUT_TYPE_USER)
	{
		LOG_USER();
	}else if(type == INPUT_TYPE_MASTER){
		LOG_USER_MASTER();
	}

	char* rmsg = input_Gets();

	if(strlen(rmsg) > 0)
	{
		*str = rmsg;

		return 1;
	}

	free(rmsg);

	return 0;
}

int input_IsKeyPressed(uint8 key)
{
	if(kbhit())
	{
		if(getch() == key)
		{
			return 1;
		}
	}

	return 0;
}