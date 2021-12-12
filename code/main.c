#include "cc.h"
//#include<stdio.h>
#include<stdarg.h>
//#include<stdbool.h>
//#include<string.h>






char *user_input;




void error_at(char *loc,char *fmt,...){


	va_list arg;
	va_start(arg,fmt);
	int pos = loc - (user_input);
	fprintf(stderr,"%s\n",user_input);
	fprintf(stderr,"%*s",pos," ");
	fprintf(stderr,"^");
	vfprintf(stderr,fmt,arg);
	fprintf(stderr,"\n");
	exit(1);

}










int main(int argc, char **argv){

	

	char *buffer;
	if( strncmp(argv[1],"-f",2) == 0)
	{
		buffer = file_open(argv[2]);
	}
	else
	{
		//グローバル変数に代入　エラー出力用
		user_input = argv[1];
		buffer = argv[1];
	}
		
	


	// fuction ごとのコード
	Node_t *code[100];
	
	//入力をトークン列に変換
	Token_t *token = tokenize(buffer);


	//token を抽象構文木に変換
	program(&token,code);

	//関数スコープ識別子テーブルを先頭にセット
	nametable = nametable -> head;

	

//アセンブリ前半を出力
	printf(".intel_syntax noprefix\n");
	if(string_iter)
		set_stringiter();
	printf("	.data\n");

	int funcflag = 1;
	//先頭の式からコード生成
	//抽象構文木を降りてコード生成
	//スタックトップには式の結果が入っている
	for(int i =0;code[i];i++){
		if( funcflag && code[i] -> kind == ND_FUNCTIONDEF )
		{
			printf("	.text\n");
			printf("	.global main\n");

			funcflag = 0;
		}
		generate(code[i]);
		if(funcflag == 0 && nametable)
			nametable = nametable -> next;

	}

	return 0;
}
