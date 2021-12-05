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




int is_ope_or_pun(char *p){

	
	char *tokens[] ={"==","!=","<=",">=","<",">","+","-","*","/","=","&",";","(",")","{","}","[","]",",",NULL};

	for(char **str = tokens ; *str ; str++ ){
		
		int len = strlen(*str);

		if( !strncmp(p,*str,len) ){

			if(str - tokens < 12)
				return len;
			return len + 1000;
		}
	}
	return false;
}



Token_kind is_keyword(char *p){

	
	char *assign[] = { "return","sizeof","while","else","for","if","int","char",NULL};
	Token_kind assign_kind[] = {TK_RETURN,TK_SIZEOF,TK_WHILE,TK_ELSE,TK_FOR,TK_IF,TK_TypeINT,TK_TypeCHAR,TK_EOF};

	Token_kind *v = assign_kind;

	for( char **q = assign; *q ; q++ ){


		if(!( strncmp(p,*q,strlen(*q)) || is_alnum(p[strlen(*q)]) ) ){


			return *v;
		}
		if(*v != TK_EOF)
			v++;
	}
	return *v;
}

Token_t *new_keyword(Token_kind kind,Token_t*cur,char *p){

	cur = new_token( kind,cur,p);
	// keywordのlength を計算 TK_Typeの時はポインタ型の読み取りに使用する
	char *q = p;
	while (!(is_ope_or_pun(q) || isspace(*q))){
		q++;
	}
	
	(cur) -> length = q -p;
	
	if(kind > 299){//ポインタ型か判定


		(cur) -> tp = calloc(1,sizeof(Type));
		(cur) -> tp -> Type_label = kind - 300;

		while (isspace(*q) || *q=='*'){
			
			if(isspace(*q)){
				
				q++;
				(cur) -> length++;
				continue;
			}
			(cur) -> length++;
			Type *pointerto = calloc(1,sizeof(Type));
			pointerto -> Type_label = TP_POINTER;
			pointerto -> pointer_to = (cur) ->tp;
			(cur) -> tp = pointerto;
			q++;
		}//qには識別子の名前があるはず
	
		if(kind > 299  && is_ope_or_pun(q)  ){//識別子があるか判定


			fprintf(stderr,"識別子が必要です。\n");
			exit(1);
		}
	}
	return cur;
}







Token_t *new_token(Token_kind kind,Token_t *cur,char *str){


	Token_t *token = calloc(1,sizeof(Token_t));
	token ->kind = kind;
	token -> str = str;
	cur ->next = token;
	return token;
};



Token_t *tokenize(char *p){//入力文字列


	Token_t head;
	head.next = NULL;
	Token_t *cur = &head;

	while(*p){


		if( isspace(*p)  ){//空白 の時はスキップ


			p++;
			continue;

		}else if ( is_keyword(p) !=TK_EOF ){
			
			cur = new_keyword(is_keyword(p),cur,p);
			p+= cur -> length;
			continue;

		}else if(is_ope_or_pun(p)){
			
			cur = new_token(TK_OPERATOR,cur,p);
			cur -> length =is_ope_or_pun(p);
			if(cur -> length > 1000){
				cur -> kind = TK_PUNCTUATOR;
				cur -> length -= 1000;
			}
			p += cur -> length;
			continue;

		}else if(isdigit(*p)){


			cur = new_token(TK_CONST,cur,p);
			cur -> val = strtol(p,&p,10);
			continue;
		
		}else if(*p != ';'){
			

			cur = new_token(TK_IDENT,cur,p);
			//length 取得
			//空白か次の演算子まで名前だと思う
			char *q = p;
			
			while(1){
			
				if( isspace(*q) || q[0] == ','  || is_ope_or_pun(q)){ 
				//q が演算子をさしたらやめる

					cur -> length = q-p;
					p =q;
					break;
				}
				q++;
			}
			continue;
		
		}else if(*p == ';'){
		

			cur = new_token(TK_PUNCTUATOR,cur,p++);
			cur -> length =1;
			continue;
		}
		error_at(cur -> str,"tokenizeできません。");
	}
	new_token(TK_EOF,cur,p);
	return head.next;
};

