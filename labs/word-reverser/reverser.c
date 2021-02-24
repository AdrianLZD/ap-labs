#include <stdio.h>

void reverse_and_print(int length, char arr[]) {
    int i, tmp;
    for (i = 0;  i < length/2; i++) {
	    tmp = arr[i];
	    arr[i] = arr[length - i - 1];
	    arr[length - i - 1] = tmp;
    }
    for (int i = 0;  i < length; i++) {
	    printf("%c" , arr[i]);
    }
}

int main(){
    char currentChar;
    char currentWord[255];
    int wordIndex = 0;

    while((currentChar = getchar()) != EOF){
        if(currentChar == '\n'){
            reverse_and_print(wordIndex, currentWord);
            printf("\n");
            break;
        }else if(currentChar == ' ' || currentChar == '\t'){
            reverse_and_print(wordIndex, currentWord);
            printf("%c", currentChar);
            wordIndex = 0;
        }else{
            currentWord[wordIndex] = currentChar;
            wordIndex++;
        }
    }
    return 0;
}
