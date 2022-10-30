#include<stdio.h>
#include<stdlib.h>
void sprintf_h(char* s, char* format, long d) {
    sprintf(s, format, d);
}

int exit_h(int code) {
    exit(code);
}