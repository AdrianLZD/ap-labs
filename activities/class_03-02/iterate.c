#include <stdio.h>

int main(){
    int a[10];
    for (int i = 0; i< 10; i++){
        a[i] = i;
    }
    
    int *pa;
    pa = a;
    for (pa; pa<= &a[9]; pa++){
        printf("%d ", *pa);
    }
    
    printf("\npa: %d\n", *pa);
}