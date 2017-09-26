#ifndef __NET_INTERFACE_H
#define __NET_INTERFACE_H

#define NET_BUFFSIE_MAX_CONTENT 500
#define NET_BUFFSIZE 512
#define NET_RECV_TRIES 5

extern int net_SendData(unsigned char* msg, int len);
extern int net_SendCmd(unsigned char* msg, int len, int cmd);
extern int net_ReceiveData(unsigned char** msg);
extern int net_ReceiveDataTimeout(OUT_USTRP msg, int tries);

#endif