#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "logger.h"

#define WHITESPACE 64
#define EQUALS     65
#define INVALID    66

static const unsigned char d[] = {
    66,66,66,66,66,66,66,66,66,66,64,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,62,66,66,66,63,52,53,
    54,55,56,57,58,59,60,61,66,66,66,65,66,66,66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,66,66,66,66,66,66,26,27,28,
    29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,66,
    66,66,66,66,66,66
};

int base64encode(const void* data_buf, size_t dataLength, char* result, size_t resultSize);
int base64decode (char *in, size_t inLen, unsigned char *out, size_t *outLen);
void encodeFile(FILE* fd, char* path);
void decodeFile(FILE* fd, char* path);
void createFile(char* path, char* content, long cntLen, char* suffix, int toRemove);
void displayProgress(int sig);

double progress = 0.0;

void encodeFile(FILE* fd, char* path){
    fseek(fd, 0L, SEEK_END);    // Set the cursor to the end of filee
    long fdlen = ftell(fd);
    long resultSize = ((fdlen + 2) / 3) * 4 + 1;
    char *result = calloc(resultSize, sizeof(char));
    rewind(fd);

    char *buffer;
    buffer = calloc(1, fdlen+1);
    for(long i = 0; i < fdlen; i++){
        fread(buffer+i, 1, 1, fd);
        progress = (i+1) * 33.0 / (fdlen+1) ;
    }
    fclose(fd);

    if(base64encode(buffer, fdlen, result, resultSize) != 0){
        errorf("The file %s could not be encoded\n", path);
        exit(0);
    };

    createFile(path, result, resultSize, "-encoded.txt", 4);
}

void decodeFile(FILE* fd, char* path){
    char *fdSuffix = strrchr(path, '-');
    if (fdSuffix && strcmp(fdSuffix, "-encoded.txt") != 0){
        errorf("The file %s does not has the suffix [-encoded.txt]\n", path);
        fclose(fd);
        exit(0);
    }

    fseek(fd, 0L, SEEK_END);    // Set the cursor to the end of file
    long fdlen = ftell(fd); 
    char *result = calloc(fdlen, sizeof(char));
    rewind(fd);
    
    char *buffer;
    buffer = calloc(1, fdlen+1);
    for(int i = 0; i< fdlen+1; i++){
        fread(buffer+i, 1, 1, fd);
        progress = (i+1) * 33.0 / (fdlen+1);
    }
    fclose(fd);
    
    if(base64decode(buffer, fdlen, result, &fdlen) != 0){
        errorf("The file %s could not be decoded\n", path);
        exit(0);
    };
    
    createFile(path, result, fdlen+1, "-decoded.txt", 12);
}

void createFile(char* path, char* content, long cntLen,  char* suffix, int toRemove){
    char* fileName = path;
    for(int i = 0; i < toRemove; i++){
        fileName[strlen(fileName)-1] = '\0';
    }
    strcat(fileName, suffix);

    FILE *fd = fopen(fileName, "w");
    if(fd == NULL){
        errorf("The file %s could not be created\n", fileName);
        exit(0);
    }

    char *c;
    int counter=1;
    for(c = content; *c != '\0'; c++){
        fputc(*c, fd);
        progress = 66.0 + ((counter++) * 34.0 / (cntLen));
    }

    progress = 66.0 + ((counter) * 34.0 / (cntLen)); // Should be 100
    fclose(fd);
}

int base64encode(const void* data_buf, size_t dataLength, char* result, size_t resultSize) {
   const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
   const uint8_t *data = (const uint8_t *)data_buf;
   size_t resultIndex = 0;
   size_t x;
   long counter = 3;
   uint32_t n = 0;
   int padCount = dataLength % 3;
   uint8_t n0, n1, n2, n3;

   /* increment over the length of the string, three characters at a time */
   for (x = 0; x < dataLength; x += 3) 
   {
      /* these three 8-bit (ASCII) characters become one 24-bit number */
      n = ((uint32_t)data[x]) << 16; //parenthesis needed, compiler depending on flags can do the shifting before conversion to uint32_t, resulting to 0
      
      if((x+1) < dataLength)
         n += ((uint32_t)data[x+1]) << 8;//parenthesis needed, compiler depending on flags can do the shifting before conversion to uint32_t, resulting to 0
      
      if((x+2) < dataLength)
         n += data[x+2];

      /* this 24-bit number gets separated into four 6-bit numbers */
      n0 = (uint8_t)(n >> 18) & 63;
      n1 = (uint8_t)(n >> 12) & 63;
      n2 = (uint8_t)(n >> 6) & 63;
      n3 = (uint8_t)n & 63;
            
      /*
       * if we have one byte available, then its encoding is spread
       * out over two characters
       */
      if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
      result[resultIndex++] = base64chars[n0];
      if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
      result[resultIndex++] = base64chars[n1];

      /*
       * if we have only two bytes available, then their encoding is
       * spread out over three chars
       */
      if((x+1) < dataLength)
      {
         if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
         result[resultIndex++] = base64chars[n2];
      }

      /*
       * if we have all three bytes available, then their encoding is spread
       * out over four characters
       */
      if((x+2) < dataLength)
      {
         if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
         result[resultIndex++] = base64chars[n3];
      }

      progress = 33.0 + ((counter+3) * 33.0 / (dataLength));
      counter += 3;
   }

   /*
    * create and add padding that is required if we did not have a multiple of 3
    * number of characters available
    */
   if (padCount > 0) 
   { 
      for (; padCount < 3; padCount++) 
      { 
         if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
         result[resultIndex++] = '=';
      } 
   }
   if(resultIndex >= resultSize) return 1;   /* indicate failure: buffer too small */
   result[resultIndex] = 0;
   progress = 66.0;
   return 0;   /* indicate success */
}

int base64decode (char *in, size_t inLen, unsigned char *out, size_t *outLen) { 
    char *end = in + inLen;
    char iter = 0;
    long counter = 1;
    uint32_t buf = 0;
    size_t len = 0;
    
    while (in < end) {
        unsigned char c = d[*in++];
        
        switch (c) {
        case WHITESPACE: continue;   /* skip whitespace */
        case INVALID:    return 1;   /* invalid input, return error */
        case EQUALS:                 /* pad character, end of data */
            in = end;
            continue;
        default:
            buf = buf << 6 | c;
            iter++; // increment the number of iteration
            /* If the buffer is full, split it into bytes */
            if (iter == 4) {
                if ((len += 3) > *outLen) return 1; /* buffer overflow */
                *(out++) = (buf >> 16) & 255;
                *(out++) = (buf >> 8) & 255;
                *(out++) = buf & 255;
                buf = 0; iter = 0;

            }   
        }

        progress = 33.0 + (counter++) * 33.0 / (inLen+1);
    }
   
    if (iter == 3) {
        if ((len += 2) > *outLen) return 1; /* buffer overflow */
        *(out++) = (buf >> 10) & 255;
        *(out++) = (buf >> 2) & 255;
    }
    else if (iter == 2) {
        if (++len > *outLen) return 1; /* buffer overflow */
        *(out++) = (buf >> 4) & 255;
    }

    *outLen = len; /* modify to reflect the actual output size */
    progress = 66.0;
    return 0;
}

void displayProgress(int sig){
    if(sig == SIGINT){
        warnf("Process was interrupted at %.2f%%\n", progress);
        exit(0);
    }

    if(sig == SIGUSR1){
        infof("Current progress is %.2f%%. To stop send SIGINT\n", progress);
    }
    
}

int main(int argc, char *argv[]){

    if(argc != 3){
        errorf("Wrong usage. Usage --enconde/--decode <file>\n");
        return 0;
    }

    char *fileextension = strrchr(argv[2], '.');
    if (fileextension && strcmp(fileextension, ".txt") != 0){
        errorf("Only .txt files are supported\n");
        return 0;
    }

    FILE *fd = fopen(argv[2], "r");
    if(fd == NULL){
        errorf("The file %s could not be opened\n", argv[2]);
        return 0;
    }

    signal(SIGINT, displayProgress);
    signal(SIGUSR1, displayProgress);

    if(strcmp(argv[1], "--encode") == 0){
        encodeFile(fd, argv[2]);
    }else if(strcmp(argv[1], "--decode") == 0){
        decodeFile(fd, argv[2]);
    }else{
        errorf("Wrong usage. Usage --enconde/--decode <file>\n");
        return 0;
    }
    return 0;
}
