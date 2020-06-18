#ifndef MYSHELL_H
#define MYSHELL_H

int mycd(char*);
int mypwd();
char *mywhich(char *, char *, int);
void IgnoreSignals();
void AcceptSignals();

#endif