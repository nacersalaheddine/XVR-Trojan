#ifndef __NET_INTERFACE_H
#define __NET_INTERFACE_H

#define NET_BUFFSIZE 512
#define NET_DATA_BUFFSIZE 2048

#define NET_HEADER_SIZE 10 //guard not to be near max of NET_BUFFSIZE and future uses
#define NET_RECV_TRIES 5

#include <windows.h>
//WARNING:: UGLY

//working properly in MinGW, otherwise there is a problem with _setsockopt in some cases 
typedef int (PASCAL *__WSAStartup)(unsigned short, LPWSADATA);
typedef int (PASCAL *__WSACleanup)(void);
typedef int (PASCAL *__recv)(unsigned int, char*, int, int);
typedef int (PASCAL*__send)(unsigned int, const char*, int, int);
typedef struct hostent* (*__gethostbyname)(const char*);
typedef int (PASCAL *__closesocket)(unsigned int);
typedef int (PASCAL *__shutdown)(unsigned int, int);
typedef int (PASCAL *__setsockopt)(unsigned int, int, int, const char*, int);
typedef int (PASCAL *__ioctlsocket)(unsigned int, long, unsigned long*);
typedef unsigned int (PASCAL *__socket)(int, int, int);
typedef int (PASCAL *__select)(int, fd_set*, fd_set*, fd_set*, const struct timeval*);
typedef unsigned short (PASCAL *__htons)(unsigned short);
typedef int (PASCAL *__connect)(unsigned int, const struct sockaddr*, int);
typedef int (PASCAL *_WSAFDIsSet)(unsigned int, fd_set*); //for FD_ISSET(); using single "_" cuz in winsoc2.h are defined "__WSAFDIsSet"

extern __WSAStartup net_lib_WSAStartup;
extern __WSACleanup net_lib_WSACleanup;
extern __recv net_lib_recv;
extern __send net_lib_send;
extern __gethostbyname net_lib_gethostbyname;
extern __closesocket net_lib_closesocket;
extern __shutdown net_lib_shutdown;
extern __setsockopt net_lib_setsockopt;
extern __ioctlsocket net_lib_ioctlsocket;
extern __socket net_lib_socket;
extern __select net_lib_select;
extern __htons net_lib_htons;
extern __connect net_lib_connect;
extern _WSAFDIsSet net_lib_WSAFDIsSet;

#define FD_ISSET_NET(fd, set) net_lib_WSAFDIsSet((unsigned int)(fd), (fd_set *)(set))

extern int net_BuffLen;
extern int net_MaxContLen;
extern int net_Init(void);
extern void net_SetBuffer(int buff);
extern int net_SendData(unsigned char* msg, int len);
extern int net_SendCmd(unsigned char* msg, int len, int cmd);
extern int net_SendEmptyCmd(int cmd);
extern int net_ReceiveData(unsigned char** msg);
extern int net_ReceiveDataTimeout(unsigned char** msg, int tries);

#endif