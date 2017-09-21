#include <stdlib.h>
#include <string.h>
#include "random.h"

char* random_Ascii(int len)
{
	int i = 0;
	char ch;
	char* rstr = malloc(len + 1);
	memset(rstr, 0, len + 1);

	while(i != len)
	{
		ch = 'a' + rand() % ('a' - 'z');

		rstr[i++] = ch;
	}

	return rstr;
}