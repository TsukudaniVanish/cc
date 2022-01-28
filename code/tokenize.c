#include "cc.h"
//#include<string.h>
//#include<stdbool.h>
#include<ctype.h>

extern unsigned int String_len(char*);
extern int String_conpair(char* ,char*, unsigned int);
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
	while (' ' == *p || '\t' == *p || '\n' == *p || '\r' == *p || '\f' == *p || '\v' == *p)
	{
		p++;
	}
	return p;	
}

int is_symbol(char *p) {
	char* operator;
	int len = 0;
	Symbols kind = MULTOPERATOR_START;
	while(kind < END_OF_SYMBOLS || (kind >= MULTOPERATOR_START && kind <= END_OF_MULTI_OPERATOR))
	{
		switch(kind) {
			case MULTOPERATOR_START:
				kind++;
				break;
			case END_OF_MULTI_OPERATOR:
				kind = UNIT_SYMBOL_START + 1;
				break;
			case END_OF_UNIT_OPERATOR:
				kind = PUNCTUATOR_START;
				break;
			case PUNCTUATOR_START:
				kind++;
				break;
			default:
				operator = get_symbol(kind);
				len = String_len(operator);
				if(String_conpair(operator, p, len))
				{
					if(kind > PUNCTUATOR_START && kind < END_OF_SYMBOLS)
						len += 1000;
					return len;
				}
					kind ++;
		}
	}
	return 0;
}


int is_comment(char *p)
{
	if(*p == '/')
	{
		return String_conpair(p,"/*",2);
	}
	return 0;
}




void comment_skip(char **p)
{
	while (1)
	{
		if( **p == '*' && String_conpair(*p,"*/",2))
		{
			*p += 2;
			return;
		}
		*p += 1;
	}
	
}

char* get_keyword(keyword kind) {
	
	switch(kind)
	{
		case RETURN:
			return "return";
		case SIZEOF:
			return "sizeof";
		case WHILE:
			return "while";
		case ELSE:
			return "else";
		case FOR:
			return "for";
		case IF:
			return "if";
		case VOID:
			return "void";
		case CHAR:
			return "char";
		case INT:
			return "int";
		case UNSIGNED:
			return "unsigned";
		case UNSIGNED_INT:
			return "unsigned int";
		default:
			return NULL;
	}
}

Token_kind get_correspond_token_kind(keyword kind) {
	switch(kind) 
	{
		case RETURN: return TK_RETURN;
		case SIZEOF: return TK_SIZEOF;
		case WHILE: return TK_WHILE;
		case ELSE: return TK_ELSE;
		case FOR: return TK_FOR;
		case IF: return TK_IF;
		case VOID: return TK_TypeVOID;
		case UNSIGNED:
		case UNSIGNED_INT:
		case INT: return TK_TypeINT;
		case CHAR: return TK_TypeCHAR;
		default:
			fprintf(stderr, "	failed to get token kind from keyword\n");
			exit(1);
	}
}


int is_keyword(char *p, keyword* kind_of){

	keyword kind = KEYWORD_START + 1;
	while(kind < KEYWORD_END)
	{
		char* keyword = get_keyword(kind);
		if(keyword == NULL)
		{
			kind ++;
			continue;
		}
		unsigned int len = String_len(keyword);
		if(String_conpair(keyword, p, len))
		{
			if(kind_of != NULL)
				*kind_of = kind;
			return 1;
		}
		kind ++;
	}
	if(kind_of != NULL)
		*kind_of = KEYWORD_END;
	return 0;
}




Token_t *tokenize(char *p){//入力文字列


	Token_t head;
	head.next = NULL;
	Token_t *cur = &head;
	while(*p != '\0'){
		keyword keyword = KEYWORD_START;

		p = skip(p);
		if(*p == '\0') break;
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
		else if (is_keyword(p, &keyword))
		{//キーワード
			Token_kind kind = get_correspond_token_kind(keyword);
			cur = new_keyword(kind, keyword,cur, p);
			p += cur -> length;
			continue;

		}else if(is_symbol(p))
		{//演算子または区切り文字
			
			cur = new_token(TK_OPERATOR,cur,p);
			cur -> length = is_symbol(p);
			if(cur -> length > 1000)
			{// punctuator or not
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
			
				if( isspace(*q) || q[0] == ','  || is_symbol(q)){ 
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

