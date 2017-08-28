#ifndef NET_INERFACE_H
#define NET_INERFACE_H

#define NET_BUFFSIZE 516
#define NET_RECV_TRIES 5

extern int net_reciveTries;
extern int net_SendData(SOCKET sock, unsigned char* msg, int len);
extern int net_ReciveData(SOCKET sock, unsigned char** msg);
extern int net_ReciveDataTimeout(SOCKET sock, unsigned char** msg);
extern int net_Execute(SOCKET sock);

#endif
