#ifndef LOGGER_H
#define LOGGER_H

#define LOG_SUCC '+'
#define LOG_ERR '-'
#define LOG_INFO '*'
#define LOG_DEFAULT_USERNAME "Lucifer"
#define LOG_MAX_NAME_LEN 64

#define LOG_COLOR_TIME 0
#define LOG_COLOR_ERROR 1
#define LOG_COLOR_INFO 2
#define LOG_COLOR_SUCC 3
#define LOG_COLOR_TEXT 4
#define LOG_COLOR_USERNAME 5
#define LOG_COLOR_USERNAME_SEP 6
#define LOG_COLOR_PRGS_DEFAULT 7
#define LOG_COLOR_PRGS_HALF 8
#define LOG_COLOR_PRGS_FULL 9
#define LOG_COLOR_LS_FILE 10
#define LOG_COLOR_LS_FOLDER 11
#define LOG_COLOR_LS_UNWN 12
#define LOG_COLOR_DISK_LIST_FIXED 13
#define LOG_COLOR_DISK_LIST_REMOVABLE 14
#define LOG_COLOR_LIST_SIZE 15

#define LOG_FLUSH() fflush(stdout)

extern int log_Color;
extern int log_Time;
extern unsigned short log_colorPalette[];
extern void LOG_Init(void);
extern void LOG_LoadConfing(void);
extern void LOG(int logType, char* format, ...);
extern void LOG_USERNAME(void);
extern void LOG_SERVER_USERNAME(void);
extern void LOG_plus_SetColor(unsigned short color);

#endif
