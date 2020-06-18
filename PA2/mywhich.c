#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include "token.h"

char* mywhich(char* path, char* name, int n_env){
    DIR *pdir;
    struct dirent *temp;
    char* res = NULL;

    for(int i = 0; i < n_env; i++){    
        pdir = opendir(path + i * BUFFER_MAX);
        while ((temp = readdir(pdir)) != NULL)
        {
            if(strcmp(temp->d_name, name) == 0){
                res = (char*)calloc(BUFFER_MAX, 1);
                strcpy(res, path + i * BUFFER_MAX);
                strcat(res, "/");
                strcat(res, name);
                break;
            }
        }
        closedir(pdir);
    }
    return res;
}