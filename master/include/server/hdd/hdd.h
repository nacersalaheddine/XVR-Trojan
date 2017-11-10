#ifndef __HDD_H
#define __HDD_H


#define HDD_DEF_PATH "C:\\"
#define HDD_MAX_PATH 2000
#define HDD_BUFFSIZE HDD_MAX_PATH + 10

extern int hdd_VisableSize;
extern char hdd_Path[HDD_BUFFSIZE];

extern void hdd_Reset(void);
extern char* hdd_GetVisable(int* w);
extern int hdd_AppendPath(char* str);
extern int hdd_SetPath(char* str);
extern int hdd_IsFolder(void);
extern void hdd_Format(void);
extern int hdd_IsValidPath(char* str);
extern void hdd_GetAbsolute(char** path);

#endif