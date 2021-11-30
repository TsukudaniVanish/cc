#include "cc.h"
//#include<string.h>
//#include<stdbool.h>
#include<ctype.h>




bool is_alnum(char c){

	
	return ('a' <= c && c <= 'z' ) ||
		   ('A' <= c && c <= 'Z' ) ||
		   ('0' <= c && c <= '9' ) ||
		   (c == '_');
}




bool isoperator(char *p){

	
	char *operators[] ={"==","!=","<=",">=","<",">","+","-","*","/","=",";","(",")","{","}","&",NULL};

	for(char **str = operators ; *str ; str++ ){
		
		int len = strlen(*str);

		if( !strncmp(p,*str,len) ){


			return true;
		}
	}
	return false;
}



//key word or 型名と一致するか判定する

bool is_assign(char *p,Token_t **cur){

	
	char *assign[] = { "return","while","else","for","if","int",NULL};
	Token_kind assign_kind[] = {TK_RETURN,TK_WHILE,TK_ELSE,TK_FOR,TK_IF,TK_Type};

	Token_kind *v = assign_kind;

	for( char **q = assign; *q ; q++ ){


		int len = strlen(*q);

		if(!( strncmp(p,*q,len) || is_alnum(p[len]) ) ){


			*cur = new_token( *v,*cur,p);
			(*cur) -> length = len;
			
			if(*v == TK_Type){//ポインタ型か判定


				(*cur) -> tp = calloc(1,sizeof(Type));
				(*cur) -> tp -> Type_label = TP_INT;
				char *r = p + len;

				while ( isspace(*r) || *r=='*' )
				{
					
					if(isspace(*r)){
						
						r++;
						(*cur) -> length++;
						continue;
					}
					(*cur) -> length++;
					Type *pointerto = calloc(1,sizeof(Type));
					pointerto -> Type_label = TP_POINTER;
					pointerto -> pointer_to = (*cur) ->tp;
					(*cur) -> tp = pointerto;
					r++;
				}//rには識別子の名前があるはず
			
				if( *v == TK_Type  && isoperator(p+( (*cur) -> length) ) ){//識別子があるか判定


					fprintf(stderr,"識別子が必要です。\n");
					exit(1);
				}
			}
			return true;
		}
		if(*v != TK_Type)
			v++;
	}
	return false;	
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
 * 				+,-,=
 *	変数(または関数)名
 * 	演算子は長さの順にtokenizeすること
 * 	演算子か判定後にlocal variable として読み込むこと
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


		if( isspace(*p) || *p == ',' ){//空白 ',' の時はスキップ


			p++;
			continue;

		}else if ( is_assign(p,&cur) ){
			

			p+= cur -> length;
			continue;

		}else if( strncmp(p,"==",2) == 0  | strncmp(p,"!=",2) == 0 | strncmp(p,"<=",2) == 0 | strncmp(p,">=",2) == 0 ){ 
			// 2文字の演算子をtokenize

			
			cur = new_token(TK_OPERATOR,cur,p);
			cur -> length =2;
			p+=2;
			continue;

		}else if( *p == '+' | *p == '-' | *p == '*' | *p == '/' | *p == '(' | *p == ')'| *p == '<' | *p == '>' | *p == '=' | *p =='{'  | *p == '}' | *p == '&'  ){//単項の演算子をtokenize

			
			cur = new_token(TK_OPERATOR,cur,p);
			cur -> length =1;
			p++;
			continue;

		}else if(isdigit(*p)){


			cur = new_token(TK_DIGIT,cur,p);
			cur -> val = strtol(p,&p,10);
			continue;
		
		}else if(*p != ';'){
			

			cur = new_token(TK_IDENT,cur,p);
			//length 取得
			//空白か次の演算子まで名前だと思う
			char *q = p;
			
			while(1){
			
				if( isspace(*q) || q[0] == ','  || isoperator(q)){ 
				//q が演算子をさしたらやめる

					cur -> length = q-p;
					p =q;
					break;
				}
				q++;
			}
			continue;
		
		}else if(*p == ';'){
		

			cur = new_token(TK_OPERATOR,cur,p++);
			cur -> length =1;
			continue;
		}
		error_at(cur -> str,"tokenizeできません。");
	}
	new_token(TK_EOF,cur,p);
	return head.next;
};

