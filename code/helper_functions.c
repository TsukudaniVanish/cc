#include<stdio.h>
#include<stdlib.h>

void int_to_string(char* s,int d) {
    sprintf(s, "%d", d);
}

char* long_to_string(char* s, long d) {
    sprintf(s, "%ld", d);
}

void test_print_int(int p)
{
    printf("%d",p);
}

void test_print(char *p)
{
    printf("%s",p);
}

void test_error()
{
    exit(1);
}