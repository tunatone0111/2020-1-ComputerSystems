#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "token.h"
#include "mypath.h"
#include "myshell.h"

int main(){
    char s[BUFFER_MAX][BUFFER_MAX][BUFFER_MAX];
    char *path = (char*)calloc(BUFFER_MAX, BUFFER_MAX), *prog;
    char *args[BUFFER_MAX];
    int n, n_env, j = 0, bgflag = 0;
    pid_t pid , pidio;
    char temp[BUFFER_MAX] = {0}, temp_ir[BUFFER_MAX], temp_or[BUFFER_MAX];
    int pipeflag = 0, pipeflag2 = 0, iredflag1 = 0, iredflag2 = 0, oredflag1 = 0, oredflag2 = 0;

    n_env = PATHParsing(path);
    IgnoreSignals();

    while(1){
        pid = getpid();
        getcwd(temp, BUFFER_MAX);
        if(pid != 0) printf("[junha@pa2myshell:%s]$ ", temp);
        memset(s, 0x00, BUFFER_MAX*BUFFER_MAX*BUFFER_MAX);
        memset(temp_ir, 0x00, BUFFER_MAX);
        memset(temp_or, 0x00, BUFFER_MAX);
        n = GetToken(s);
        bgflag = 0;
        if(n == 0) continue;
        if(memcmp(s[0][0], "exit", 4) == 0) break;
        pipeflag = 0;

        for(int i = 0; i < n; i++){//pipes
            j = 0;
            pipeflag2 = pipeflag;
            pipeflag = 0;
            iredflag1 = 0;
            oredflag1 = 0;
            for (int i = 0; i < BUFFER_MAX; i++)
                args[i] = (char *)NULL;

            while(s[i][j][0] != 0x00){//executions
                if(s[i][j][0] == '|'){
                    pipeflag = 1;
                    j++;
                    break;
                }
                if(memcmp(s[i][j], "<", 1) == 0){
                    iredflag1 = 1;
                    iredflag2 = iredflag1;
                    j++;
                    continue;
                }else if(memcmp(s[i][j], ">>", 2) == 0){
                    oredflag1 = 2;
                    oredflag2 = oredflag1;
                    j++;
                    continue;
                }else if(memcmp(s[i][j], ">", 1) == 0){
                    oredflag1 = 1;
                    oredflag2 = oredflag1;
                    j++;
                    continue;
                }
                
                if(s[i][j][0] == '&'){
                    bgflag = 1;
                    j++;
                    continue;
                }

                if(iredflag2 != 0){
                    iredflag2 = 0;
                    strcpy(temp_ir, s[i][j]);
                    PathResol(temp_ir);
                    j++;
                    continue;
                }else if(oredflag2 != 0){
                    oredflag2 = 0;
                    strcpy(temp_or, s[i][j]);
                    PathResol(temp_or);
                    j++;
                    continue;
                }
                args[j] = (char*)calloc(BUFFER_MAX, sizeof(char));
                strcpy(args[j], s[i][j]);
                j++;
            }
            if(pipeflag == 1){
                oredflag1 = 1;
                strcpy(temp_or, "iotemp_");
                temp_or[6] = 65+i;
                temp_or[7] = 0x00;
                strcat(temp_or, ".mytype");
            }
            if (pipeflag2 == 1){
                iredflag1 = 1;
                strcpy(temp_ir, "iotemp_");
                temp_ir[6] = 64+i;
                temp_ir[7] = 0x00;
                strcat(temp_ir, ".mytype");
            }

            if (memcmp(s, "cd", 2) == 0){
                mycd(args[1]);
            }

            pidio = fork();
            if(pidio == 0){
                int fd1, fd2;
                if(iredflag1 == 1){
                    fd1 = open(temp_ir, O_RDONLY);
                    if(fd1<0){
                        perror("error");
                        exit(-1);
                    }
                    dup2(fd1, STDIN_FILENO);
                    close(fd1);
                }
                if(oredflag1 != 0){
                    if(oredflag1 == 2){
                        fd2 = open(temp_or, O_RDWR | O_APPEND | S_IROTH, 0644);
                    }else if(oredflag1 == 1){
                        fd2 = open(temp_or, O_RDWR | O_CREAT | S_IROTH, 0644);
                    }
                    if (fd2 < 0){
                        perror("error");
                        exit(-1);
                    }
                    dup2(fd2, STDOUT_FILENO);
                    close(fd2);
                }

                if(strstr(args[0], "/") == NULL){
                    if(memcmp(s, "which", 5) == 0){
                        char *temp;
                        if((temp = mywhich(path, args[1], n_env)) != NULL)
                            puts(temp);
                        else printf("%s: command not found\n", args[0]);

                    }else if (memcmp(s, "cd", 2) == 0){
                    
                    }else if(memcmp(s, "pwd", 3) == 0){
                        mypwd();

                    }else if((prog = mywhich(path, args[0], n_env)) != NULL){
                        pid = fork();
                        if(pid == 0){
                            AcceptSignals();
                            execv(prog, args);
                        }
                        waitpid(pid, NULL, 0);
                        free(prog);

                    }else{
                        printf("%s: command not found\n", args[0]);
                    }
                }else{
                    switch(PathResol(args[0])){
                        case 1:
                            pid = fork();
                            if(pid == 0){
                                AcceptSignals();
                                execv(args[0], args);
                            }
                            waitpid(pid, NULL, 0);
                            break;
                        case 2:
                            printf("%s: Is a directory\n", args[0]);
                            break;
                        default:
                            printf("%s: No such file or directory\n", args[0]);
                    }
                }
                exit(0);
            }
            if(bgflag == 0) waitpid(pidio, NULL, 0);

            for (int i = 0; i < BUFFER_MAX; i++)
                if(args[i] != NULL) free(args[i]);
        }

    }


    free(path);
    return 0;
}

void IgnoreSignals(){
    signal(SIGINT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
}

void AcceptSignals(){
    signal(SIGINT, SIG_DFL);
    signal(SIGTSTP, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);
}
