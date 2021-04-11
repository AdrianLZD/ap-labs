
int infof(const char *format, ...);
int warnf(const char *format, ...);
int errorf(const char *format, ...);
int panicf(const char *format, ...);

int main() {
    infof("%s %d", "Info log test", 1);
    infof("%s %d %s %s %c", "Info log test", 2, "another", "argument", '!');
    warnf("%s %d", "Warning log test", 1);
    warnf("%s %d %s %s %c", "Warning log test", 2, "another", "argument", '!');
    errorf("%s %d", "Error log test", 1);
    errorf("%s %d %s %s %c", "Error log test", 2, "another", "argument", '!');
    panicf("%s %d", "Panic log test", 1);
    panicf("%s %d %s %s %c", "Panic log test", 2, "another", "argument", '!');

    return 0;   
}
