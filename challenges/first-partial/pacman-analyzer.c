#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct CmdLine{
    char *date;
    char *action;
}CmdLine;

typedef struct Package{
    char *lastUpdate;
    char *installDate;
    char *removalDate;
    short updates;
}Package;

typedef struct HashItem{
    char *key;
    char *lastUpdate;
    char *installDate;
    char *removalDate;
    int updates;
    struct HashItem *next;
} HashItem;

typedef struct HashTable{
    int size;
    HashItem **array;
} HashTable;



unsigned int hash(char *key, int size);
HashTable *htCreate(int size);
int htPut(HashTable *ht, char *key, HashItem *package);
int nodeHandler(HashTable *ht, HashItem *item);
HashItem *htGet(HashTable *ht, char *key);
void htFree(HashTable *ht);

void analizeLog(char *logFile, char *report);
void readFile(FILE *file);
void analizeLine(char *line);
void findDate(char **line, struct CmdLine *commandLine);
void findType(char **line);
int findAction(char **line, struct CmdLine *commandLine);
void findPackageInfo(char **line, struct CmdLine *commandLine);
void updatePackageList(char *packageName,struct CmdLine *commandLine);
void determineType(char **line, int *length);
void writeReport(char *reportPath);
void writeNoUpdates(FILE *reportFile);
void writePackagesInfo(FILE *reportFile);

void sliceArray(char **line, int newStart);

int installedPackages;
int removedPackages;
int upgradedPackages;
int currentInstalledPackages;
int scriptletCount;
int alpmCount;
int pacmanCount;
int dateSize=0;

char *oldestPackage;
char *newestPackage;
char *allPackages;

struct HashTable *packages;


int main(int argc, char **argv) {
    
    if (argc != 5) {
        printf("WRONG USAGE \n");
	    printf("Usage:./pacman-analizer.o -input input_file -report output_file \n");
	    return 0;
    }
    
    if(strcmp(argv[1],"-input")!=0){
        printf("WRONG USAGE \n");
        printf("Usage:./pacman-analizer.o -input input_file -output output_file \n");
	    return 0;
    }

    if(strcmp(argv[3],"-report")!=0){
        printf("WRONG USAGE \n");
        printf("Usage:./pacman-analizer.o -input input_file -output output_file \n");
	    return 0;
    }
    
    analizeLog(argv[2], argv[4]);
    
    return 0;
}

void analizeLog(char *logFile, char *report) {
    printf("Generating Report from: [%s] log file\n", logFile);
    

    FILE *inputFile = fopen(logFile, "r");
    if(inputFile==NULL){
        printf("The input file [%s] could not be opened.\n", logFile);
        return;
    }
    packages = htCreate(3000);
    readFile(inputFile);
    writeReport(report);
    
    htFree(packages); 
    
    printf("Report is generated at: [%s]\n", report);
}

void readFile(FILE *file){
    char *currentLine;
    size_t len;
    ssize_t read;
    
    allPackages = malloc(sizeof(char)+1);
    while((read = getline(&currentLine, &len, file)) != -1){
        analizeLine(currentLine);
    }
    fclose(file);
}

void analizeLine(char *line){
    CmdLine *commandLine = malloc(sizeof(CmdLine));
    if(line[0]=='['){
        findDate(&line, commandLine);
        findType(&line);
        if(findAction(&line, commandLine)==1){
            
            findPackageInfo(&line, commandLine);
            
        }
        //printArray(line);
        //printf("\n");
        //determineType(&line, &length);*/
    }
    
    
    //free(commandLine->date);
    //free(commandLine);
    
}

void findDate(char **line, struct CmdLine *commandLine){
    int dateEnd=0;
    char dateBuffer[255];
    int length = strlen(line[0]);
    for(int i = 1; i<length; i++){
        if(line[0][i] ==']'){
            dateBuffer[i-1]='\0';
            dateEnd = i + 2;
            if(dateSize==0){
                dateSize = i;
            }
            break;
        }else{
            dateBuffer[i-1]=line[0][i];
        }
    }
    commandLine->date = strdup(dateBuffer);
    memset(dateBuffer, 0, 255);

    sliceArray(line, dateEnd);
}

void findType(char **line){
    char *type = malloc(50*sizeof(char));
    int charCounter=0;
    int length = strlen(line[0]);
    for(int i = 1; i<length; i++){
        if(line[0][i] != ']'){
            charCounter ++;
            type[i-1] = line[0][i];
        }else{
            type[i-1]= '\0';
            break;
        }
    }
    if(strcmp(type, "ALPM")==0){
        alpmCount++;
    }else if(strcmp(type, "PACMAN")==0){
        pacmanCount++;
    }else if(strcmp(type, "ALPM-SCRIPTLET")==0){
        scriptletCount++;
    }
    free(type);
    sliceArray(line, charCounter+3);
}

int findAction(char **line, struct CmdLine *commandLine){
    char *action = malloc(60 * sizeof(char));
    int length = strlen(line[0]);
    int i=0;
    
    for(i = 0; i<length; i++){
        if(line[0][i] != ' '){
            action[i] = line[0][i];
        }else{
            action[i]= '\0';
            break;
        }
    }

    int hasPackage = 0;
    if(strcmp(action, "installed")==0 || strcmp(action, "reinstalled")==0){
        installedPackages++;
        hasPackage = 1;
    }else if(strcmp(action, "removed")==0){
        removedPackages++;
        hasPackage = 1;
    }else if(strcmp(action, "upgraded")==0){
        upgradedPackages++;
        hasPackage = 1;
    }

    commandLine->action = strdup(action);

    free(action);
    sliceArray(line, i+1);
    return hasPackage;
}

void findPackageInfo(char **line, struct CmdLine *commandLine){
    char *packageName = malloc(60 * sizeof(char));
    int length = strlen(line[0]);
    
    for(int i = 0; i<length; i++){
        if(line[0][i] != ' '){
            packageName[i] = line[0][i];
        }else{
            packageName[i]= '\0';
            break;
        }
    }
    updatePackageList(packageName, commandLine);
    
    
    //free(packageName);
}

void updatePackageList(char *packageName, struct CmdLine *commandLine){
    int nameLength = strlen(packageName);
    HashItem *pack;
    pack = malloc(sizeof(HashItem));
    pack->installDate = malloc(dateSize*sizeof(char));
    pack->lastUpdate = malloc(dateSize*sizeof(char));
    pack->removalDate = malloc(dateSize*sizeof(char));
    pack->installDate = "-";
    pack->lastUpdate = "-";
    pack->removalDate = "-";
    pack->updates = 0;
    int hashPut = htPut(packages, packageName, pack);
    if(hashPut>=0){
        if(hashPut==0){
            allPackages = realloc(allPackages, (strlen(allPackages) + nameLength+2)*sizeof(char));
            strcat(allPackages, packageName);
            strcat(allPackages, "\n");
        }
        
        if(strcmp(commandLine->action,"installed")==0 || strcmp(commandLine->action,"reinstalled")==0){
            if(oldestPackage==NULL){
                oldestPackage = strdup(packageName);
            }
            htGet(packages, packageName)->installDate = strdup(commandLine->date);
            newestPackage = strdup(packageName);
        }
        if(strcmp(commandLine->action,"upgraded")==0){
            htGet(packages, packageName)->lastUpdate = strdup(commandLine->date);
            htGet(packages, packageName)->updates += 1;

        }
        if(strcmp(commandLine->action,"removed")==0){
            htGet(packages, packageName)->removalDate = strdup(commandLine->date);
        }
    }    
    
}

void writeReport(char *reportPath){
    FILE *reportFile = fopen(reportPath, "w");
    fprintf(reportFile, "Pacman Packages Report\n");
    fprintf(reportFile,"----------------------\n");
    fprintf(reportFile,"- Installed packages : %d\n", installedPackages);
    fprintf(reportFile,"- Removed packages   : %d\n", removedPackages);
    fprintf(reportFile,"- Upgraded packages  : %d\n", upgradedPackages);
    fprintf(reportFile,"- Current installed  : %d\n", installedPackages-removedPackages);
    fprintf(reportFile,"-------------\n");
    fprintf(reportFile,"General Stats\n");
    fprintf(reportFile,"-------------\n");
    fprintf(reportFile,"- Oldest Package               : %s\n", oldestPackage);
    fprintf(reportFile,"- Newest Package               : %s\n", newestPackage);
    writeNoUpdates(reportFile);
    fprintf(reportFile,"- [ALPM-SCRIPTTLET] count      : %d\n", scriptletCount);
    fprintf(reportFile,"- [ALPM] count                 : %d\n", alpmCount);
    fprintf(reportFile,"- [PACMAN] count               : %d\n", pacmanCount);
    fprintf(reportFile,"----------------\n");
    fprintf(reportFile,"List of packages\n");
    fprintf(reportFile,"----------------\n");
    
    writePackagesInfo(reportFile);
    fclose(reportFile);
}

void writeNoUpdates(FILE *reportFile){
    fprintf(reportFile,"- Packagages with no updgrades : ");
    int length = strlen(allPackages);
    int bufferIndex = 0;
    char *currentName;

    char nameBuffer[255];
    HashItem *currentItem;
    memset(nameBuffer,0,255);
    for(int i = 0; i<length;i++){
        if(allPackages[i]=='\n'){
            currentName = strdup(nameBuffer);
            currentItem = htGet(packages, currentName);
            if(currentItem->updates>0){
                fprintf(reportFile,"%s, ", currentName);
            }
            memset(nameBuffer, 0, 255);
            free(currentName);
            bufferIndex=0;
        }else{
            nameBuffer[bufferIndex++] = allPackages[i];
        }
    }
    fprintf(reportFile,"\n");
}

void writePackagesInfo(FILE *reportFile){
    int length = strlen(allPackages);
    int bufferIndex = 0;
    char nameBuffer[255];
    char *currentName;
    HashItem *currentItem;
    memset(nameBuffer,0,255);
    for(int i = 0; i<length;i++){
        if(allPackages[i]=='\n'){
            currentName = strdup(nameBuffer);
            currentItem = htGet(packages, currentName);
            
            fprintf(reportFile,"- Package Name       : %s\n", currentItem->key);
            fprintf(reportFile,"  - Install date     : %s\n", currentItem->installDate);
            fprintf(reportFile,"  - Last Update date : %s\n", currentItem->lastUpdate);
            fprintf(reportFile,"  - How many updates : %d\n", currentItem->updates);
            fprintf(reportFile,"  - Removal date     : %s\n", currentItem->removalDate);
            
            memset(nameBuffer,0,255);
            free(currentName);
            bufferIndex = 0;
        }else{
            nameBuffer[bufferIndex] = allPackages[i];
            bufferIndex++;
        }
    }
    
}

HashTable *htCreate(int size){
    HashTable *ht;

    if (size < 1) {
        return NULL;
    }

    ht = malloc(sizeof(HashTable));
    if (ht == NULL) {
        return (NULL);
    }

    ht->array = (HashItem**)malloc(size * sizeof(HashItem));
    if (ht->array == NULL) {
        return (NULL);
    }

    memset(ht->array, 0, size * sizeof(HashItem));
    ht->size = size;

    return ht;
}

unsigned int hash(char *key, int size){
    unsigned int hash = 0;
    int i = 0;

    while (key && key[i]) {
        hash = (hash + key[i]) % size;
        i++;
    }
    return (hash);
}

int htPut(HashTable *hashtable, char *key, HashItem *package){
    HashItem *node;

    if (hashtable == NULL) {
        return -1;
    }
    node = malloc(sizeof(HashItem));
    
    if (node == NULL) {
        return -1;
    }

    node->key = strdup(key);
    node->installDate = package->installDate;
    node->lastUpdate = package->lastUpdate;
    node->removalDate = package->removalDate;
    node->updates = package->updates;
    
    return nodeHandler(hashtable, node);
}

int nodeHandler(HashTable *hashtable, HashItem *node){
    unsigned int i = hash(node->key, hashtable->size);
    HashItem *tmp = hashtable->array[i];
    HashItem *prev = NULL;
    int result = 0;
    if (hashtable->array[i] != NULL) {
        tmp = hashtable->array[i];
        while (tmp != NULL) {
            if(strcmp(tmp->key, node->key)==0){
                break;
            }
            prev = tmp;
            tmp = tmp->next;
        }

        if (tmp == NULL) {
            
            node->next = hashtable->array[i];
            hashtable->array[i] = node;
        } else {
            //free(tmp->package);
            //tmp->package = node->package;
            //free(node->package);
            free(node->key);
            free(node);
            //printf("%s", node->package->lastUpdate);
            result = 1;
        }
    } else {
        node->next = NULL;
        hashtable->array[i] = node;
    }
    return result;
}

HashItem *htGet(HashTable *hashtable, char *key){   
     
    if (hashtable == NULL) {
        return NULL;
    }
    
    char *key_cp = strdup(key);
    unsigned int i = hash(key, hashtable->size);
    HashItem *tmp = hashtable->array[i];
    
    while (tmp != NULL) {
        if (strcmp(tmp->key, key_cp) == 0) {
            break;
        }
        tmp = tmp->next;
    }
    
    free(key_cp);   
    if(tmp == NULL){
        return NULL;
    } 
    return tmp;
}

void htFree(HashTable *hashtable){
    HashItem *tmp;

    if (hashtable == NULL) {
        return;
    }
    for (int i = 0; i < hashtable->size; ++i) {
        if (hashtable->array[i] != NULL) {
            while(hashtable->array[i] != NULL) {
                tmp = hashtable->array[i]->next;
                free(hashtable->array[i]->key);
                //free(hashtable->array[i]->package->installDate);                
                //free(hashtable->array[i]->package->removalDate);
                //free(hashtable->array[i]->name);
                free(hashtable->array[i]);
                hashtable->array[i] = tmp;
            }
            free(hashtable->array[i]);
            }
        }
    free(hashtable->array);
    free(hashtable);
}

void sliceArray(char **array,  int newStart){
    int length = strlen(array[0]);
    char *newLine = calloc(length-newStart,sizeof(char));
    for(int i = 0; i<length-newStart; i++){
        newLine[i] = array[0][i+newStart];
    }

    *array = newLine;
}