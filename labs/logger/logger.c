
#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#define INFO_COLOR "\033[0;36m"
#define WARNING_COLOR "\033[0;33m"
#define ERROR_COLOR "\033[0;31m"
#define PANIC_COLOR "\033[0;35m"

int printLog(char *color, char *type, const char *format, va_list args);
int infof(const char *format, ...);
int warnf(const char *format, ...);
int errorf(const char *format, ...);
int panicf(const char *format, ...);

int printLog(char *color, char *type, const char *format, va_list args){
    char currentTime[26];
    time_t now = time(0);
    strftime (currentTime, 26, "%Y-%m-%d %H:%M:%S", localtime (&now));

    printf("%s%s [%s] ", color, currentTime, type);
    vprintf(format, args);
    va_end(args);
    printf("\n");
    
    //Resets default color
    printf("%c[0m", 0x1B);
    return 0;
}

int infof(const char *format, ...){
    va_list args;
    va_start(args, format);
    return printLog(INFO_COLOR, "INFO", format, args);
}

int warnf(const char *format, ...){
    va_list args;
    va_start(args, format);
    return printLog(WARNING_COLOR, "WARNING", format, args);
}

int errorf(const char *format, ...){
    va_list args;
    va_start(args, format);
    return printLog(ERROR_COLOR, "ERROR", format, args);
}

int panicf(const char *format, ...){
    va_list args;
    va_start(args, format);
    return printLog(PANIC_COLOR, "PANIC", format, args);
}