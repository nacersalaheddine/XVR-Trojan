#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "input.h"

char* input_Get(int type)
{
	fflush(stdin);

	char* rmsg = malloc(INPUT_MAX_SIZE);
	memset(rmsg, 0, INPUT_MAX_SIZE);

	if(type == INPUT_MAIN)
	{
		LOG_USERNAME();
	}else if(type == INPUT_MASTER){
		LOG_SERVER_USERNAME();
	}

	fgets(rmsg, INPUT_MAX_SIZE - 1, stdin);

	return rmsg;
}