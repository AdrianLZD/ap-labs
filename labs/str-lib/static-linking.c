#include <stdio.h>

int mystrlen(char *str);
char *add(char *origin, char *addition);
int find(char *origin, char *substr);
int mystrcmp(char *string1, char *string2);

int main(int argc, char**argv) {

    if(argc<2){
        printf("No command detected.\n");
        return 0;
    }

    if(mystrcmp(argv[1],"-add")==0){
        if(argc==4){
            printf("Initial Length : %d\n", mystrlen(argv[2]));
            char* added = add(argv[2], argv[3]);
            printf("New String     : %s\n", added);
            printf("New Length     : %d\n", mystrlen(added));
        }else{
            printf("Missing parameters for add.\n");
        }
        return 0;
    }
    
    if(mystrcmp(argv[1], "-find")==0){
        if(argc==4){
            int position = find(argv[2], argv[3]);
            if(position>-1){
                printf("[%s] string was found at [%d] position.\n", argv[3], position);
            }else{
                printf("[%s] string was not found.\n", argv[3]);
            }
            
        }else{
            printf("Missing parameters for find.\n");
        }
        return 0;
    }

    printf("No valid command detected.\n");
    return 0;
}
