#include "cc.h"
//#include<stdio.h>
#include<stdarg.h>
//#include<stdbool.h>
//#include<string.h>



//変数を名前で検索する
Lvar *find_lvar(Token_t **token,Lvar **locals){
	

	for(Lvar *var = *locals; var;var = var -> next){


		if( var -> length == (*token)-> length && !memcmp( (*token)-> str, var ->name,var -> length )  ){


			return var; 
		}
	}
	return NULL;
}


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

};




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
};

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
};










int main(int argc, char **argv){
	
	if( argc != 2 ){


		fprintf(stderr,"引数の個数が正しくありません\n");
		return 1;
	}

	//グローバル変数に代入　エラー出力用
	user_input = argv[1];
	
	

	// ';'で区切った文
	Node_t *code[100];
	
	//入力をトークン列に変換
	Token_t *token = tokenize(argv[1]);


	//token を抽象構文木に変換
	program(&token,code);

	

//アセンブリ前半を出力
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	


	//先頭の式からコード生成
	//抽象構文木を降りてコード生成
	//スタックトップには式の結果が入っている
	for(int i =0;code[i];i++){
		generate(code[i]);

	}

	return 0;
}
