#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "myheader.h"

int IsCharacter(char x){
    if(('A' <= x) && (x <= 'Z')){
        return 2;
    }else if(('a' <= x) && (x <= 'z')){
        return 1;
    }else return 0;
};

void Parser(word_t* table[], FILE* fp){
    char buff[BUFF_SIZE];
    char word_buff[BUFF_SIZE];
    int i, j, k, string_end;

    for(i = 0; feof(fp) == 0; i++){
        fgets(buff, BUFF_SIZE, fp);
        if(i%1000000 == 0) printf("%dM done\n", i/1000000);
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
}