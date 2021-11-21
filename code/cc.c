#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

//グローバル変数 エラー出力用
//
char *user_input;


/*
 * 生成文法
 * expr = mul( "+"mul | "-"mul)*
 * mul = primary ("*" primary | "/" primary )*
 * primary = num | "(" expr ")"
 *
 * */


typedef enum{
	
	ND_ADD, // <-> +
	ND_SUB, // <-> -
	ND_MUL, // <-> *
	ND_DIV, // <-> /
	ND_NUM, // <-> integer

}Node_kind;

typedef struct Node Node_t;

//抽象構文木のノード型

struct Node {
	
	Node_kind kind;
	Node_t *left;
	Node_t *right;
	int val;

};



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




// //エラーをはく関数
// void error(char *fmt,...){//printf と同じ引数をとる
// 
// 
// 	va_list arg;
// 	va_start(arg,fmt);
// 	vfprintf(stderr,fmt,arg);
// 	fprintf(stderr,"\n");
// 	va_end(arg);
// 	exit(1);
// 
// };
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


		error_at( (*token)-> str,"数ではありません");

	}else{


		int val = (*token) -> val;
		*token = (*token) -> next;
		return val;
	}
};




/*
 *expect function
 */

void expect(char character ,Token_t **token){


	if( (*token) -> kind != TK_OPERATOR | (*token) -> str[0] != character ){
		
		error_at( (*token) -> str ,"不正な文字");
		exit(1);
	
	}else{
	
		(*token) = (*token) -> next;
		
	}
};

int expect_num(Token_t **token){

	
	if( (*token) -> kind != TK_DIGIT  ){


		error_at( (*token) -> str ,"数ではありません");
		exit(1);
	
	}else{
		
		int v = (*token) -> val;	
		(*token) = (*token) -> next;
		return v;
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

		}else if( *p == '+' | *p == '-' | *p == '*' | *p == '/' | *p == '(' | *p == ')'  ){

			cur = new_token(TK_OPERATOR,cur,p);
			p++;
			continue;

		}else if(isdigit(*p)){


			cur = new_token(TK_DIGIT,cur,p);
			cur -> val = strtol(p,&p,10);
			continue;
		}

		error_at(cur -> str,"tokenizeできません。");
	}

	new_token(TK_EOF,cur,p);
	return head.next;
};



//Node_t を作る関数
Node_t *new_node( Node_kind kind,Node_t *l,Node_t *r){


	Node_t *node=calloc(1,sizeof( Node_t ));
	node ->kind = kind;
	node -> left = l;
	node -> right =r;
	return node;
}

Node_t *new_node_num(int val){


	Node_t *node=calloc(1,sizeof( Node_t ));
	node -> kind = ND_NUM;
	node -> val = val;
	return node;
}

/*
 * token から構文木を生成 
 */

Node_t *expr(Token_t **);
Node_t *mul(Token_t **);
Node_t *primary(Token_t **);

/*
 * 関数実装
 */

Node_t *expr(Token_t **token){


	Node_t *node = mul(token);

	for(;;){

		
		if( find('+',token) ){


			node = new_node(ND_ADD,node,mul(token));

		}else if( find('-',token) ){


			node = new_node(ND_SUB,node,mul(token));

		}else{


			return node;
		}
	}
}




Node_t *mul(Token_t **token){



	Node_t *node = primary(token);

	for(;;){


		if( find('*',token) ){


			node = new_node(ND_MUL,node,primary(token));

		}else if( find('/',token) ){


			node = new_node(ND_DIV,node,primary(token));

		}else{


			return node;
		}
	}
}




Node_t *primary(Token_t **token){

	
	if( find('(',token) ){ // '(' の次は expr


		Node_t *node = expr(token);
		expect(')',token);// ')'かcheck
		return node;
	}

	return new_node_num( expect_num(token) );
}




//抽象構文木からアセンブリコードを生成する
void generate(Node_t *node){


	if(node -> kind == ND_NUM){


		printf("	push %d\n",node -> val);
	}else{

		generate(node -> left);
		generate(node -> right);
		
		
		printf("	pop rdi\n");
		printf("	pop rax\n");

		switch (node -> kind){
		case ND_ADD:
			
			printf("	add rax, rdi\n");
			break;

		case ND_SUB:
			
			printf("		sub rax, rdi\n");
			break;
		
		case ND_MUL:
			
			printf("	imul rax, rdi\n");
			break;
		
		case ND_DIV:

			printf("	cqo\n");
			printf("	idiv rdi\n");
			break;
		}
		
		printf("	push rax\n");
	}
}





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
