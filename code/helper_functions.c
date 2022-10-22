#include<stdio.h>
#include<stdlib.h>

void int_to_string(char* s,int d) {
    sprintf(s, "%d", d);
}

char* long_to_string(char* s, long d) {
    sprintf(s, "%ld", d);
}