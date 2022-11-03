// TODO: tokenize #elif #else 
#include "cc.h"
//#include<string.h>
//#include<stdbool.h>
#include<ctype.h>

extern unsigned int String_len(char*);
extern int String_compare(char* ,char*, unsigned int);
extern void Memory_copy(void* dst, void* src, unsigned length);

int sizeof_token(int kind) {
	switch (kind)
	{
	case TK_TypeVOID:
		return 0;
	case TK_TypeINT:
		
		return SIZEOF_INT;
	
	case TK_TypeCHAR:

		return 1;
	}
	return 0;
}




int is_alnum(char c) {
	return ('a' <= c && c <= 'z' ) ||
		   ('A' <= c && c <= 'Z' ) ||
		   ('0' <= c && c <= '9' ) ||
		   (c == '_');
}

int is_space(char p) {
	if(' ' == p || '\t' == p || '\n' == p || '\r' == p || '\f' == p)
	{
		return 1;
	}
	return 0;
}

// This function assume input and word is not NULL.
int find_word(char* input, char* word) {
	if(*input != *word) {
		return 0;
	}
	for(int i = 0; word[i] != '\0'; i ++) {
		if(input[i] == '\0') {
			return 0;
		}
		if(input[i] != word[i]) {
			return 0;
		}
	}
	return 1;
}

// skip white space characters
char *skip(char * p) {
	while (' ' == *p || '\t' == *p || '\n' == *p || '\r' == *p || '\f' == *p || '\v' == *p)
	{
		p++;
	}
	return p;	
}

// skip white space characters except new line.
char* skip_in_macro(char* p) {
	while(' ' == *p || '\t' == *p || '\r' == *p || '\f' == *p || '\v' == *p)
	{
		p++;
	}
	return p;
}

/**
 * @brief get value of a control character and add *p to its length
 * 
 * @param p 
 * @return int 
 */
int get_control_character(char** pointer) {
	char* p = *pointer;
	if(*p != '\\') {
		return 0;
	}

	p++;
	int value = 0;
	switch (*p) {
		case '0': value = '\0';
			break;
		case 'n': value = '\n';
			break;
		case 't': value = '\t';
			break;
		case 'v': value = '\v';
			break;
		case 'r': value = '\r';
			break;
		case 'f': value = '\f';
			break;
		default:
			break;
	}
	*pointer = p;
	return value;
}

char *get_symbol(int kind) {
	switch(kind)
	{
	case THREE_DOTS: return "...";
	case EQUAL: return "==";
	case NEQ: return "!=";
	case LEQ: return "<=";
	case GEQ: return	">=";
	case INC: return "++";
	case DEC: return "--";
	case LOG_AND: return "&&";
	case LOG_OR: return "||";
	case ARROW: return "->";
	case LE: return "<";
	case GE: return ">";
	case AND: return "&";
	case ASSIGN: return "=";
	case PLUS: return "+";
	case MINUS: return "-";
	case STER: return "*";
	case DOT: return ".";
	case BACK_SLASH: return "/";
	case EXCLAMATION: return "!";
	case QUESTION: return "?";
	case COMMA: return ",";
	case SEMICOLON: return ";";
	case COLON: return ":";
	case PARENTHESIS: return "(";
	case PARENTHESIS_CLOSE: return ")";
	case BRACE: return "{";
	case BRACE_CLOSE: return "}";
	case BRACKET: return "[";
	case BRACKET_CLOSE: return "]";
	default:
		return NULL;
	}
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
				if(String_compare(operator, p, len))
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


int is_comment(char *p) {
	if(*p == '/')
	{
		return String_compare(p,"/*",2) || String_compare(p, "//",2);
	}
	return 0;
}


void comment_skip(char **p) {

	*p += 1;
	//line comment 
	if(**p == '/')
	{
		while(**p != '\n' && **p != '\0')
		{
			*p += 1;
		}
		return;
	}
	// block comment
	while (1)
	{
		if( **p == '*' && String_compare(*p,"*/",2))
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
		case CONTINUE: return "continue";
		case DEFINED: return "defined";
		case TYPEDEF: return "typedef";
		case RETURN: return "return";
		case SIZEOF: return "sizeof";
		case BREAK: return "break";
		case WHILE: return "while";
		case SWITCH: return "switch";
		case CASE: return "case";
		case DEFAULT: return "default";
		case ELSE: return "else";
		case FOR: return "for";
		case IF: return "if";
		case DO: return "do";
		case STATIC: return "static";
		case EXTERN: return "extern";
		case MACRO_DEFINE: return "#define";
		case MACRO_ENDIF: return "#endif";
		case MACRO_IF: return "#if";
		case MACRO_IFDEF: return "#ifdef";
		case MACRO_IFNDEF: return "#ifndef";
		case MACRO_INCLUDE: return "#include";
		case VOID: return "void";
		case CHAR: return "char";
		case INT: return "int";
		case UNSIGNED: return "unsigned";
		case UNSIGNED_INT: return "unsigned int";
		case LONG: return "long";
		case LONG_INT: return "long int";
		case STRUCT: return "struct";
		case UNION: return "union";
		case ENUM: return "enum";
		default:
			return NULL;
	}
}

Token_kind get_correspond_token_kind(keyword kind) {
	switch(kind) 
	{
		case CONTINUE: return TK_CONTINUE;
		case DEFINED: return TK_OPERATOR;
		case TYPEDEF: return TK_TYPEDEF;
		case RETURN: return TK_RETURN;
		case SIZEOF: return TK_SIZEOF;
		case BREAK: return TK_BREAK;
		case WHILE: return TK_WHILE;
		case SWITCH: return TK_SWITCH;
		case CASE: return TK_CASE;
		case DEFAULT: return TK_DEFAULT;
		case ELSE: return TK_ELSE;
		case FOR: return TK_FOR;
		case IF: return TK_IF;
		case DO: return TK_DO;
		case STATIC: return TK_STATIC;
		case EXTERN: return TK_EXTERN;
		case VOID: return TK_TypeVOID;
		case LONG:
		case LONG_INT: return TK_TypeLONG;
		case UNSIGNED:
		case UNSIGNED_INT:
		case INT: return TK_TypeINT;
		case CHAR: return TK_TypeCHAR;
		case STRUCT: return TK_STRUCT;
		case UNION: return TK_UNION;
		case ENUM: return TK_ENUM;
		default:
			fprintf(stderr, "	failed to get token kind from keyword\n");
			exit(1);
	}
}


int is_keyword(char *p) {
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
		if(String_compare(p, keyword, len) && (is_space(*(p + len)) || is_symbol(p+len) || (p + len)[0] == '\0'))
		{
			return kind;
		}
		kind ++;
	}
	return KEYWORD_START;
}

typedef enum {
	TOKENIZE_ERR,
	STRINGLITERAL_START,
	CHARACTER_LITERAL_START,
	COMMENT_START,
	MACRO_START,
	KEYWORD,
	SYMBOL,
	NUMBER,
	IDENTIFIER,
	END,
}TokenizeMarker;

TokenizeMarker tokenize_flow_control(char* p) {
	if(*p == '\0')
		return END;
	
	if(*p == '"')
		return STRINGLITERAL_START;
	
	if(*p == '\'')
		return CHARACTER_LITERAL_START;
	
	if(is_comment(p))
		return COMMENT_START;
	
	if(*p == '#')
		return MACRO_START;
	
	if(is_keyword(p))
		return KEYWORD;
	
	if(is_symbol(p))
		return SYMBOL;
	
	if(isdigit(*p))
		return NUMBER;

	if(is_alnum(*p))
		return IDENTIFIER;
	return TOKENIZE_ERR;
}

TokenizeMarker tokenize_macro_flow_control(char* p) {
	if(*p == '\n')
		return END;
	return tokenize_flow_control(p);
}

Token_t* tokenize_character_literal(char** p, Token_t* cur) {
	cur = new_token(TK_CONST, cur, *p);
	(*p)++;
	if(**p == '\\') {
		cur -> val = get_control_character(p);
	} else {
		cur -> val = **p;
	}
	(*p)++;
	if(**p != '\'') {
		error_at(*p, "fail to tokenize: %c", **p);
	}
	(*p)++;
	return cur;
}

Token_t* tokenize_string_literal(char** p, Token_t* cur) {
	cur = new_token(TK_PUNCTUATOR, cur, *p);
	cur -> length = 1;
	char *q = *p;
	while (q && *(q + 1) != '"')
	{
		q++;
	}
	cur = new_token(TK_STRINGLITERAL, cur, *p + 1);
	cur -> length = q - *p;
	*p = q + 1;
	if(**p != '"')
	{
		error_at(*p,"Tokenizer failed to find end marker of string literal\n");
	}
	cur = new_token(TK_PUNCTUATOR, cur, *p);
	cur -> length = 1;
	(*p)++;
	return cur;
}

Token_t* tokenize_keyword(char** p, Token_t* cur) {
	keyword keyWord = is_keyword(*p);
	Token_kind kind = get_correspond_token_kind(keyWord);
	cur = new_keyword(kind, keyWord, cur, *p);
	*p += cur -> length;
	return cur;
}

Token_t* tokenize_symbol(char** p, Token_t* cur) {
	cur = new_token(TK_OPERATOR, cur, *p);
	cur -> length = is_symbol(*p);
	if(cur -> length > 1000)
	{// punctuator or not
		cur -> kind = TK_PUNCTUATOR;
		cur -> length -= 1000;
	}
	if(cur -> length == 3) {
		// place holder ...
		cur -> kind = TK_PLACE_HOLDER;
	}
	*p += cur -> length;
	return cur;
}

Token_t* tokenize_number(char** p, Token_t* cur) {
	cur = new_token(TK_CONST, cur, *p);
	cur -> val = strtol(*p, p, 10);
	return cur;
}

Token_t* tokenize_identifier(char** p, Token_t* cur) {
	cur = new_token(TK_IDENT, cur, *p);
	//calculate length of identifier
	char *q = *p;
	while(1){
		if( isspace(*q) || q[0] == '\0' || q[0] == ','  || is_symbol(q))
		{ //stop
			cur -> length = q - *p;
			*p = q;
			break;
		}
		q++;
	}
	return cur;
}

/**
 * @brief tokenize until keyword appears.
 * After this function called, The pointer points a pointer pointing a character just before keyword.
 * This function does not add TK_EOF
 * 
 * @param pointer 
 * @param until keyword
 * @return Token_t* 
 */
Token_t* tokenize_until(char** pointer, char* until) {
	char* p = *pointer;
	Token_t head;
	head.next = NULL;
	Token_t* cur = &head;

	// To ensure p[0:len(until)] is not until, we skip p first.
	p = skip(p);
	while(find_word(p, until) == 0 && *p != '\0') {
		TokenizeMarker marker = tokenize_flow_control(p);
		switch(marker) {
			case END: 
				break;
			case STRINGLITERAL_START: cur = tokenize_string_literal(&p, cur);
				break;
			case CHARACTER_LITERAL_START:
				cur = tokenize_character_literal(&p, cur);
				break;
			case COMMENT_START: comment_skip(&p);
				break;
			case MACRO_START: cur = tokenize_macro(&p, cur);
				break;
			case KEYWORD: cur = tokenize_keyword(&p, cur);
				break;
			case SYMBOL: cur = tokenize_symbol(&p, cur);
				break;
			case NUMBER: cur = tokenize_number(&p, cur);
				break;
			case IDENTIFIER: cur = tokenize_identifier(&p, cur);
				break;
			case TOKENIZE_ERR:
				error_at(cur -> str,"Failed to tokenize");
		}

		p = skip(p);
		continue;
	}
	*pointer = p;
	return head.next;
}

/**
 * @brief read one line and make token list for MacroData.macroBody
 * 
 * @param pointer 
 * @return Token_t* 
 */
Token_t* tokenize_macro_one_line(char** pointer) {
	char* p = *pointer;
	Token_t head;// if this is pointer then this course segfault-error
	head.next = NULL;
	Token_t* cur = &head;
	while (*p != '\n' && *p != '\0')
	{
		p = skip_in_macro(p);
		TokenizeMarker marker = tokenize_macro_flow_control(p);
		switch (marker)
		{
		case END:
			break;
		case STRINGLITERAL_START: cur = tokenize_string_literal(&p, cur);
			break;
		case CHARACTER_LITERAL_START: cur = tokenize_character_literal(&p, cur);
			break;
		case COMMENT_START: comment_skip(&p);
			break;
		case KEYWORD: cur = tokenize_keyword(&p, cur);
			break;
		case SYMBOL: cur = tokenize_symbol(&p, cur);
			break;
		case NUMBER: cur = tokenize_number(&p, cur);
			break;
		case IDENTIFIER: cur = tokenize_identifier(&p, cur);
			break;
		case TOKENIZE_ERR:
			error_at(cur -> str,"Failed to tokenize");
		}
		continue;	
	}
	new_token(TK_EOF, cur, p);
	*pointer = p;
	return head.next;
}

Token_t *tokenize(char **pointer) {
	char* p = *pointer;
	Token_t head;
	head.next = NULL;
	Token_t *cur = &head;

	while(*p != '\0')
	{
		p = skip(p);
		TokenizeMarker marker = tokenize_flow_control(p);
		switch (marker)
		{
		case END: 
			break;
		case STRINGLITERAL_START: cur = tokenize_string_literal(&p, cur);
			break;
		case CHARACTER_LITERAL_START:
			cur = tokenize_character_literal(&p, cur);
			break;
		case COMMENT_START: comment_skip(&p);
			break;
		case MACRO_START: cur = tokenize_macro(&p, cur);
			break;
		case KEYWORD: cur = tokenize_keyword(&p, cur);
			break;
		case SYMBOL: cur = tokenize_symbol(&p, cur);
			break;
		case NUMBER: cur = tokenize_number(&p, cur);
			break;
		case IDENTIFIER: cur = tokenize_identifier(&p, cur);
			break;
		case TOKENIZE_ERR:
			error_at(cur -> str,"Failed to tokenize");
		}
		continue;	
	}
	new_token(TK_EOF, cur, p);
	*pointer = p;
	return head.next;
};

char* tokenize_macro_define(char* p) {
	// get identifier
	p = skip_in_macro(p);
	char* q = p;
	while(!is_space(*q) && *q != ',' && !is_symbol(q))
	{
		q++;
	}
	char* name = calloc(q - p, sizeof(char));
	Memory_copy(name, p, q - p);
	p = q;
	MacroData* data = new_MacroData(name, MACRO_OBJECT, NULL, NULL);

	skip_in_macro(p);
	if(*p == '(')
	{// read parameters
		data -> tag = MACRO_FUNCTION;
		Vector* v = make_vector();
		p++;
		while(*p != ')')
		{
			// read parameters
			char* q = p;
			while(*q != ',' && *q != ')' && !is_space(*q))
				q++;
			unsigned int length = q -p;
			char* param = calloc(length, sizeof(char));
			Memory_copy(param, p, length);
			
			Vector_push(v, param);

			p = q;
			p = skip_in_macro(p);
			if(*p == ',')
				p++;
			continue;
		}
		p++;
		data -> parameters = v;
	}
	
	data -> macroBody = tokenize_macro_one_line(&p);
	Map_add(macros, name, data);
	return p;
}

char* skip_to_MACRO_ENDIF(char* p) {
	char* _endif = "#endif";
	while(*p != '\0')
	{
		if(*p == '#')
		{
			if(String_compare(p, _endif, 6)) {
				p = p + 6;
				return p;
			}	
		}
		p ++;
	}
	return p;
}

// This function finds the next marker(#else, #elif, #end)
char* get_next_flow_marker(char* p) {
	char* _else = "#else";
	int l_else = String_len(_else);
	
	char* _elif = "#elif";
	int l_elif = String_len(_elif);
	
	char* _end = "#endif";
	int l_end = String_len(_end);


	char* marker = NULL;
	while(*p != '\0') {
		if(*p == '#') {
			if(String_compare(p, _else, l_else)){
				return _else;
			}
			if(String_compare(p, _elif, l_elif)) {
				return _elif;
			}
			if(String_compare(p, _end, l_end)) {
				return _end;
			}
		}
		p++;
	}
	return marker;
}

// this function evaluate macro_token and tokenize if it is true until #end appears.
Token_t* tokenize_macro_conditional_flow(char** pointer, Token_t* cur, Token_t* macro_token) {
	char* p = *pointer;
	Expr* exp = parse_macro_expr(&macro_token);
	if(eval_Expr(exp))
	{
		*pointer = p;
		char* until = get_next_flow_marker(p);

		Token_t* token = tokenize_until(pointer, until);
		
		*pointer = skip_to_MACRO_ENDIF(*pointer);
		cur -> next = token;
		return Token_consume_to_last(cur);
	}
	
	p = skip_to_MACRO_ENDIF(p);
	*pointer = p;
	return cur;
}

// this function tokenize one line of macro expression.
// After tokenize, it passes tokens with cur and pointer to tokenize_macro_conditional_flow.
Token_t* tokenize_macro_if(char** pointer, Token_t* cur) {
	char* p = *pointer;
	p = skip_in_macro(p);
	// read and eval expression
	Token_t* token = tokenize_macro_one_line(&p);
	if(p[0] == '\n')
		p++;
	*pointer = p;

	return tokenize_macro_conditional_flow(pointer, cur, token);
}

// this function tokenize one line of macro expression.
// Then add defined token to its head.
// After tokenize, it passes tokens with cur and pointer to tokenize_macro_conditional_flow.
Token_t* tokenize_macro_ifdef(char** pointer, Token_t* cur) {
	char* p = *pointer;
	skip_in_macro(p);

	// read expression 
	Token_t* expression_token = tokenize_macro_one_line(&p);
	Token_t* token = new_Token_t(TK_OPERATOR, expression_token, 0, 7, "defined", NULL);
	if(*p == '\n') {
		p++;
	}
	*pointer = p;

	return tokenize_macro_conditional_flow(pointer, cur, token);
}


// this function tokenize one line of macro expression.
// Then add ! and defined token to its head.
// After tokenize, it passes tokens with cur and pointer to tokenize_macro_conditional_flow.
Token_t* tokenize_macro_ifndef(char** pointer, Token_t* cur) {
	char* p = *pointer;
	skip_in_macro(p);

	// read expression 
	Token_t* expression_token = tokenize_macro_one_line(&p);
	Token_t* token_defined = new_Token_t(TK_OPERATOR, expression_token, 0, 7, "defined", NULL);
	Token_t* token = new_Token_t(TK_OPERATOR, token_defined, 0, 1, "!", NULL);
	if(*p == '\n') {
		p++;
	}
	*pointer = p;

	return tokenize_macro_conditional_flow(pointer, cur, token);
}

// tokenize header file and connect a head of resulting tokens to cur -> next
// return a last token of resulting tokens
Token_t* tokenize_macro_include(char** pointer, Token_t** cur) {
	char* p = *pointer;
	p = skip_in_macro(p);
	if(*p == '"') {
		p++;
		char* q = p;
		while(*q != '\"') {
			if(*q == '\n' || *q == '\0') {
				error_at(p, "include file is not specified");
			}
			q++;
		}
		int len = q - p;
		char* fileName = calloc(len, sizeof(char));
		Memory_copy(fileName, p,len);
		char* includeBuf = file_open(fileName);
		Token_t* token = tokenize(&includeBuf); // cur points end of include file
		*pointer = q + 1;
		if(token -> kind != TK_EOF)
			return Token_tailHead(token, *cur);
		else
			return *cur;
	} else {
		error_at(p, "include file is not specified");
	}
}

Token_t* tokenize_macro(char** p, Token_t* cur) {

	keyword keyWord = is_keyword(*p);
	char* keywordString = get_keyword(keyWord);
	if(keyWord != KEYWORD_START && keywordString != NULL)
		*p = *p + String_len(keywordString);
	switch(keyWord)
	{
	case MACRO_DEFINE:
		*p = tokenize_macro_define(*p);
		return cur;
	case MACRO_IF:
		return tokenize_macro_if(p, cur);
	case MACRO_IFDEF:
		return tokenize_macro_ifdef(p, cur);
	case MACRO_IFNDEF:
		return tokenize_macro_ifndef(p, cur);
	case MACRO_INCLUDE:
		return tokenize_macro_include(p, &cur);
	default:
		error_at(*p, "Anonymous keyword");
	}
	return cur;				
}

Token_t* lexical_analyze(char *p) {
	return tokenize(&p);
}