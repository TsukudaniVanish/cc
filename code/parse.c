#include "cc.h"
//#include<stdlib.h>
//#inclide<string.h>


extern unsigned int String_len(char*);
extern int String_conpair(char*,char*,unsigned int);
extern void Memory_copy(void*,void*,unsigned int);

Vector* init_parser() {
	tagNameSpace = make_Map();
	nameTable = make_vector();
	// fuction ごとのコード
	Vector *codes = make_vector();	//token を抽象構文木に変換
	return codes;
}
Type* new_Type(int label, Type* pointer_to, unsigned int size, char* name) {
	
	Type* tp = calloc(1,sizeof(Type));
	tp -> Type_label = label;
	tp -> pointer_to = pointer_to;
	tp -> size = size;
	tp -> name = NULL;
	return tp;
}

Type* new_tp(int label,Type* pointer_to,long int size) {

	
	return new_Type(label, pointer_to, size, NULL);
}


Type *Type_function_return(char **name,Token_t** token) {
	Token_t *buf = consume(token);
	if(*name)
		free(*name);
	*name = calloc(buf -> length, sizeof(char));
	Memory_copy(*name,buf -> str,buf -> length);
	Lvar *lvar = find_lvar(*name,buf -> length,&global);
	if(lvar != NULL)
		return lvar -> tp;
	return NULL;
}




Lvar *find_lvar(char *name,int length,Lvar **locals) {
	
	for(Lvar *var = *locals; var;var = var -> next)
	{
		if( var -> length == length && String_conpair( name, var ->name,length))
		{
			return var; 
		}
	}
	return NULL;
}

Lvar *new_lvar(Type *tp,char *name, int length,Lvar *next) {

	Lvar *lvar = calloc(1,sizeof(Lvar));
	lvar -> next = next;
	//名前コピー
	lvar -> name = calloc(length,sizeof(char));
	Memory_copy(lvar -> name,name,length);
	// 名前コピー終わり
	lvar -> length = length;
	lvar -> tp = tp;

	if(next)
	{
		lvar -> offset = next -> offset + (lvar -> tp -> size);
	}
	else
	{
		lvar -> offset = (lvar -> tp->size);
	}
	return lvar;
}

int typecheck(Node_t *node) {

	Type *tp_l , *tp_r;
	if(node -> left)
	{
		tp_l = node -> left -> tp;
	}
	else
	{
		return 0;
	}
	if(node -> right)
	{
		tp_r = node -> right -> tp;
	}
	else
	{
		return 0;
	}

	if( tp_l -> Type_label == tp_r -> Type_label)
		return 1;

	return 2;
}

#if defined(Min)
	#define Is_type_integer(t) INTEGER_TYPE_START == Min(t,INTEGER_TYPE_START) ? t <= INTEGER_TYPE_END ? 1: 0: 0 
#endif
Type *imptypecast(Node_t *node)
{
	int tp_l , tp_r;

	switch (typecheck(node))
	{
	case 0:
		return NULL;
	case 1:
		return node -> left -> tp;
	default:
		tp_l = node -> left -> tp -> Type_label;
		tp_r = node -> right -> tp -> Type_label;
		break;
	}
	
	//array -> pointer
	
	if(tp_l == TP_ARRAY)	tp_l = TP_POINTER;
	if(tp_r == TP_ARRAY) 	tp_r = TP_POINTER;

	if(tp_l == tp_r)
		return node -> left -> tp;

	if(tp_l == TP_POINTER)
	{
		if(Is_type_integer(tp_r))
		{
			return node -> left -> tp;
		}
		return NULL;
	}
	else if(tp_r == TP_POINTER)
	{
		if(Is_type_integer(tp_l))
		{
			return node -> right -> tp;
		}
		return NULL;
	}
	if(Is_type_integer(tp_l) && Is_type_integer(tp_r))
	{
		return tp_l < tp_r ? node -> left -> tp  : node -> right -> tp;
	}
	return NULL;
}

int is_lvardec(Token_t **token) {
	if((*token) -> kind > 299)
		return 1;
	return 0;
}

Lvar *declere_ident(Type *tp, char *name,int len ,Lvar **table) {
	Lvar *lvar = new_lvar(tp,name,len,*table);
	*table = lvar;
	return lvar;
}

Lvar *declere_glIdent(Type *tp,char *name, int len, Lvar **table) {
	Lvar *lvar = find_lvar(name,len,table);
	if(lvar && lvar -> tp -> Type_label != TP_ARRAY)
		return NULL;
	lvar = new_lvar(tp,name,len,*table);
	*table = lvar;
	return lvar;	
}

int is_arrmemaccess(Token_t **token) {
	Token_t *buf = *token;

	if(find('[',&buf))
	{
		while (buf -> kind !=TK_EOF)
		{
			if(find(']',token))
				break;
			buf = buf -> next;
		}
		return 1;
	}
	return 0;
}
#define Is_type_pointer(kind) (kind == TP_POINTER || kind == TP_ARRAY? 1: 0)
Node_t* arrmemaccess(Token_t **token , Node_t** prev) {
	expect('[',token);
	Node_t *node = expr(token);
	expect(']',token);
	
	if(
		(Is_type_pointer(node -> tp -> Type_label) && (*prev) -> tp -> Type_label == TP_INT) ||
		(node -> tp -> Type_label == TP_INT && Is_type_pointer((*prev) -> tp -> Type_label))
	){
		Node_t *get_address = new_node(ND_ADD,*prev,node, (*token) -> str);
		return new_Node_t(ND_DEREF,get_address,NULL,0,0,get_address -> tp -> pointer_to,NULL);
	}

	fprintf(stderr, "type : %d\n", (*prev) -> tp -> Type_label);
	error_at((*token) -> str,"lval is expected");
}

int is_lval(Node_t* node) {
	if(node -> tp)
	switch(node -> kind)
	{
		case ND_FUNCTIONCALL: return 1;
		case ND_GLOBVALCALL: return 1;
		case ND_LVAL: return 1;
		case ND_DEREF: return 1;
		case ND_STRINGLITERAL: return 1;
		default:
			return 0;
	}
}

StructData* make_StructData() {
	StructData* data = calloc(1, sizeof(StructData));
	data -> size = 0;
	data -> memberNames = make_vector();
	data -> memberContainer = make_Map();
	return data;
}

void StructData_add(StructData* data, Node_t* member) {
	data -> size += member -> tp -> size;
	Vector_push(data -> memberNames, member -> name);
	Map_add(data -> memberContainer, member -> name, member);
}




/*
 * token から構文木を生成 
 */
/*
 * (someting)* <= something の0以上の繰り返し
 * 生成文法
 *
 * program = func*
 * func = ident "(" (type? ident)*  ")"  stmt 
 * stmt = expr";"
 * 		| declere ";"
 * 		| "{" stmt* "}"
 * 		| "if" "(" expr  ")" stmt ( "else" stmt  )?
 * 		| "while"  "(" expr ")" stmt
 * 		| "for"  "(" expr?; expr? ; expr? ")"stmt
 * 		| "return" expr";"
 * declere = declere_specify ident_specify ( "=" expr )?
 * ident_specify = pointer? ident ("[" expr "]")*
 * declere_specify =  type_specify
 * type_specify = "void"
 * 		| "int"
 * 		| "unsigned int"
 * 		| "unsigned"
 * 		| "char"
 * 		| struct_specify
 * 	struct_specify = "struct" ( ident? "{" struct_declere* "}" | ident )
 * 	struct_declere = struct_declere_inside ("," struct_declere_inside)* ";"  
 * 	struct_declere_inside = type_specify ident_specify 
 * pointer = "*"*
 * expr = assign
 * assign = log_or ("=" expr )?
 * log_or = log_and (|| log_or)?
 * log_and = equality (&& log_and)?
 * equality = relational("==" relational | "!=" relational)*
 * relational = add( "<=" add | "<" add | ">=" add | ">" add  )*
 * add = mul( "+"mul | "-"mul)* 
 * mul = unitary ("*" unitary | "/" unitary )*
 * unitary = postfix
 * 			|"sizeof" unitary
 * 			| ('+' | '-' | '*' | '&' ) unitary
 * 			| ('++' | '--') postfix
 * postfix = primary 
 * 			|( primary [expr] | primary '++' | primary '--')*
 * primary = num 
 * 			| indent 
 * 			| "(" expr ")"
 * 			| "\"" string literal "\""
 * 終端記号:
 * 		num
 * 		indent
 * 		string literal
 */
/*
 * 関数実装
 */




void program(Token_t **token,Vector *codes) {

	while(!at_eof(token))
	{
		Vector_push(nameTable,NULL);
		Vector_push(codes, func(token));
	}
	Vector_push(nameTable,NULL);
}

Node_t *func(Token_t **token) {

	if ((*token) -> kind <= TOKEN_TYPE - 1){

		error_at((*token) -> str , "型宣言がありません");

	}
	if( !( (*token) ->next ) && (*token) -> next -> kind != TK_IDENT ){


		error_at((*token) -> str,"関数名が必要です");
	}
	return new_node_glob_ident(token);
}

Node_t *stmt(Token_t **token) {

	Node_t *node = NULL;

	if( (*token) -> kind > TOKEN_FLOW_OPERATION_START - 1 && (*token) -> kind < TOKEN_SIZEOF)
	{//if (else) while for return をパース

		node = new_node_flow_operation(consume(token) -> kind,token);

	}
	else if(find('{',token))
	{
		node = new_node_block(token);
	}
	else if(is_lvardec(token))
	{
		node = declere(token);
		expect(';',token);
		if(node -> kind == ND_LVAL)
			return NULL;
		return node;
	}
	else
	{
		node = expr(token);
		expect(';',token);
	}
	
	return node;
}

Node_t *declere(Token_t **token) {
	Lvar *table = Vector_get_tail(nameTable);
	Node_t* node = new_Node_t(ND_LVAL, NULL, NULL, 0, 0, NULL, NULL);

	node = declere_specify(token, node);
	if(node -> tp -> Type_label != TP_STRUCT || (*token) -> kind == TK_IDENT)
	{
		node = ident_specify(token, node);
	}

	if(node -> tp -> Type_label == TP_STRUCT && node -> name == NULL)
	{//  struct definition only
		return node;
	}
	Lvar *lvar = declere_ident(node -> tp, node -> name,String_len(node -> name),&table);
	node -> offset = lvar -> offset;
	Vector_replace(nameTable,Vector_get_length(nameTable)-1,table);
	if(find('=',token))
	{
		parsing_here = (*token) -> str;
		node = new_node(ND_ASSIGN,node,expr(token), (*token) -> str);
		return node;
	}
	return node;

}
/*@brief specify identifier : is it an pointer to someting ? identifier name?
 * */
Node_t* ident_specify(Token_t** token, Node_t* node) {
	if((*token) -> kind == TK_OPERATOR)
	{
		node = pointer(token, node);
	}
	node -> name = expect_ident(token);
	while(find('[', token))
	{
		int array_size = expect_num(token) * (node -> tp -> size);
		expect(']', token);
		node -> tp = new_tp(TP_ARRAY, node -> tp, array_size);
	}
	return node;
}
/*@brief specify decleration type struct constant or volatile?
 * */
Node_t* declere_specify(Token_t** token, Node_t* node) {
	return type_specify(token, node);
}

/*
 * @brief pointer parse pointer '*'. change assigned node -> tp.
 * */
Node_t *pointer(Token_t** token, Node_t* node) {
	for(;;)
	{
		if(find('*', token))
		{
			if(node -> tp == NULL)
				return node;
			Type* tp = new_tp(TP_POINTER, node -> tp, 8);
			node -> tp = tp;
			continue;
		}
		break;
	}
	return node;
}
/*
 * @brief type parse type and struct decleration. change assigned node -> tp when assigned token -> kind >=  TOKEN_TYPE and < TK_STRUCT. 
 * */
Node_t *type_specify(Token_t** token, Node_t* node) {
	if((*token) -> kind >= TOKEN_TYPE && (*token) -> kind != TK_STRUCT)
	{	
		node -> tp = (*token) -> tp;
		*token = (*token) -> next;
		return node;
	}
	if((*token) -> kind == TK_STRUCT)
	{
		consume(token);
		node -> tp = new_tp(TP_STRUCT, NULL, 0);
		return struct_specify(token, node);
	}
	error_at((*token) -> str, "Type keyword is expected");
}
/*
 * @brief Doesn't change node address
 * */
Node_t* struct_specify(Token_t** token, Node_t* node) {
	char* name = NULL; 
	if((*token) -> kind == TK_IDENT)
	{
		name = calloc((*token) -> length, sizeof(char));
		Memory_copy(name, (*token) -> str, (*token) -> length);
	}
	else
	{
		name = "_";
	}
	node -> tp -> name = name;
	consume(token);
	StructData* structData = Map_at(tagNameSpace, name);
	if(structData == NULL)
	{
		structData = make_StructData();
		Map_add(tagNameSpace, name, structData);
	}

	if(find('{', token))
	{
		while(!find('}', token))
		{
			struct_declere(token, node);
		}
	}
	char* lastMember = Vector_get_tail(structData -> memberNames);
	if(lastMember == NULL)
	{
		return node;
	}
	Node_t* tail = Map_at(structData -> memberContainer, lastMember);
	node -> tp -> size = tail -> offset;
	return node;
}
/*
 * @brief Doesn't change node address
 * */
Node_t* struct_declere(Token_t** token, Node_t* node) {
	node = struct_declere_inside(token, node);
	while(find(',', token))
	{
		node = struct_declere_inside(token, node);
	}
	expect(';', token);
	return node;
}
/*
 * @brief Doesn't change node address
 * */
Node_t* struct_declere_inside(Token_t** token, Node_t* node) {
	StructData* data = Map_at(tagNameSpace, node -> tp -> name);
	char* previousMember = Vector_get_tail(data -> memberNames);
	Node_t* member = new_Node_t(ND_LVAL, NULL, NULL, 0, 0, NULL, NULL);
	
	member = type_specify(token, member);
	if((*token) -> kind == TK_OPERATOR || (*token) -> kind == TK_IDENT)
	{
		member = ident_specify(token, member);
	}
	else
	{
		member -> name = "_";
	}

	if(previousMember != NULL)
	{
		Node_t* prev = Map_at(data -> memberContainer, previousMember);
		unsigned threshold = prev -> offset + (8 - (prev -> offset % 8));
		if(prev -> offset + member -> tp -> size <= threshold)
		{
			member -> offset = prev -> offset + member -> tp -> size;
		}
		else{
			member -> offset = threshold + member -> tp -> size;
		}
	}
	else
	{
		member -> offset = member -> tp -> size;
	}
	StructData_add(data, member);
	return node;
}

Node_t *expr(Token_t** token) {
	return assign(token);
}

Node_t *assign(Token_t **token) {
	char* parsing_here = (*token) -> str;// for error detection
	Node_t *node = log_or(token);
	
	if( find('=',token) )
	{
		if(is_lval(node))
		{
			node = new_node(ND_ASSIGN,node,expr(token), (*token) -> str);
			return node;
		}
		else
		{
			error_at(parsing_here ,"laval is expected");
		}
	}
	return node;
}

Node_t* log_or(Token_t **token) {
	Node_t *node = log_and(token);
	if(find(LOG_OR, token))
	{
		node = new_Node_t(ND_LOGOR, node, log_or(token), 0, 0, new_tp(TP_INT, NULL, 4), NULL);
	}
	return node;
}

Node_t* log_and(Token_t **token) {
	Node_t *node = equality(token);
	if(find(LOG_AND, token))
	{
		node = new_Node_t(ND_LOGAND, node, log_and(token), 0, 0, new_tp(TP_INT, NULL, 4), NULL);
	}
	return node;
}


Node_t *equality(Token_t **token){


	Node_t *node = relational(token);

	for(;;)
	{	
		parsing_here = (*token) -> str;
		if(find(EQUAL,token))
		{
			node = new_node(ND_EQL,node,relational(token), (*token) -> str);
		
		}else if( find(NEQ,token) )
		{	
			node = new_node(ND_NEQ,node,relational(token), (*token) -> str);
		}
		else
		{
			return node;
		}
	}
}

Node_t *relational(Token_t **token) {

	Node_t *node = add(token);

	for (;;)
	{
		parsing_here = (*token) -> str;
		if( find(LEQ,token) )
		{		
			node = new_node(ND_LEQ,node,add(token), (*token) -> str);
		}
		else if( find('<',token))
		{
			node = new_node(ND_LES,node,add(token), (*token) -> str);
		}
		else if(find(GEQ,token))
		{
			node = new_node(ND_LEQ,add(token),node, (*token) -> str);
		}
		else if(find('>',token))
		{
			node = new_node(ND_LES,add(token),node, (*token) -> str);	
		}
		else
		{	
			return node;
		}
	}
}

Node_t *add(Token_t **token) {
	Node_t *node = mul(token);

	for(;;)
	{
		parsing_here = (*token) -> str;
		if( find('+',token) )
		{
			node = new_node(ND_ADD,node,mul(token), (*token) -> str);
		}
		else if( find('-',token) )
		{
			node = new_node(ND_SUB,node,mul(token), (*token) -> str);
		}
		else
		{
			return node;
		}
	}
}

Node_t *mul(Token_t **token) {

	Node_t *node = unitary(token);

	for(;;)
	{
		parsing_here = (*token) -> str;
		if(find('*',token))
		{
			node = new_node(ND_MUL,node,unitary(token), (*token) -> str);
		}
		else if(find('/',token))
		{
			node = new_node(ND_DIV,node,unitary(token), (*token) -> str);
		}
		else
		{
			return node;
		}
	}
}



Node_t *unitary(Token_t **token)
{
	Node_t *node = NULL;
	if((*token)-> kind == TK_SIZEOF)
	{
		(*token) = (*token) -> next;
		node = unitary(token);
		if( node -> val != 0 )
		{
			node = new_node_num(node -> val);
		}
		else
		{	
			node = new_node_num(node ->tp -> size);
		}	
		return node;		
	}

	if(find(INC,token))
	{
		if((*token) -> kind != TK_IDENT)
		{// this is not enough condition for detect Left value.
			error_at((*token) -> str, "lval is expected");
		}
		node = new_Node_t(ND_INC,unitary(token),NULL,0,0,NULL,NULL);
		node -> tp = node -> left -> tp;
		return node;
	}
	else if(find(DEC,token))
	{// this is not enough condition for detect Left value.	
		if((*token) -> kind != TK_IDENT)
		{
			error_at((*token) -> str, "lval is expected");
		}
		node = new_Node_t(ND_DEC,unitary(token),NULL,0,0,NULL,NULL);
		node -> tp = node -> left -> tp;
		return node;
	}

	if( find('+',token) )
	{
		node = unitary(token);
		return node;
		
	}
	else if( find('-',token) )
	{
		node = new_node(ND_SUB,new_node_num(0),unitary(token), (*token) -> str);
		node -> tp = node -> right -> tp;
		return node;
	}
	else if((*token) -> kind == TK_OPERATOR && ( (*token) -> str[0] == '*' || (*token) -> str[0] == '&'))
	{
		node = new_node_ref_deref(token);
		return node;
	}
		
	node = postfix(token);
	return node;
}

Node_t *postfix(Token_t **token) {
	Node_t *node = primary(token);
	if(node == NULL)
	{
		return node;
	}
	for(;;)
	{
		if(is_arrmemaccess(token))
		{
			node = arrmemaccess(token, &node);
			parsing_here = (*token) -> str;
			continue;
		}
		if(find(INC,token))
		{
			if(!is_lval(node))
			{
				error_at(parsing_here, "lval is expected");
			}
			node = new_Node_t(ND_INC,NULL,node,0,0,NULL,NULL);
			node -> tp = node -> right -> tp;
			continue;
		}
		if(find(DEC,token))
		{
			if(!is_lval(node))
			{
				error_at(parsing_here, "lval is expected");
			}
			node = new_Node_t(ND_DEC,NULL,node,0,0,NULL,NULL);
			node -> tp = node -> right -> tp;
			continue;
		}
		return node;
	}
}

Node_t *primary(Token_t **token) {

	parsing_here = (*token) -> str;
	Node_t *node = NULL;

	if(find('(',token))
	{
		node = expr(token);
		expect(')',token);// ')'かcheck

	}else if((*token)-> kind == TK_IDENT || ( (*token) -> kind >299) )
	{

		node = new_node_ident(token);
	}
	else if(find('\"',token))
	{
		node = new_node_stringiter(token);
	}
	else if((*token) -> kind == TK_CONST)
	{
		node = new_node_num(expect_num(token));
	}
	return node;
}

