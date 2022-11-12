#include "cc.h"

extern unsigned int String_len(char*);
extern int String_compare(char* ,char* ,unsigned int);
extern void Memory_copy(void* dest,void* source, unsigned int size);

extern void exit(int);
static void* NULL = (void*) 0;
extern void* calloc(unsigned nmem, unsigned size);

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
		case TP_VOID: return SIZEOF_VOID;
		case TP_INT: return SIZEOF_INT;
		case TP_LONG: return SIZEOF_LONG;
		case TP_CHAR: return SIZEOF_CHAR;
		default:
			return SIZEOF_VOID;
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
		if((*token) -> length == String_len(multoper) && String_compare(multoper,(*token) -> str,String_len(multoper)))
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

	if (buf -> kind == TK_STATIC || buf -> kind == TK_EXTERN)
		consume(&buf);
    if(buf -> kind >= TOKEN_TYPE || buf -> kind == TK_IDENT)
        consume(&buf);
	while(buf -> str[0] == '*') {
		consume(&buf);
	}
	if(buf -> kind == TK_IDENT)
		consume(&buf);

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

// this function does not consume token.
char* get_ident_name(Token_t** token) {
	if((*token) -> kind != TK_IDENT)
	{
		return NULL;
	}
	int len = (*token) -> length;
	char* name = calloc(len, sizeof(char));
	Memory_copy(name, (*token) -> str, len);
	return name;
}

/**
 * @brief check if identifier token is in ordinary name space. this function does not comsume token.
 * 
 * @param token 
 * @return int 
 */
int is_type_alias(Token_t** token) {
	if((*token) -> kind != TK_IDENT)
	{
		return 0;
	}

	char* name = get_ident_name(token);
	NameData* data = search_from_ordinary_namespace(name, ScopeController_get_current_scope(controller));

	if(data == NULL || data -> tag != TAG_TYPEDEF)
	{
		return 0;
	}
	return 1;
}

// this function does not consume token
/**
 * @brief check '(' and 'type_specifier' in token
 * 
 * @param token 
 * @return int 
 */
int is_cast(Token_t** token) {
	Token_t* buf = *token;

	if(!find('(', &buf)) {
		return 0;
	}

	if(buf -> kind >= TK_TYPESTART && buf -> kind <= TK_DECLARATION_SPECIFIER_END) {
		return 1;
	}
	if(is_type_alias(&buf)) {
		return 1;
	}
	return 0;

}

Token_t* Token_copy(Token_t* token) {
	return new_Token_t(token -> kind, NULL, token -> val, token -> length, token -> str, token -> tp);
}

Token_t* Token_copy_all(Token_t* token, Token_t* until) {
	Token_t* buf = new_Token_t(token -> kind, NULL, token -> val, token -> length, token -> str, token -> tp);
	Token_t* toReturn = buf;
	Token_t *cur = NULL;
	token = token -> next;
	while(token -> kind != TK_EOF && token != until)
	{
		cur = new_Token_t(token -> kind, NULL, token -> val, token -> length, token -> str, token -> tp);
		buf -> next = cur;
		token = token -> next;
		buf = cur;
	}
	cur = new_Token_t(TK_EOF, NULL, 0, 0, "\0", NULL);
	buf -> next = cur;
	return toReturn;
}

// insert chain to tokens
void Token_splice(Token_t* insert, Token_t* pre, Token_t* next) {
	pre -> next = insert;
	while (insert -> next -> kind != TK_EOF)
	{
		insert = insert -> next;
	}
	insert -> next = next;
	return;
}

Token_t* Token_tailHead(Token_t* newToken, Token_t* old) {
	old -> next = newToken;
	return Token_consume_to_last(newToken);
}

Token_t* Token_consume_to_last(Token_t* token) {
	while (token -> next && token -> next -> kind != TK_EOF)
	{
		token = token ->next;
	}
	return token;
}

// consume token until token -> next == until
Token_t* consume_to(Token_t* token, Token_t* until) {
	if(token == NULL) {
		return token;
	}
	while (token -> next != NULL && token -> next != until)
	{
		consume(&token);
	}
	return token;
}
// this function does not consume any token.
int check_ident_appear_until_punctuator(Token_t** token) {
	Token_t* buf = *token;
	while (buf -> kind != TK_PUNCTUATOR || buf -> kind == TK_EOF)
	{
		if(buf -> kind == TK_IDENT) {
			return 1;
		}
		consume(&buf);
	}
	return 0;
}

void expect(int kind ,Token_t **token) {
	if( (*token) -> kind > 100 |  kind != (*token) -> str[0])
	{
		error_at( (*token) -> str ,"expect %c but got %c", kind, (*token) -> str[0]);
	}
	else
	{
		(*token) = (*token) -> next;
	}
}

char *expect_ident(Token_t **token) {
	if((*token)->kind != TK_IDENT)
	{
		error_at( (*token)-> str, "invalid identifier name" );
	}
	else
	{
		char *name = calloc((*token) -> length, sizeof(char));
		Memory_copy(name,(*token) -> str, (*token) -> length);
		consume(token);
		return name;
	}
}

int expect_num(Token_t **token) {
	if((*token) -> kind != TK_CONST)
	{
		error_at( (*token) -> str , "number literal is expected");
	}
	else
	{	
		int v = (*token) -> val;	
		(*token) = (*token) -> next;
		return v;
	}
}