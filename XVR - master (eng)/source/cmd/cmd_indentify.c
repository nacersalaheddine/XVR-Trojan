#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include "net/commands.h"
#include "net/interface.h"
#include "logger.h"
#include "server.h"
#include "types.h"

int net_cmd_Indentify(char* msg, int msgLen, SOCKET sock)
{
	net_formatCmd(&msg, &msgLen, NET_CMD_INDENTIFY);
	net_makeToRawCmd(NET_CMD_INDENTIFY, &msg, &msgLen);

	if(net_SendData(sock, msg, msgLen) < 1)
	{
		return -2;
	}

	char* rmsg;

	int i = 0;
	int type = rmsg[0];
	int vtype = rmsg[1];
	char vmsg[3] = { NET_CMD_INDENTIFY, '+', 0 };
	
	while(i != 4)
	{
		if(net_ReciveData(sock, &rmsg) < 1)
		{
			LOG(LOG_ERR, "We didn't receive back answer!\n");
	
			return 1;
		}
	
		type = rmsg[0];
		vtype = rmsg[1];
		*rmsg++;
		*rmsg++;
	
		if(type == '-')
		{
			if(vtype == 'C')
			{
				LOG(LOG_ERR, "We didn't receive computer name!\n");
			}else if(vtype == 'U'){
				LOG(LOG_ERR, "We didn't receive username!");
			}else if(vtype == 'X'){
				LOG(LOG_ERR, "We didn't receive X cords!\n");
			}else if(vtype == 'Y'){
				LOG(LOG_ERR, "We didn't receive Y cords!\n");
			}else{
				continue;
			}
		}else if(type == '+'){
			if(vtype == 'C')
			{
				LOG(LOG_INFO, "Computer:%s\n", rmsg);
			}else if(vtype == 'U'){
				LOG(LOG_INFO, "Username:%s\n", rmsg);
			}else if(vtype == 'X'){
				LOG(LOG_INFO, "X:%s\n", rmsg);
			}else if(vtype == 'Y'){
				LOG(LOG_INFO, "Y:%s\n", rmsg);
			}else{
				continue;
			}

			if(net_SendData(sock, vmsg, 2) < 1)
			{
				*rmsg--;
				*rmsg--;
				free(rmsg);

				return -2;
			}

			i++;
		}
		
		*rmsg--;
		*rmsg--;
	}

	free(rmsg);
	
	return 1;
}