#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <stdarg.h>
#include <time.h>
#include <windows.h>
#include "logger.h"

int log_PS = 0;

void LOG(int logType, char* format, ...)
{
    va_list vl;
    va_start(vl, format);

    time_t rtime;
    struct tm *tm;
    time(&rtime);
    tm = localtime(&rtime);

    if(log_PS)
    {
        char* _color = "\x1b[0m";
        
        if(logType == LOG_SUCC)
        {
            _color = "\e[0;32m";
        }else if(logType == LOG_ERR){
            _color = "\e[0;31m";
        }else if(logType == LOG_INFO){
            _color = "\e[1;33m";
        }

        printf("\x1b[1;30m[\x1b[0;37m%002d\\%002d\\%004d\x1b[1;30m][\x1b[0;37m%002d:%002d:%002d\x1b[1;30m][\x1b[0;37m%s%c\x1b[1;30m]\x1b[0;37m", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec, _color, logType);  
    }else{
        printf("[%002d\\%002d\\%004d][%002d:%002d:%002d][%c]", tm->tm_mday, tm->tm_mon + 1, tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec, logType);      
    }

    vprintf(format, vl);
    va_end(vl);
}