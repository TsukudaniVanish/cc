#include "cc.h"
//#include<stdarg.h>
#include<errno.h>

void error(char *fmt,...)
{
	va_list arg;
	va_start(arg,fmt);
	fprintf(stderr,fmt,arg);
	va_end(arg);
	exit(1);
}


char* file_open(char* path)
{
	FILE *fp = fopen(path , "r");
	if(!fp)
		error("ファイルが見つかりません パス: %s",path,strerror(errno));
	
	//ファイルの長さを調べる
	if(fseek(fp,0,SEEK_END) == -1)
	{
		error("%s : fseek :%s",path,strerror(errno));
	}
	
	size_t size = ftell(fp);

	if(fseek(fp,0,SEEK_SET) == -1)
	{
		error("%s : fseek :%s",path,strerror(errno));
	}

	char *buf = calloc(1,size +2);
	fread(buf,size,1,fp);

	//ファイルが改行で終わるようにする
	if(size == 0 || buf[size -1] != '\n')
	{
		buf[size] = '\n';
	}
	buf[size] = '\0';
	fclose(fp);

	return buf;
	
}
