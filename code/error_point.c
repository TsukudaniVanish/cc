#include "cc.h"
//#include<stdarg.h>

void error_at(char *loc,char *fmt,...){

    va_list arg;
    va_start(arg,fmt);


    //loc の含まれている場所を検索
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
    int linenum;
    for(char *p = user_input ; p < line ; p++)
        if(*p == '\n')
            ++linenum;

    int indent = fprintf(stderr , "%s :line %d : " , filepah, linenum);//ファイル名と行番号表示
    fprintf(stderr,"%.*s\n" , (int)(end - line) ,line);

    //エラー個所を指摘
    int pos = loc - line + indent;
    fprintf(stderr,"%*s", pos, " ");
    fprintf(stderr, "^");
    fprintf(stderr,fmt,arg);
    fprintf(stderr,"\n");

    va_end(arg);
    exit(1);
}