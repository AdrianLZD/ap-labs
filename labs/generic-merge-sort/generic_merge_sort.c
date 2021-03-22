#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 5000

char *lineptr[MAX_LINES];
char *aux[MAX_LINES];

int readlines(char* path);
void writelines(char *path, int nlines);
int numcmp(char *s1, char *s2);
void mergesort(int (*cmp)(void *, void *), int left, int right);

int main(int argc, char **argv)
{
    if(argc<2){
        printf("Please include the correct parameters.\n");
        return 0;
    }
    
    int isNumeric = 0;
    int fileArg = 1;
    if(strcmp(argv[1],"-n")==0){
        if(argc>2){
            isNumeric = 1;
            fileArg = 2;
        }else{
            printf("The input file parameter is missing.\n");
            return 0;
        }
    }
    
    int nlines = readlines(argv[fileArg]);
    if(isNumeric){
        mergesort((int (*)(void *, void *))(numcmp),0, nlines-1);
    }else{
        mergesort((int (*)(void *, void *))(strcmp),0, nlines-1);
    }
    
    
    writelines(argv[fileArg], nlines);
    
    return 0;
}

void mergesort(int (*cmp)(void *, void *), int left, int right){
     
    if(right <= left){
        return;
    }
   
    int middle = (right+left)/2;
    
    mergesort(cmp,left, middle);
    mergesort(cmp, middle+1, right);

    int l_ptr = left;
    int r_ptr = middle +1;
    
    for(int i = left; i<=right; i++){
        if(l_ptr == middle + 1){
            aux[i] = strdup(lineptr[r_ptr]);
            r_ptr++;
        }
        else if(r_ptr == right + 1){
            aux[i] = strdup(lineptr[l_ptr]);
            l_ptr++;
        }
        else if((*cmp)(lineptr[l_ptr], lineptr[r_ptr])<0){
            aux[i] = strdup(lineptr[l_ptr]);
            l_ptr++;
        }
        else{
            aux[i] = strdup(lineptr[r_ptr]);
            r_ptr++;
        }
    }
    
    for(int i = left; i <= right; i++){
        lineptr[i] = aux[i];
    }
}

int numcmp(char *s1, char *s2){
    double num1 = atof(s1);
    double num2 = atof(s2);

    if(num1 < num2){
        return -1;
    }else if(num1==num2){
        return 0;
    }else{
        return 1;
    }
}

int readlines(char *path){
    FILE *file = fopen(path, "r");

    if(file==NULL){
        printf("The file [%d] could not be opened.\n", path);
        exit(0);
    }

    char *line;
    size_t len = 0;
    ssize_t read;

    int nlines = 0;

    while((read = getline(&line, &len, file))!=-1){
        if(nlines==MAX_LINES){
            printf("The file is too big to be read.\n");
            exit(0);
        }
        lineptr[nlines++] = strdup(line);
    }

    fclose(file);
    return nlines;
}

void writelines(char *path, int nlines){
    char *output = calloc(9+strlen(path), sizeof(char));
    strcat(output, "sorted_");
    strcat(output, path);

    printf("%s\n", output);
    FILE *file = fopen(output, "w");

    if(file==NULL){
        printf("The file [%s] could not be used for writting the results.\n", path);
        exit(0);
    }

    for(int i = 0; i<nlines; i++){
        fprintf(file,"%s", lineptr[i]);
    }

    printf("Result file can be found at ./%s\n", output);
}
