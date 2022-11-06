#include "cc.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
extern void Memory_copy(void*, void*, unsigned int);
void expect(int kind ,Token_t **token) {
	if( (*token) -> kind > 100 |  kind != (*token) -> str[0])
	{
		error_at( (*token) -> str ,"expect %c but got %c", kind, (*token) -> str[0]);
	}
	else
	{
		(*token) = (*token) -> next;
	}
}

char *expect_ident(Token_t **token) {
	if((*token)->kind != TK_IDENT)
	{
		error_at( (*token)-> str, "invalid identifier name" );
	}
	else
	{
		char *name = calloc((*token) -> length, sizeof(char));
		Memory_copy(name,(*token) -> str, (*token) -> length);
		consume(token);
		return name;
	}
}

int expect_num(Token_t **token) {
	if((*token) -> kind != TK_CONST)
	{
		error_at( (*token) -> str , "number literal is expected");
	}
	else
	{	
		int v = (*token) -> val;	
		(*token) = (*token) -> next;
		return v;
	}
}

// this function does not call exit(1)
void error(char *fmt,...)
{
	va_list arg;
	va_start(arg,fmt);
	fprintf(stderr,fmt,arg);
	va_end(arg);
}

void error_at(char *loc,char *fmt,...){
    va_list arg;
    va_start(arg,fmt);

    //search location of loc in input
    char* line = loc;
    while (user_input < line && *line != '\n' )
    {
        --line;
    }
    char *end = loc;
    while (*end != '\n' && *end != '\0')
    {
        ++end;
    }
    
    //見つけた行の番号を計算
    int linenum = 0;
    for(char *p = user_input ; p < line ; p++)
	{
        if(*p == '\n')
		{
            ++linenum;
		}
	}

    int indent = fprintf(stderr , "%s :line %d :" , filepath, linenum);
    fprintf(stderr,"%.*s" , (int)(end - line) , *line == '\n'? line + 1: line);
	if((*line == '\n' && *end != '\n') || (*line != '\n'))
	{
		fprintf(stderr, "\n");
	}

    //point error place
    int pos = loc - line + indent;
	for(char* i = line + 1; i < loc; i++)
	{
		if(*i == '\t')
		{		
			fprintf(stderr, "\t");
			pos--;
		}
	}
	pos = pos >= 0? pos: 0;
	fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^");
    vfprintf(stderr,fmt,arg);
    fprintf(stderr,"\n");

    va_end(arg);
    exit(1);
}

void assert(char *test_name,char *format,...)
{
	va_list arg;
	va_start(arg,format);
	fprintf(stderr,"	\x1b[31mUnit test error at %s\x1b[m\n",test_name);
	vfprintf(stderr,format,arg);
	va_end(arg);
	return;
}
