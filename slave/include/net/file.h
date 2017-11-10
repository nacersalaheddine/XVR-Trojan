#ifndef __NET_FILE_H
#define __NET_FILE_H

#define NET_FILE_DATA 0x1
#define NET_FILE_END 0x2
#define NET_FILE_FAILED 0x3
#define NET_FILE_GOOD 0x4

extern int net_File_Send(char* path);
extern int net_File_Recv(char* path);

#endif