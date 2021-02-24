#include <stdio.h>
#include <time.h>
#include <stdlib.h>


void reverse(int lenght, int arr[]) {

    int i, tmp;

    for (i = 0;  i < lenght/2; i++) {
	tmp = arr[i];
	arr[i] = arr[lenght - i - 1];
	arr[lenght - i - 1] = tmp;
    }
}

int main(int argc, char **argv) {

    int i, lenght, *arr;
    srand(time(NULL));

    if (argc < 2) {
	printf("You need to specify the size of the array:\n");
	printf("  ./reverse3 5\n");
	return -1;
    }

    lenght = atoi(argv[1]);
    arr = malloc(sizeof(int) * lenght);

    printf("Initial array: \n");
    for (i = 0;  i < lenght; i++) {
	arr[i] = rand() % 100;
	printf("%d " , arr[i]);
    }

    reverse(lenght, arr);

    printf("\n\nReversed array: \n");
    for (i = 0;  i < lenght; i++) {
	printf("%d " , arr[i]);
    }

    printf("\n");

    return 0;
}