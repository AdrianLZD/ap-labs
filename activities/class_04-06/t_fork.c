#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

static int idata = 111;

int main(int argc, char *argv[]){
    int istack = 222; //Allocated in stack segment
    pid_t childPid;

    switch (childPid = fork()){
        case -1:
            exit(1);
        case 0:
            idata *= 3;
            istack *= 3;
            break;
        default:
            sleep(3);
            break;
    }

    printf("PID=%ld %s idata=%d istack=%d\n", (long) getpid(),
        (childPid == 0) ? "child " : "(parent", idata, istack);
    exit(0);
}