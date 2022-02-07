#include "cc.h"

extern unsigned int String_len(char*);
extern int String_conpair(char* ,char* ,unsigned int);

Token_t *new_Token_t(Token_kind kind, Token_t* next, int val, int length, char* str, Type* tp) {
	Token_t* token = calloc(1, sizeof(Token_t));
	token -> kind = kind;
	token -> next = next;
	token -> val = val;
	token -> length = length;
	token -> str = str;
	token -> tp = tp;
	return token;
}

Token_t *new_token(Token_kind kind,Token_t *cur,char *str) {
	Token_t *token = new_Token_t(kind, NULL, 0, 0, str, NULL);
	cur ->next = token;
	return token;
}

unsigned int get_type_size(int kind) {
	switch(kind) {
		case TP_VOID: return 0;
		case TP_INT: return 4;
		case TP_CHAR: return 1;
		default:
			return 0;
	}	
}

Token_t *new_keyword(Token_kind kind, keyword kindOfKeyword, Token_t*cur, char *p) {
	cur = new_token( kind,cur,p);
	// keywordのlength を計算 TK_Typeの時はポインタ型の読み取りに使用する
	char *q = get_keyword(kindOfKeyword);
	unsigned int len = String_len(q);
	cur -> length = len;
	if(cur -> kind >= TOKEN_TYPE && cur -> kind != TK_STRUCT)
	{	
		cur -> tp = new_tp(cur -> kind - TOKEN_TYPE, NULL, get_type_size(cur -> kind - TOKEN_TYPE));
	}
	return cur;
}


int find(int kind,Token_t **token) {

	if(kind > MULTOPERATOR_START)
	{ // len >= 2 identifier
		char *multoper = get_symbol(kind);
		if((*token) -> length == String_len(multoper) && String_conpair(multoper,(*token) -> str,String_len(multoper)))
		{	
			consume(token);
			return 1;
		}
		return 0;
	}
	if((*token) -> str[0] != kind)
	{
		return 0;
	}
	else
	{
		consume(token);
		return 1;
	}
}

Token_t *consume(Token_t **token) {
	Token_t *tok = *token;
	*token = (*token)-> next;
	return tok;
}

int is_functioncall(Token_t **token) {
    Token_t * buf = (*token);

    if(buf -> kind > 299)
        consume(&buf);

    expect_ident(&buf);

    if(find('[',&buf))
    {
        while(find(']', &buf))
		{
			consume(&buf);
		}
    }
    if(find('(',&buf))
        return 1;
        
    return 0;
}
