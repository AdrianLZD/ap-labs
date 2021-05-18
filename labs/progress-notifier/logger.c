#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <syslog.h>
#include <string.h>
#include "logger.h"

#define INFO_COLOR "\033[0;36m"
#define WARNING_COLOR "\033[0;33m"
#define ERROR_COLOR "\033[0;31m"
#define PANIC_COLOR "\033[0;35m"

int isSyslog;
int syslogPriority;

int initLogger(char *logType) {
    if(strcmp(logType, "syslog") == 0){
        isSyslog = 1;
        printf("Initializing Logger on: %s\n", logType);
    }else if(strcmp(logType, "stdout")==0 || strcmp(logType, "")==0){
        isSyslog = 0;
        printf("Initializing Logger on: %s\n", "stdout");
    }else{
        isSyslog = 0;
        printf("The Logger does not support the type: %s\n", logType);
        printf("Initializing Logger on default option: %s\n", "stdout");
    }
    return 0;
}

int printLog(char *color, char *type, const char *format, va_list args){

    if(isSyslog){
        char dest[strlen(format) + strlen(type) + 3];
        strcpy(dest, "[");
        strcat(dest, type);
        strcat(dest, "] ");
        strcat(dest, format);
        vsyslog(syslogPriority, dest, args);
    }else{
        char currentTime[26];
        time_t now = time(0);
        strftime (currentTime, 26, "%Y-%m-%d %H:%M:%S", localtime (&now));
        printf("%s%s [%s] ", color, currentTime, type);
        vprintf(format, args);
        printf("%c[0m", 0x1B);
        fflush(stdout);
    }

    va_end(args);

    return 0;
}

int infof(const char *format, ...){
    syslogPriority = LOG_INFO;
    va_list args;
    va_start(args, format);
    return printLog(INFO_COLOR, "INFO", format, args);
}

int infofplain(const char *format, ...){
    syslogPriority = LOG_INFO;
    va_list args;
    va_start(args, format);
    printf("%s", INFO_COLOR);
    vprintf(format, args);
    printf("%c[0m", 0x1B);
    fflush(stdout);
}

int warnf(const char *format, ...){
    syslogPriority = LOG_WARNING;
    va_list args;
    va_start(args, format);
    return printLog(WARNING_COLOR, "WARNING", format, args);
}

int errorf(const char *format, ...){
    syslogPriority = LOG_ERR;
    va_list args;
    va_start(args, format);
    return printLog(ERROR_COLOR, "ERROR", format, args);
}

int panicf(const char *format, ...){
    syslogPriority = LOG_CRIT;
    va_list args;
    va_start(args, format);
    return printLog(PANIC_COLOR, "PANIC", format, args);
}