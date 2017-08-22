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
	LOG(LOG_INFO, "Версия:%s\n", VERSION);
	LOG(LOG_INFO, "Създаване на сървара...\n");
	LOG(LOG_INFO, "Кое ИП да допусним да се свърже ? [празно място за сяко]\n");
	
	int rv = serv_CreateServer();

	if(rv != SERV_NO_ERROR)
     {
		LOG(LOG_ERR, "Грешка при създаването на сървар:%d\n", rv);
          LOG(LOG_ERR, "WSA последна грешка:%d\n", WSAGetLastError());
          LOG(LOG_ERR, "Други грешки:%d\n", GetLastError());

          return -1;
     }else{
          LOG(LOG_SUCC, "Сървара е създаден\n");
     }

	LOG(LOG_INFO, "Стартиране на сървара...\n");

	while(1)
	{
		rv = serv_StartServer();

		if(rv == SERV_LOST_CONNECTION)
		{
			continue;
		}else if(rv != SERV_NO_ERROR){
			LOG(LOG_ERR, "Сървара спря със грешка:%d\n", rv);
			LOG(LOG_ERR, "WSA последна грешка:%d\n", WSAGetLastError());
			LOG(LOG_ERR, "Други грешки:%d\n", GetLastError());

			return -2;
		}else{
			LOG(LOG_SUCC, "Сървара спря без грешки!\n");
			break;
		}
	}

	return 0;
}