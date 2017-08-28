#ifndef SERVER_H
#define SERVER_H

#define SERV_PORT 2666

#define SERV_NO_ERROR 0x0
#define SERV_WSASTARTUP 0x1
#define SERV_BIND 0x2
#define SERV_LISTEN 0x3
#define SERV_SELECT 0x4
#define SERV_ACCEPT 0x5
#define SERV_NFOUNDF 0x6
#define SERV_POEMACH 0x7
#define SERV_LOST_CONNECTION 0x8

extern int serv_Activated;
extern int serv_CreateServer(void);
extern int serv_StartServer(void);
extern void serv_CloseConnection(void);

#endif