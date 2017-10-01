#ifndef __BLOCKLIST_H
#define __BLOCKLIST_H

#define BLOCKLIST_ESCAPE_KEY 0x1B

#define BLOCKLIST_NO_ERROR 0
#define BLOCKLIST_ERROR_FULL 1
#define BLOCKLIST_ERROR_INVALID_IP 2
#define BLOCKLIST_ERROR_OVER_MAX 3
#define BLOCKLIST_ERROR_HAS 4

#define BLOCKLIST_IP_MAX 15
#define BLOCKLIST_IP_MINIMAL 5
#define BLOCKLIST_MAX 124

extern int blocklist_IsInUse;
extern void blocklist_Init(void);
extern int blocklist_Count(void);
extern int BLOCKlist_GetSize(void);
extern int blocklist_Add(char* ip);
extern int blocklist_RemoveAt(int idx);
extern void blocklist_PrintList(int page);
extern int blocklist_IsBlocked(char* cip);
extern void blocklist_Clear(void);

#endif