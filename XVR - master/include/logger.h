#ifndef LOGGER_H
#define LOGGER_H

#define LOG_SUCC '+'
#define LOG_ERR '-'
#define LOG_INFO '*'

#define LOG_FLUSH() fflush(stdout)

extern int log_PS;
extern void LOG(int logType, char* format, ...);

#endif
