#include <stdlib.h>
#include "types.h"
#include "SCL.h"

uint64 SCL_Seed = SCL_SEED;
uint64 SCL_lastRand = 1;

void SCL_Encrypt(OUT_USTRP msg, int len)
{
	srand(SCL_Seed);

	int i;
	uint8* key = SCL_KEY;
	uint8* salt = SCL_SALT;
	uint8* nmsg = *msg;

	for(i = 0; i != len; i++)
	{
		nmsg[i] = SCL_KEY_SALT_ENCRYPT(nmsg[i], key[rand() % SCL_KEY_LEN], salt[rand() % SCL_KEY_LEN]);
	}
}

void SCL_Decrypt(OUT_USTRP msg, int len)
{
	srand(SCL_Seed);
	
	int i;
	uint8* key = SCL_KEY;
	uint8* salt = SCL_SALT;
	uint8* nmsg = *msg;
	uint8 keyVal = 0;
	uint8 saltVal = 0;
	int randValuesI = 0;
	int randValues[len * 2];

	for(i = 0; i != len * 2; i++)
	{
		randValues[i] = rand() % SCL_KEY_LEN;
	}

	for(i = 0; i != len; i++)
	{
		keyVal = key[randValues[randValuesI++]];
		saltVal = salt[randValues[randValuesI++]];

		nmsg[i] = SCL_KEY_SALT_DECRYPT(nmsg[i], keyVal, saltVal);
	}
}

int SCL_rand(void)
{
	uint64 new_rand = SCL_lastRand * SCL_UP_SEED + 0x41F523fA;
	SCL_lastRand += 2;

	if(SCL_lastRand >= UINT64_MAX) //0 * 2315214 = 0 right ?
	{
		SCL_lastRand = 1;
	}

	return new_rand % SCL_UP_SEED_MAX_VALUE;
}

void SCL_ResetSeed(void)
{
	SCL_Seed = SCL_SEED;
	SCL_lastRand = 1;
}

//called after the end on every master command
void SCL_SeedUp(void)
{
	SCL_Seed += SCL_rand();

	if(SCL_Seed > UINT64_MAX)
	{
		SCL_ResetSeed();
	}
}
