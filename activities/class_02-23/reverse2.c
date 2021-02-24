#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define ARRAY_LENGHT 10

void reverse(int arr[ARRAY_LENGHT]) {

    int i, tmp;

    for (i = 0;  i < ARRAY_LENGHT/2; i++) {
	tmp = arr[i];
	arr[i] = arr[ARRAY_LENGHT - i - 1];
	arr[ARRAY_LENGHT - i - 1] = tmp;
    }
}

int main() {

    int i, arr[ARRAY_LENGHT];
    srand(time(NULL));

    printf("Initial array: \n");
    for (i = 0;  i < ARRAY_LENGHT; i++) {
	arr[i] = rand() % 100;
	printf("%d " , arr[i]);
    }

    reverse(arr);

    printf("\n\nReversed array: \n");
    for (i = 0;  i < ARRAY_LENGHT; i++) {
	printf("%d " , arr[i]);
    }

    printf("\n");

    return 0;
}