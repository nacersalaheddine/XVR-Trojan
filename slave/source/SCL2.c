#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SCL2.h"
#include "types.h"

#define SCL2_UIN32_MAX 0xFFFFFFFF

_scl_info SCL_INFO;
_scl_key *SCL_KEY;

uint32 SCL2_rand(void)
{
	uint64 xrand = SCL_INFO.lastRand + SCL_INFO.upSeed + SCL_INFO.randSalt;
	uint64 yrand = SCL_INFO.lastRand + SCL_INFO.ranHelper;

	uint32 rval = 0;

	if(xrand >= SCL2_UIN32_MAX)
	{
		rval = (uint32)(xrand - SCL2_UIN32_MAX);
	}else{
		rval = (uint32)xrand;
	}

	if(yrand >= SCL2_UIN32_MAX)
	{
		SCL_INFO.lastRand = yrand - SCL2_UIN32_MAX;
	}else{
		SCL_INFO.lastRand += SCL_INFO.ranHelper;
	}

	return rval % SCL_INFO.upMax;
}

extern inline uint8 SCL2_enc(uint8 val)
{
	int i;
	uint8 rval = val;

	for(i = 0; i != SCL_KEY->times; i++)
	{
		rval = (rval ^ SCL_KEY->key[rand() % SCL_KEY->keyLen]) ^ SCL_KEY->salt[rand() % SCL_KEY->keyLen];
	}

	return rval;
}

extern inline uint8 SCL2_dec(uint8 val)
{
	int i;
	int krnd = 0x0;
	int srnd = 0x0;
	uint8 rval = val;

	for(i = 0; i != SCL_KEY->times; i++)
	{
		krnd = rand() % SCL_KEY->keyLen;
		srnd = rand() % SCL_KEY->keyLen;

		rval = (rval ^ SCL_KEY->salt[srnd]) ^ SCL_KEY->key[krnd];
	}
		
	return rval;
}

void SCL2_Encrypt(OUT_USTRP data, uint32 len)
{
	srand(SCL_INFO.seed);
	uint8* ndata = *data;

	int i;
	
	for(i = 0; i != len; i++)
	{
		ndata[i] = SCL2_enc(ndata[i]);
	}
}

void SCL2_Decrypt(OUT_USTRP data, uint32 len)
{
	srand(SCL_INFO.seed);
	uint8* ndata = *data;

	int i;
	
	for(i = 0; i != len; i++)
	{
		ndata[i] = SCL2_dec(ndata[i]);
	}
}

void SCL2_SeedUp(void)
{
	uint64 ns = SCL_INFO.seed + SCL2_rand();

	if(ns >= SCL2_UIN32_MAX)
	{
		SCL_INFO.seed = (uint32)(ns - SCL2_UIN32_MAX);

		return;
	}

	SCL_INFO.seed = (uint32)ns;
}

void SCL2_Reset(void)
{
	SCL_INFO.seed = SCL_INFO.seedRestore;
	SCL_INFO.lastRand = 1;
}

void SCL2_Destroy(void)
{
	memset(&SCL_INFO, 0, sizeof(SCL_INFO));

	if(SCL_KEY)
	{
		free(SCL_KEY->key);
		free(SCL_KEY->salt);
		free(SCL_KEY);
	}
}

void SCL2_Init(_scl_info sclinfo, _scl_key** sclkey)
{
	SCL2_Destroy();

	memcpy(&SCL_INFO, &sclinfo, sizeof(SCL_INFO));
	SCL_INFO.seedRestore = SCL_INFO.seed;
	SCL_INFO.lastRand = 1;

	_scl_key* _sclkey = *sclkey;
	SCL_KEY = (_scl_key*)malloc(_sclkey->size);
	memcpy(SCL_KEY, _sclkey, _sclkey->size);
	free(_sclkey);

	SCL2_Reset();
}