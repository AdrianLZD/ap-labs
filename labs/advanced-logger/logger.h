// Logger

int initLogger(char *logType);
int printLog(char *color, char *type, const char *format, va_list args);
int infof(const char *format, ...);
int warnf(const char *format, ...);
int errorf(const char *format, ...);
int panicf(const char *format, ...);

