#include <stdio.h>

void swap(int *px, int *py){
     int temp;
     temp = *px;
     *px = *py;
     *py = temp;
}

int main(){
    int x = 12;
    int y = 4;
    swap(&x,&y);
    printf("x %d, y %d\n", x, y);
}