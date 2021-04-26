#define _XOPEN_SOURCE 500
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <ftw.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "logger.h"

#define FILE_MAX_LENGTH 255   /* # chars in a file name */
#define PATH_MAX_LENGTH 4096  /* # chars in a path name including null */

typedef struct {
    int *array;
    size_t used;
    size_t size;
} Array;

void init_array(Array *a, size_t initialSize) {
    a->array = malloc(initialSize * sizeof(int));
    a->used = 0;
    a->size = initialSize;
}

void insert_to_array(Array *a, int element) {
    if (a->used == a->size) {
        a->size *= 2;
        a->array = realloc(a->array, a->size * sizeof(int));
    }
    a->array[a->used++] = element;
}

void free_array(Array *a) {
    free(a->array);
    a->array = NULL;
    a->used = a->size = 0;
}

void add_watcher_to_directories(int fdInotify);
void add_watcher(int fdInotify, char *directory);
void add_watcher2(int fdInotify, int *watchers, int eventwatcher, char *name);
char *find_path(int *watchers, int eventwatcher, int namelen);

int dirCount;
char *directories[PATH_MAX_LENGTH]; 
char *mainDirectory;
Array watchers;

int get_directory_tree(const char *path, const struct stat *sb, int tflag, struct FTW *ftwbuf){
    if (tflag == FTW_D && ftwbuf->level < 3) {
        directories[dirCount] = malloc(sizeof(char) * PATH_MAX_LENGTH);
        snprintf(directories[dirCount], PATH_MAX_LENGTH, "%s", path);
        dirCount++;
    }
    return 0;
}

void handle_events(int fdInotify, int *wd){
    char buf[4096] __attribute__ ((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;
    ssize_t len;
    while(1){
        len = read(fdInotify, buf, sizeof(buf));

        if (len == -1 && errno != EAGAIN) {
            errorf("Watcher read error.\n");
            errorf("ABORTING.\n");
            exit(0);
        }

        if (len <= 0){
            break;
        }
        
        for (char *ptr = buf; ptr < buf + len;
            ptr += sizeof(struct inotify_event) + event->len) {
            
            char *action;
            char *type;
            char *name;
            char *path;
            int directory = 0;
            event = (const struct inotify_event *) ptr;

            if (event->len){

                if (event->mask & IN_ISDIR){
                    type = strdup("Directory");
                    directory = 1;
                }else{
                    type = strdup("File");
                }

                name = strdup(event->name);
                path = find_path(wd, event->wd, strlen(name));

                if(event->mask & IN_CREATE){
                    action = strdup("Create");
                    infof("- [%s - %s] - %s%s\n", type, action,path, name);

                    if(directory){
                        char dir[strlen(mainDirectory) + strlen(name) + 3];
                        snprintf(dir, sizeof(dir), "/%s/%s", mainDirectory, name);
                        add_watcher2(fdInotify, wd, event->wd, name);
                    }

                }else if(event->mask & IN_DELETE){
                    action = strdup("Removal");
                    infof("- [%s - %s] - %s%s\n", type, action,path, name);
                }else if(event->mask & IN_MOVED_FROM){
                    action = strdup("Rename");
                    infof("- [%s - %s] - %s%s", type, action,path, name);
                }else if(event->mask & IN_MOVED_TO){
                    infofplain(" -> %s\n",name);
                }                
            }
        }
    }
}

int main(int argc, char *argv[]){
    if(argc != 2){
        errorf("Wrong usage. Try using: ./monitor $PATH\n");
        exit(0);
    }

    if(nftw(argv[1], get_directory_tree, 20, FTW_PHYS) != 0){
        errorf("There was an error creating the directory tree.\n");
        errorf("ABORTING.\n");
        exit(0);
    }

    int fdInotify;
    if((fdInotify = inotify_init1(IN_NONBLOCK)) < 0){
        errorf("There was an error initiating the inotify process.\n");
        errorf("ABORTING.\n");
        exit(0);
    }

    infof("Starting File/Directory Monitor on: %s\n", argv[1]);
    infof("-----------------------------------\n");

    mainDirectory = strdup(argv[1]);

    add_watcher_to_directories(fdInotify);
    
    while(1){
        for(int i = 0; i < watchers.size; i++){
            handle_events(fdInotify, watchers.array);
        }
    }

    return 0;
}

void add_watcher_to_directories(int fdInotify){
    init_array(&watchers, 50);
    for(int i = 0; i < dirCount; i++){
        add_watcher(fdInotify, directories[i]);
    }
}

void add_watcher(int fdInotify, char *directory){
    int wd;
    uint32_t mask = IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO;
    if((wd = inotify_add_watch(fdInotify, directory, mask))<0){
        errorf("Couldn't add watch to directory: %s\n", directory);
    }else{
        insert_to_array(&watchers, wd);
    }
}

void add_watcher2(int fdInotify, int *watchs, int eventwatcher, char *name){

    uint32_t mask = IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO;
    for(int i = 0; i<dirCount; i++){
        if(watchs[i] == eventwatcher){
            directories[dirCount] = malloc(sizeof(char) * PATH_MAX_LENGTH);
            snprintf(directories[dirCount], PATH_MAX_LENGTH, "%s/%s", directories[i], name);

            if((watchers.array[dirCount] = inotify_add_watch(fdInotify, directories[dirCount], mask))<0){
                errorf("Couldn't add watch to directory: %s\n", directories[dirCount]);
            }else{
                insert_to_array(&watchers, watchers.array[dirCount]);
            }
            dirCount++;
        }
    }
}

char *find_path(int *watchers, int eventwatcher, int namelen){
    char *fullpath;

    for(int i = 0; i < dirCount; i++){
        if(watchers[i] == eventwatcher){
            fullpath = malloc(sizeof(char) * strlen(directories[i]));
            snprintf(fullpath, sizeof(char) * (strlen(directories[i])+namelen+5), "%s", directories[i]);
        }
    }

    char *relativepath;
    relativepath = malloc(sizeof(fullpath));

    int index = 0;
    for(int i = strlen(mainDirectory)+1; i < strlen(fullpath); i++){
        relativepath[index++]= fullpath[i];
    }
    
    if(strlen(mainDirectory) == strlen(fullpath)){
        relativepath[index++] = '\0';
    }else{
        relativepath[index++] = '/';
        relativepath[index] = '\0';
    }

    return relativepath;
}



