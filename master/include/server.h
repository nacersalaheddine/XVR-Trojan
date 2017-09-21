#ifndef __SERVER_H
#define __SERVER_H

#define SERVER_PORT 2666
#define SERVER_KEEPALIVE_INTERVAL 500
#define SERVER_KEEPALIVE_MAX 120

#define SERVER_NO_ERROR 0
#define SERVER_ERROR_WSASTARTUP 1
#define SERVER_ERROR_BIND 2
#define SERVER_ERROR_LISTEN 3

extern unsigned int server_Client;
extern int server_UsingPort;
extern int server_HasWhitelist;
extern char* server_WhitelistIp;
extern int server_WsaInit(void);
extern void server_Cleanup(void);
extern int server_Create(void);
extern int server_WaitForSlave(void);
extern void server_ConnectionHandle(void);
extern void server_CloseConnection();

#endif