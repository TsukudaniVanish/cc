#include<stdio.h>
#include<stdlib.h>

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