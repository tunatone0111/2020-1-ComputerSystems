#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "token.h"
#include "mypath.h"
#include "myshell.h"

int PATHParsing(char* res)
{
    char path[128];
    strcpy(path, getenv("PATH"));
    char *ptr = strtok(path, ":");
    int i = 0;

    while (ptr != NULL)
    {
        strcpy(res + i++ * BUFFER_MAX, ptr);
        ptr = strtok(NULL, ":");
    }
    return i;
}

int PathResol(char* path){
    char resdir[BUFFER_MAX] = { 0 };
    char buff[BUFFER_MAX] = { 0 };
    char* stk[BUFFER_MAX] = {0};
    char *ptr1, *ptr2, *ptr3;
    int isdir = 0, tos = -1;

    if (strcmp(path, "/") == 0)
        return 2;

    if(path[0] == '~'){
        strcpy(resdir, "/home/");
        strcat(resdir, getlogin());
        strcpy(path, path+1);
    }else if(path[0] != '/'){
        getcwd(resdir, BUFFER_MAX);
        strcat(resdir, "/");
    }
    strcat(resdir, path);

    if (resdir[strlen(resdir) - 1] == '/') 
        isdir = 1;

    while ((ptr1 = strstr(resdir, "//")) != 0){
        memcpy(ptr1, ptr1 + 1, strlen(ptr1 + 1) + 1);
    }

    strcpy(buff, resdir);
    memset(resdir, 0x00, BUFFER_MAX);
    stk[++tos] = "/";
    ptr1 = strtok(buff+1, "/");
    while(ptr1 != NULL){
        if(strcmp(ptr1, "..") == 0){
            if(tos > 0) tos--;
        }
        else stk[++tos] = ptr1;

        ptr1 = strtok(NULL, "/");
    }
    strcpy(resdir, "/");
    for(int i = 1; i<tos; i++){
        strcat(resdir, stk[i]);
        strcat(resdir, "/");
    }
    strcat(resdir, stk[tos]);
    if(isdir == 1) strcat(resdir, "/"); 

    while((ptr1 = strstr(resdir, "./")) != 0){
        memcpy(ptr1, ptr1+2, strlen(ptr1+2)+1);
    }

    strcpy(path, resdir);

    strcpy(buff, resdir);
    ptr1 = strtok(buff, "/");
    ptr2 = ptr1;
    while(ptr1 != NULL){
        ptr2 = ptr1;
        ptr1 = strtok(NULL, "/");
    }
    resdir[strlen(resdir)-strlen(ptr2)-isdir] = (char)0x00;

    if(mywhich(resdir, ptr2, 1) != NULL){
        if(isdir == 1){
            return 2;
        }else return 1;
    }else return 0;
}