#include <stdio.h>
#include <stdlib.h>

static const char *MONTHS[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
static const int DAYS[2][13] = {{31,28,31,30,31,30,31,31,30,31,30,31,365},{31,29,31,30,31,30,31,31,30,31,30,31,366}};

void month_day(int year, int yearday, int *pmonth, int *pday);

int main(int argc, char **argv) {
    if (argc != 3){
        printf("Wrong Usage: It should be [./month-day <year> <yearday>]");
        return 0;
    }

    int month=0;
    int day=0;
    month_day(atoi(argv[1]), atoi(argv[2]), &month, &day);
    return 0;
}

void month_day(int year, int yearday, int *pmonth, int *pday){
    if(yearday<1){
        printf("The day %d is not valid.\n", yearday);
        return;
    }

    int isLeap = year%4==0 && year%100!=0 || year%400==0;
    if(yearday > DAYS[isLeap][12]){
        printf("The day %d is not in the selected year.\n", yearday);
        return;
    }

    for(int i = 0, sum = 0; i<12; i++){
        sum += DAYS[isLeap][i];
        if(sum >= yearday){
            sum -= DAYS[isLeap][i];
            *pday = yearday-sum;
            *pmonth = i;
            break;
        }
    }

    char cero = '\0';
    if(*pday<10){
        cero = '0';
    }
    printf("%s %c%d, %d\n", MONTHS[*pmonth],cero, *pday, year);
    
}
