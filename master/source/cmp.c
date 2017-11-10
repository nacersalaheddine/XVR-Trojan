#include <stdlib.h>
#include <string.h>

int cmp_cmp(char* s1, char* s2)
{
	int len1 = strlen(s1);
	int len2 = strlen(s2);

	if(len1 != len2)
	{
		return 0;
	}

	int i;
	char ch1;
	char ch2;

	for(i = 0; i != len2; i++)
	{
		ch1 = s1[i];
		ch2 = s2[i];

		if(ch1 >= 'a' && ch1 <= 'z')
		{
			ch1 -= 0x20;
		}

		if(ch2 >= 'a' && ch2 <= 'z')
		{
			ch2 -= 0x20;
		}

		if(ch1 != ch2)
		{
			return 0;
		}
	}

	return 1;
}