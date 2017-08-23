#ifndef NET_INERFACE_H
#define NET_INERFACE_H

#include "types.h"

#define NET_IP YOUR DOMAIN OR IP
#define NET_PORT 2666

#define NET_BUFFSIZE 516
#define NET_RECV_TRIES 5
#define NET_WAIT_BETWEEN_FUNC() Sleep(1000)

extern int net_reciveTries;
extern int net_SendData(unsigned char* msg, int len);
extern int net_ReciveData(unsigned char** msg);
extern int net_ReciveDataTimeout(unsigned char** msg);
extern void net_Prepare(void);
extern void net_CreateSocket(void);
extern void net_Connect(void);

#endif
