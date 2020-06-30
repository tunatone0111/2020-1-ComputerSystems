#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "myheader.h"

int IsCharacter(char x){
    if(('A' <= x) && (x <= 'Z')){
        return 2;
    }else if(('a' <= x) && (x <= 'z')){
        return 1;
    }else return 0;
};

void Parser(void* args){
    word_t** table = ((args_t*)args)->table;
    FILE* fp = ((args_t*)args)->fp;
    int startpoint = ((args_t*)args)->startpoint;
    int endpoint = ((args_t*)args)->endpoint;

    char buff[BUFF_SIZE];
    char word_buff[BUFF_SIZE];
    int i, j, k, string_end;

    fseek(fp, startpoint, SEEK_SET);
    while(ftell(fp) < endpoint){
        fgets(buff, BUFF_SIZE, fp);
        j = 0;
        k = 0;
        string_end = 0;

        while(buff[j] != '\n'){

            while(!IsCharacter(buff[j])){
                if(buff[j+1] == '\n') string_end = 1;
                j++;
            }
            if(string_end) break;
            k = j;

            while(IsCharacter(buff[j])){
                // small letter
                if(IsCharacter(buff[j]) == 2) buff[j] = buff[j]-'A'+'a';
                j++;
            }

            // end of word, then send it to word buffer
            memcpy(word_buff, buff+k, j-k);
            word_buff[j-k] = '\0';
            WordCount(table, word_buff);
        }

    }
    pthread_exit(0);
}