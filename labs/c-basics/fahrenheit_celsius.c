#include <stdio.h>
#include <stdlib.h>

/* print Fahrenheit-Celsius table */

int main(int argc, char **argv)
{
    if(argc==2){
        float celsius = (5.0/9.0)*(atoi(argv[1])-32);
        printf("Fahrenheit: %s, Celcius: %6.1f\n", argv[1], celsius);

    }else if(argc==4){
        int limit = atoi(argv[2]);
        int increment = atoi(argv[3]);
        for (int fahr = atoi(argv[1]); fahr <= limit; fahr = fahr + increment)
            printf("Fahrenheit: %3d, Celcius: %6.1f\n", fahr,(5.0/9.0)*(fahr-32));

    }
    return 0;
}
