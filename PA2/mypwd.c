#include <unistd.h>
#include <stdio.h>

int mypwd(){
    char temp[64] = {0};
    getcwd(temp, 64);
    puts(temp);
    return 0;
}
