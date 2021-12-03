#include "cc.h"
//#include<stdio.h>
#include<stdarg.h>
//#include<stdbool.h>
//#include<string.h>



Lvar *find_lvar(Token_t **token,Lvar **locals){
	

	for(Lvar *var = *locals; var;var = var -> next){


		if( var -> length == (*token)-> length && !memcmp( (*token)-> str, var ->name,var -> length )  ){


			return var; 
		}
	}
	return NULL;
}


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





bool find(char *string,Token_t **token){

	if( (*token) -> kind > 100 | strlen(string) != (*token) -> length | memcmp( (*token) -> str,string,(*token)-> length ) != 0  ){


		return false;

	}else{


		*token = (*token) -> next;
		return true;
	}
}




void expect(char *string ,Token_t **token){


	if( (*token) -> kind > 100 | (*token) -> length != strlen(string) | memcmp( (*token) -> str,string,(*token) -> length ) ){
		
		error_at( (*token) -> str ,"不正な文字");
	
	}else{
	
		(*token) = (*token) -> next;
		
	}
}

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

	
	if( (*token) -> kind != TK_CONST  ){


		error_at( (*token) -> str ,"数ではありません");
	
	}else{
		
		int v = (*token) -> val;	
		(*token) = (*token) -> next;
		return v;
	}
}

bool at_eof(Token_t **token){


	if( (*token)-> kind != TK_EOF ){


		return false;
	
	}else{


		return true;
	}
}




int main(int argc, char **argv){
	
	if( argc != 2 ){


		fprintf(stderr,"引数の個数が正しくありません\n");
		return 1;
	}

	//グローバル変数に代入　エラー出力用
	user_input = argv[1];
	
	

	// fuction ごとのコード
	Node_t *code[100];
	
	//入力をトークン列に変換
	Token_t *token = tokenize(argv[1]);


	//token を抽象構文木に変換
	program(&token,code);

	//関数スコープ識別子テーブルを先頭にセット
	nametable = nametable -> head;

	

//アセンブリ前半を出力
	printf(".intel_syntax noprefix\n");
	printf(".global main\n");
	


	//先頭の式からコード生成
	//抽象構文木を降りてコード生成
	//スタックトップには式の結果が入っている
	for(int i =0;code[i];i++){
		generate(code[i]);
		if(nametable -> next)
			nametable = nametable -> next;

	}

	return 0;
}
