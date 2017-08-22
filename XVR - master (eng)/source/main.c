#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "version.h"
#include "logger.h"
#include "server.h"

int main(int argc, char* args[])
{
	if(argc > 1)
	{
		if(!strcmp(args[1], "-PS"))
		{
			log_PS = 1;
		}
	}

	srand(time(NULL));
	SetConsoleOutputCP(65001); //65001 е за UTF-8
	LOG(LOG_INFO, "Version:%s\n", VERSION);
	LOG(LOG_INFO, "Creating the server...\n");

	int rv = serv_CreateServer();

	if(rv != SERV_NO_ERROR)
     {
		LOG(LOG_ERR, "Error when creating the server:%d\n", rv);
          LOG(LOG_ERR, "WSA Error:%d\n", WSAGetLastError());
          LOG(LOG_ERR, "Error::%d\n", GetLastError());

          return -1;
     }else{
          LOG(LOG_SUCC, "The server is created.\n");
     }

	LOG(LOG_INFO, "Starting the server...\n");

	while(1)
	{
		rv = serv_StartServer();

		if(rv == SERV_LOST_CONNECTION)
		{
			continue;
		}else if(rv != SERV_NO_ERROR){
			LOG(LOG_ERR, "Error when creating the server:%d\n", rv);
			LOG(LOG_ERR, "WSA Error:%d\n", WSAGetLastError());
			LOG(LOG_ERR, "Error::%d\n", GetLastError());

			return -2;
		}else{
			LOG(LOG_SUCC, "Server closed with no errors!\n");
			break;
		}
	}

	return 0;
}