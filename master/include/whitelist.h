#ifndef __WHITELIST_H
#define __WHITELIST_H

#define WHITELIST_ESCAPE_KEY 0x1B

#define WHITELIST_NO_ERROR 0
#define WHITELIST_ERROR_FULL 1
#define WHITELIST_ERROR_INVALID_IP 2
#define WHITELIST_ERROR_OVER_MAX 3
#define WHITELIST_ERROR_HAS 4

#define WHITELIST_IP_MAX 15
#define WHITELIST_IP_MINIMAL 5
#define WHITELIST_MAX 124

extern int whitelist_IsInUse;
extern void whitelist_Init(void);
extern int whitelist_Count(void);
extern int whitelist_GetSize(void);
extern int whitelist_Add(char* ip);
extern int whitelist_RemoveAt(int idx);
extern void whitelist_PrintList(int page);
extern int whitelist_IsKnown(char* cip);
extern void whitelist_Clear(void);
extern void whitelist_StoreAll(void);

#endif
