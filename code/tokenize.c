#include "cc.h"
//#include<string.h>
//#include<stdbool.h>
#include<ctype.h>



int sizeof_token(int kind){

	switch (kind)
	{
	case TK_TypeINT:
		
		return 4;
	
	case TK_TypeCHAR:

		return 1;
	}
	return 0;
}




bool is_alnum(char c){

	
	return ('a' <= c && c <= 'z' ) ||
		   ('A' <= c && c <= 'Z' ) ||
		   ('0' <= c && c <= '9' ) ||
		   (c == '_');
}


char *skip(char * p)
{
	while (' ' == *p || '\t' == *p || '\n' == *p || '\r' == *p || '\f' == *p)
	{
		p++;
	}
	return p;	
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


		(cur) -> tp = new_tp(kind -300,NULL,sizeof_token(kind));

		while (isspace(*q) || *q=='*'){
			
			if(isspace(*q)){
				
				q++;
				(cur) -> length++;
				continue;
			}
			(cur) -> length++;
			Type *pointerto = new_tp(TP_POINTER,(cur) ->tp,8);
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

	while(*p != '\0'){


		p = skip(p);
		if(*p == '"')
		{//文字列リテラル
			cur = new_token(TK_PUNCTUATOR,cur,p);
			cur -> length = 1;
			char *q = p;
			while (*(q+1) != '"')
			{
				q++;
			}
			cur = new_token(TK_STRINGITERAL,cur,p+1);
			cur -> length = q-p;
			p = q+1;
			if(*p != '"')
			{
				fprintf(stderr,"文字列イテラルが閉じていません");
				exit(1);
			}
			cur = new_token(TK_PUNCTUATOR,cur,p);
			cur -> length = 1;
			p++;
			continue;	
		}
		else if ( is_keyword(p) !=TK_EOF )
		{//キーワード
			
			cur = new_keyword(is_keyword(p),cur,p);
			p += cur -> length;
			continue;

		}else if(is_ope_or_pun(p))
		{//演算子または区切り文字
			
			cur = new_token(TK_OPERATOR,cur,p);
			cur -> length =is_ope_or_pun(p);
			if(cur -> length > 1000){
				cur -> kind = TK_PUNCTUATOR;
				cur -> length -= 1000;
			}
			p += cur -> length;
			continue;

		}else if(isdigit(*p))
		{//数字


			cur = new_token(TK_CONST,cur,p);
			cur -> val = strtol(p,&p,10);
			continue;
		
		}else if(*p != ';'){//識別子
			

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

