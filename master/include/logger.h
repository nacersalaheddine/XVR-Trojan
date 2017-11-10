#ifndef __LOGGER_H
#define __LOGGER_H

#define LOG_SUCC 0x0
#define LOG_INFO 0x1
#define LOG_WAR 0x2
#define LOG_ERR 0x3
#define LOG_CRITICAL 0x4
#define LOG_TABLE 0x5
#define LOG_NONE 0x6

#define LOG_COLOR_ORIGINAL 0x0
#define LOG_COLOR_TEXT 0x1
#define LOG_COLOR_TIME 0x2
#define LOG_COLOR_SUCC 0x3
#define LOG_COLOR_INFO 0x4
#define LOG_COLOR_WAR 0x5
#define LOG_COLOR_ERR 0x6
#define LOG_COLOR_CRITICAL 0x7
#define LOG_COLOR_USERNAME 0x8
#define LOG_COLOR_USERNAME_SEP 0x9
#define LOG_COLOR_HDD_PATH 0xA
#define LOG_COLOR_DISKLIST_NO_ROOT_DIR 0xB
#define LOG_COLOR_DISKLIST_REMOVABLE 0xC
#define LOG_COLOR_DISKLIST_FIXED 0xD
#define LOG_COLOR_DISKLIST_REMOTE 0xE
#define LOG_COLOR_DISKLIST_CDROM 0xF
#define LOG_COLOR_DISKLIST_RAMDISK 0x10
#define LOG_COLOR_DISKLIST_UNKNOWN 0x11
#define LOG_COLOR_LS_FILE 0x12
#define LOG_COLOR_LS_FOLDER 0x13
#define LOG_COLOR_LS_UNKNOWN 0x14
#define LOG_COLOR_PRGS_BAR_DEFAULT 0x15
#define LOG_COLOR_PRGS_BAR_HALF 0x16
#define LOG_COLOR_PRGS_BAR_FULL 0x17

#define LOG_COLOR_COUNT 0x18

#define LOG_OPT_COLOR "-color"
#define LOG_OPT_TIME "-time"
#define LOG_OPT_SAVE "-save"

#define LOG_NEWLINE() putchar('\n')

#define LOG_DEF_USERNAME "Root"
#define LOG_USERNAME_MAX 400
extern char log_Username[LOG_USERNAME_MAX];
extern unsigned char log_PrgsChar;

extern int log_op_Color;
extern int log_op_Time;

extern void LOG_Init(void);
extern int LOG_LoadConfig(void);
extern void LOG_LoadConfig_OutError(int rv);
extern void LOG_SetColor(int idx);
extern void LOG_CursorStatus(int of);
extern void LOG_PrintTime(void);
extern void LOG(int logType, char* format, ...);
extern void LOG_USER(void);
extern void LOG_USER_MASTER(void);
extern void LOG_ResetTitle(void);
extern void LOG_TitleMaster(void);
extern void LOG_TitleDefault(void);
extern void LOG_TablePrint(int max, char* format, ...);

#endif