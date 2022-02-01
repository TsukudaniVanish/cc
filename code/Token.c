#include "cc.h"

extern unsigned int String_len(char*);
extern int String_conpair(char* ,char* ,unsigned int);
extern void Memory_copy(void*, void*, unsigned int);
Token_t *new_token(Token_kind kind,Token_t *cur,char *str) {
	Token_t *token = calloc(1,sizeof(Token_t));
	token ->kind = kind;
	token -> str = str;
	cur ->next = token;
	return token;
}

long get_type_size(int kind) {
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

char *get_symbol(int kind) {
	switch(kind)
	{
	case EQUAL: return "==";
	case NEQ: return "!=";
	case LEQ: return "<=";
	case GEQ: return	">=";
	case INC: return "++";
	case DEC: return "--";
	case LOG_AND: return "&&";
	case LOG_OR: return "||";
	case LE: return "<";
	case GE: return ">";
	case AND: return "&";
	case ASSIGN: return "=";
	case PLUS: return "+";
	case MINUS: return "-";
	case STER: return "*";
	case BACK_SLASH: return "/";
	case COMMA: return ",";
	case SEMICORRON: return ";";
	case PARENTHESIS: return "(";
	case PARANTHESIS_CLOSE: return ")";
	case BRACE: return "{";
	case BRACE_CLOSE: return "}";
	case BRACKET: return "[";
	case BRACKET_CLOSE: return "]";
	default:
		fprintf(stderr,"invalid kind of operator : %d",kind);
		exit(1);
	}
}

int find(int kind,Token_t **token) {

	if(kind > MULTOPERATOR_START){ // len >= 2 identifier
		char *multoper = get_symbol(kind);
		if((*token) -> length == String_len(multoper) && String_conpair(multoper,(*token) -> str,String_len(multoper)))
		{	
			consume(token);
			return 1;
		}
		return 0;
	}

	if((*token) -> str[0] != kind){

		return 0;

	}else{


		consume(token);
		return 1;
	}
}




void expect(int kind ,Token_t **token) {


	if( (*token) -> kind > 100 |  kind != (*token) -> str[0]){
		
		error_at( (*token) -> str ,"不正な文字");
	
	}else{
	
		(*token) = (*token) -> next;
		
	}
}

char *expect_ident(Token_t **token) {
	if( (*token)->kind != TK_IDENT )
	{
		error_at( (*token)-> str,"無効な変数名" );
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
	if( (*token) -> kind != TK_CONST  )
	{
		error_at( (*token) -> str ,"数ではありません");
	}
	else
	{	
		int v = (*token) -> val;	
		(*token) = (*token) -> next;
		return v;
	}
}

int at_eof(Token_t **token) {
	if((*token)-> kind != TK_EOF)
	{
		return 0;
	}
	else
	{
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
