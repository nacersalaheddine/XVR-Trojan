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

	uint8* rmsg;
	int i = 0;
	int type = 0;
	int vtype = 0;
	
	while(i != 4)
	{
		if(net_ReciveDataTimeout(sock, &rmsg) < 1)
		{
			LOG(LOG_ERR, "We didn't receive back answer!\n");
	
			return 1;
		}
	
		type = rmsg[0];
		vtype = rmsg[1];
		rmsg += 2;
	
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
				LOG(LOG_INFO, "X:%d\n", ((rmsg[0] & 0xFF) << 24) | ((rmsg[1] & 0xFF) << 16) | ((rmsg[2] & 0xFF) << 8) | (rmsg[3] & 0xFF));
			}else if(vtype == 'Y'){
				LOG(LOG_INFO, "Y:%d\n", ((rmsg[0] & 0xFF) << 24) | ((rmsg[1] & 0xFF) << 16) | ((rmsg[2] & 0xFF) << 8) | (rmsg[3] & 0xFF));
			}else{
				continue;
			}

			i++;
		}
		
		rmsg -= 2;
	}

	free(rmsg);
	
	return 1;
}