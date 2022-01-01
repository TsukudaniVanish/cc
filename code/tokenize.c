#include "cc.h"
//#include<string.h>
//#include<stdbool.h>
#include<ctype.h>



int sizeof_token(int kind){

	switch (kind)
	{
	case TK_TypeVOID:
		return 0;
	case TK_TypeINT:
		
		return 4;
	
	case TK_TypeCHAR:

		return 1;
	}
	return 0;
}




int is_alnum(char c){

	
	return ('a' <= c && c <= 'z' ) ||
		   ('A' <= c && c <= 'Z' ) ||
		   ('0' <= c && c <= '9' ) ||
		   (c == '_');
}

int is_space(char p)
{
	if(' ' == p || '\t' == p || '\n' == p || '\r' == p || '\f' == p)
	{
		return 1;
	}
	return 0;
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


int is_comment(char *p)
{
	if(*p == '/')
	{
		return strncmp(p,"/*",2) ? 0 : 1;
	}
	return 0;
}




void comment_skip(char **p)
{
	while (1)
	{
		if( **p == '*' && !strncmp(*p,"*/",2))
		{
			*p += 2;
			return;
		}
		*p += 1;
	}
	
}



Token_kind is_keyword(char *p){

	
	char *assign[] = { "return","sizeof","while","else","for","if",
						"void","int","char",
						NULL};
	Token_kind assign_kind[] = {TK_RETURN,TK_SIZEOF,TK_WHILE,TK_ELSE,TK_FOR,TK_IF,
								TK_TypeVOID,TK_TypeINT,TK_TypeCHAR,
								TK_EOF};

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
			cur = new_token(TK_STRINGLITERAL,cur,p+1);
			cur -> length = q-p;
			p = q+1;
			if(*p != '"')
			{
				error_at(p,"文字リテラルが閉じていません\n");
			}
			cur = new_token(TK_PUNCTUATOR,cur,p);
			cur -> length = 1;
			p++;
			continue;	
		}
		else if(is_comment(p))
		{//コメント
			comment_skip(&p);
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
		
		}else if(*p != ';')
		{//識別子

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
		
		}else if(*p == ';')
		{
		
			cur = new_token(TK_PUNCTUATOR,cur,p++);
			cur -> length =1;
			continue;
		}
		error_at(cur -> str,"tokenizeできません。");
	}
	new_token(TK_EOF,cur,p);
	return head.next;
};

