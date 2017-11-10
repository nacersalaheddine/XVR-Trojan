#ifndef __NET_INTERFACE_H
#define __NET_INTERFACE_H

#define NET_BUFFSIZE 512
#define NET_DATA_BUFFSIZE 2048

#define NET_HEADER_SIZE 10
#define NET_RECV_TRIES 5

extern int net_Ping;
extern int net_BuffLen;
extern int net_MaxContLen;
extern void net_SetBuffer(int buff);
extern int net_CheckConnection(void);
extern int net_SendData(unsigned char* msg, int len);
extern int net_SendCmd(unsigned char* msg, int len, int cmd);
extern int net_SendEmptyCmd(int cmd);
extern int net_ReceiveData(unsigned char** msg);
extern int net_ReceiveDataTimeout(unsigned char** msg, int tries);
extern void net_PrintLastError(void);

#endif