#include <stdlib.h>

int mystrcmp(char *string1, char *string2);
void cleanarray(char *toClean);

int mystrlen(char *str){
    int res = 0;
    while(str[res]!='\0'){
        res++;
    }
    return res;
}

char *add(char *origin, char *addition){
    int l1 = mystrlen(origin);
    int l2 = mystrlen(addition);
    char* res = (char *)malloc(l1+l2+1);
    int i = 0;
    for(i = 0; i < l1; i++){
        res[i] = origin[i];
    }
    for(i; i < l1+l2; i++){
        res[i] = addition[i-l1];
    }
    return res;
}

int find(char *origin, char *substr){
    int originLen = mystrlen(origin);
    int subIndex = 0;
    char* buffer = (char*)malloc(originLen+1);
    for(int i = 0;i<originLen; i++){
        if((origin[i] == substr[subIndex])){
            buffer[subIndex] = origin[i];
            if(mystrcmp(buffer, substr)==0){
                return i - mystrlen(buffer) + 1;
            }
            subIndex++;
        }else{
            cleanarray(buffer);
            subIndex=0;
        }
    }
    return -1;
}

void cleanarray(char *toClean){
    int i = 0;
    while(toClean[i]!='\0'){
        toClean[i] = '\0';
        i++;
    }
}

int mystrcmp(char *string1, char *string2){
    int len = mystrlen(string1);
    if(len != mystrlen(string2)){
        return -1;
    }

    for(int i = 0;i<len; i++){
        if(string1[i]!=string2[i]){
            return -1;
        }
    }

    return 0;
}
