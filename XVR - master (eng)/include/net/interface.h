#ifndef NET_INERFACE_H
#define NET_INERFACE_H

#define NET_BUFFSIZE 516

#define NET_PROTECT_CPU() Sleep(2)

extern int net_SendData(SOCKET sock, char* msg, int len);
extern int net_ReciveData(SOCKET sock, char** msg);
extern int net_Execute(SOCKET sock);

#endif
