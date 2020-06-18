#include <unistd.h>
#include <stdio.h>
#include "mypath.h"

int mycd(char *path){
  PathResol(path);
  if(chdir(path) != 0) puts("chdir error");
  return 0;
}
