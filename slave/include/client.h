#ifndef __CLIENT_H
#define __CLIENT_H

#define CLIENT_IP -->YOUR MASTER IP
#define CLIENT_PORT 2666

#define CLIENT_SLEEP_BETWEEN_FUNC() Sleep(500)
#define CLIENT_CHECK_CONNECTION_TIMER() Sleep(2000)

extern unsigned int client_Socket;
extern void client_Prepare(void);
extern void client_Disconnect(void);
extern void client_Connect(void);
extern void client_ConnectionHandle(void);

#endif