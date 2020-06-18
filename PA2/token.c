#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"

int GetToken(char(*res)[BUFFER_MAX][BUFFER_MAX]){
    int i = 0, j = 0;
    char s[BUFFER_MAX] = { 0 };
    fgets(s, BUFFER_MAX, stdin);
    s[strlen(s)-1] = '\0';

    char *ptr = strtok(s, " ");
    while(ptr != NULL){

        if(memcmp(ptr, "|", 1) == 0){
            res[i][j++][0] = '|';
            res[i][j++][0] = '\0';
            i++;
            j = 0;
        }
        else{
            strcpy(res[i][j++], ptr);
        }

        ptr = strtok(NULL, " ");
    }
    strcpy(res[++i][0], "NULL");

    return i;
}