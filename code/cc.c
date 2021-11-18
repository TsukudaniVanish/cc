#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>




typedef enum{
	TK_OPERATOR, //記号
	TK_DIGIT, //整数
	TK_EOF, //終了記号
}Token_kind;


typedef struct Token Token_t;

struct Token {
	Token_kind kind;
	Token_t *next;
	int val;//if kind == TK_DIGIT　-> val = the number
	char *str;//string of token

};




//エラーをはく関数
void error(char *fmt,...){//printf と同じ引数をとる


	va_list arg;
	va_start(arg,fmt);
	vfprintf(stderr,fmt,arg);
	fprintf(stderr,"\n");
	va_end(arg);
	exit(1);

};





//token.kindが'op'か判定してtoken = token.next
// char OPERATOR,Token_t TOKEN -> bool
bool find(char operator,Token_t **token){

	if( (*token) -> kind != TK_OPERATOR | (*token) -> str[0] != operator ){
		return false;
	}else{
		*token = (*token) -> next;
		return true;
	}
};





//token.kind == TL_DIGIT なら token = token.next して val を返す
//Token_t *token -> int
int number(Token_t **token){

	if( (*token) -> kind != TK_DIGIT ){
		error("数ではありません");
	}else{
		int val = (*token) -> val;
		*token = (*token) -> next;
		return val;
	}
};



//新しいtokenを作り　cur->nextに代入するtoken
//Token_kind KIND_OF_Token_t , Token_t *CURRENT_TOKEN,char *STRING -> Token_t 
Token_t *new_token(Token_kind kind,Token_t *cur,char *str){


	Token_t *token = calloc(1,sizeof(Token_t));
	token ->kind = kind;
	token -> str = str;
	cur ->next = token;
	return token;
};




//tokenize funcion char * -> Token_t
Token_t *tokenize(char *p){//入力文字列


	Token_t head;
	head.next = NULL;
	Token_t *cur = &head;

	while(*p){
		if( isspace(*p) ){//空白の時はスキップ
			p++;
			continue;
		}else if( *p == '+' | *p == '-' ){
			cur = new_token(TK_OPERATOR,cur,p);
			p++;
			continue;
		}else if(isdigit(*p)){
			cur = new_token(TK_DIGIT,cur,p);
			cur -> val = strtol(p,&p,10);
			continue;
		}

		error("tokenizeできません。");
	}

	new_token(TK_EOF,cur,p);
	return head.next;
};




int main(int argc, char **argv){
	
	if( argc != 2 ){
		fprintf(stderr,"引数の個数が正しくありません\n");
		return 1;
	}
	
	Token_t *token = tokenize(argv[1]);//tokenize

//アセンブリ前半を出力
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");

//式の最初は数　そうなっているか判定しつつ,最初の mov 命令を出力
	printf("	mov rax, %d\n",number(&token));	

	//'+' or '-'を検知してアセンブリを出力
	while(token -> kind != TK_EOF ){
		if(find('+',&token)){
			printf("	add rax, %d\n",number(&token));
			continue;
		}
		find('-',&token);
		printf("	sub rax, %d\n",number(&token));
		
	}
	printf("	ret\n");
	return 0;
}
