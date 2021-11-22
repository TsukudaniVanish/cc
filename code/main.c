#include "cc.h"
#include<stdlib.h>
#include<ctype.h>
#include<stdarg.h>
#include<stdio.h>
#include<string.h>


//グローバル変数 エラー出力用
char *user_input;




//user_input を読み込んでエラー個所を示す関数
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




//token.kindが'op'か判定してtoken = token.next
// char OPERATOR,Token_t TOKEN -> bool
bool find(char *operator,Token_t **token){

	if( (*token) -> kind != TK_OPERATOR | strlen(operator) != (*token) -> length | memcmp( (*token) -> str,operator,(*token)-> length ) != 0  ){


		return false;

	}else{


		*token = (*token) -> next;
		return true;
	}
};




/*
 *expect function
 */

void expect(char *string ,Token_t **token){


	if( (*token) -> kind != TK_OPERATOR | (*token) -> length != strlen(string) | memcmp( (*token) -> str,string,(*token) -> length ) ){
		
		error_at( (*token) -> str ,"不正な文字");
	
	}else{
	
		(*token) = (*token) -> next;
		
	}
};

char expect_ident(Token_t **token){


	if( (*token)->kind != TK_IDENT ){


		error_at( (*token)-> str,"無効な変数名" );
	
	}else{


		char name = (*token)-> str[0];
		(*token) = (*token) -> next;
		return name;
	}
}

int expect_num(Token_t **token){

	
	if( (*token) -> kind != TK_DIGIT  ){


		error_at( (*token) -> str ,"数ではありません");
	
	}else{
		
		int v = (*token) -> val;	
		(*token) = (*token) -> next;
		return v;
	}
};

bool at_eof(Token_t **token){


	if( (*token)-> kind != TK_EOF ){


		return false;
	
	}else{


		return true;
	}
}




//新しいtokenを作り　cur->nextに代入するtoken
//Token_kind KIND_OF_Token_t , Token_t *CURRENT_TOKEN,char *STRING -> Token_t 
Token_t *new_token(Token_kind kind,Token_t *cur,char *str){


	Token_t *token = calloc(1,sizeof(Token_t));
	token ->kind = kind;
	token -> str = str;
	cur ->next = token;
	return token;
};

/*
 * tokenize function
 * 演算子:
 * 		算術演算子:
 * 				+,-,*,/
 * 		比較演算子:
 * 				==,!=,<=,>=,<,>
 * 		単項演算子:
 * 				+,-
 *
 * 	演算子は長さの順にtokenizeすること
 *
 */

/*
 * tokenize funcion 
 */

//char * -> Token_t
Token_t *tokenize(char *p){//入力文字列


	Token_t head;
	head.next = NULL;
	Token_t *cur = &head;

	while(*p){


		if( isspace(*p) ){//空白の時はスキップ


			p++;
			continue;

		}else if( strncmp(p,"==",2) == 0  | strncmp(p,"!=",2) == 0 | strncmp(p,"<=",2) == 0 | strncmp(p,">=",2) == 0 ){ // 2文字の演算子をtokenize

			
			cur = new_token(TK_OPERATOR,cur,p);
			cur -> length =2;
			p+=2;
			continue;

		}else if( *p == '+' | *p == '-' | *p == '*' | *p == '/' | *p == '(' | *p == ')'| *p == '<' | *p == '>'  ){//単項の演算子をtokenize

			
			cur = new_token(TK_OPERATOR,cur,p);
			cur -> length =1;
			p++;
			continue;

		}else if(isdigit(*p)){


			cur = new_token(TK_DIGIT,cur,p);
			cur -> val = strtol(p,&p,10);
			continue;
		
		}else if('a' <= *p && *p <= 'z'){
			

			cur = new_token(TK_IDENT,cur,p);
			p++;
			cur -> length = 1;
			continue;
		}

		error_at(cur -> str,"tokenizeできません。");
	}

	new_token(TK_EOF,cur,p);
	return head.next;
};

int main(int argc, char **argv){
	
	if( argc != 2 ){


		fprintf(stderr,"引数の個数が正しくありません\n");
		return 1;
	}

//グローバル変数に代入　エラー出力用
	user_input = argv[1];

	
	Token_t *token = tokenize(argv[1]);//tokenize


	//token を抽象構文木に変換
	Node_t *node = expr(&token);

	

//アセンブリ前半を出力
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	printf("main:\n");
	

	//抽象構文木を降りてコード生成
	//スタックトップには式の結果が入っている
	generate(node);


	//スタックトップとりだして返す
	printf("	pop rax\n");
	printf("	ret\n");
	return 0;
}
