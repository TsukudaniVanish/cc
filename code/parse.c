#include "cc.h"
//#include<stdlib.h>
//#inclide<string.h>


extern unsigned int String_len(char*);
extern int String_conpair(char*,char*,unsigned int);
extern void Memory_copy(void*,void*,unsigned int);

ScopeInfo* new_ScopeInfo(unsigned nested, unsigned number) {
	ScopeInfo* info = calloc(1, sizeof(ScopeInfo));
	info ->nested = nested;
	info ->number = number;
	return info;
}
ScopeInfo* ScopeInfo_copy(ScopeInfo* info) {
	return new_ScopeInfo(info ->nested, info ->number);
}
int ScopeInfo_equal(ScopeInfo* self, ScopeInfo* other) {
	if(self ->nested != other ->nested || self ->number != other ->number)
	{
		return 0;
	}
	return 1;
}
// compare Scope with current Scope
int ScopeInfo_inscope(ScopeInfo* info) {
	ScopeInfo* currentNest = ScopeController_get_current_scope(controller);
	if(currentNest ->nested > info -> nested)
		return 1;
	if(ScopeInfo_equal(currentNest, info))
	{
		return 1;
	}
	return 0;
}
ScopeController* ScopeController_init() {
	ScopeController* controller = calloc(1, sizeof(ScopeController));
	controller ->nestedScopeData = make_vector();
	controller -> currentNest = 0;
	Vector_push(controller ->nestedScopeData, new_ScopeInfo(0, 0));
	return controller;
}
void ScopeController_nest_appeared(ScopeController* con) {
	con ->currentNest ++;
	if(Vector_get_length(con ->nestedScopeData) >= con -> currentNest + 1)
	{
		ScopeInfo *current = Vector_at(con ->nestedScopeData, con ->currentNest);
		current ->number++;
		return;
	}
	else
	{
		ScopeInfo* newNest = new_ScopeInfo(con ->currentNest, 0);
		Vector_push(con ->nestedScopeData, newNest);
	}
}
void ScopeController_nest_disappeared(ScopeController* con) {
	con ->currentNest--;
	return;
}
ScopeInfo* ScopeController_get_current_scope(ScopeController* con) {
	return Vector_at(con ->nestedScopeData, con ->currentNest);
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


Vector* init_parser() {
	tagNameSpace = make_Map();
	nameTable = make_vector();
	if(controller == NULL)
		controller = ScopeController_init();
	// fuction ごとのコード
	Vector *codes = make_vector();	
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
	lvar -> scope = ScopeController_get_current_scope(controller);
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
Type *imptypecast(Node_t *node) {
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
	if(lvar != NULL)
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

//Node_t making function with type check typically used in parsing formula
Node_t *new_node( Node_kind kind,Node_t *l,Node_t *r, char *parsing_here) {
	Node_t *node = new_Node_t(kind,l,r,0,0,NULL,NULL);
	//type check
	if(typecheck(node) == 0)
	{
		error_at(parsing_here,"Type cast error");
	}
	node -> tp = imptypecast(node);
	if(node -> tp)
	{
		int node_tp = node -> tp -> Type_label;
		if(node_tp == TP_ARRAY) node_tp = TP_POINTER;
		if(node_tp == TP_POINTER && (node -> kind == ND_ADD || node -> kind == ND_SUB))
		{
			if(l -> kind == ND_NUM)
				l -> val = l -> val * node -> tp -> pointer_to -> size;
			if(r -> kind == ND_NUM)
				r -> val = r -> val * node -> tp -> pointer_to -> size;
		}
		return node;
	}
	else
	{
		error_at(parsing_here,"Type cast error");
	}
}

/*
 * @brief read fuction number of function arguments. *v has argument info after call this function
 * */
int Node_read_function_parameters(Token_t **token,Node_t **vector) {

	int to_return = 0;
	Node_t* v = new_Node_t(ND_ARGMENT,NULL,NULL,0,0,NULL,NULL);
	*vector = v;
	while (!find(')',token))
	{
		v -> left = declere(token);
		find(',',token);

		Node_t *next = new_Node_t(ND_ARGMENT,NULL,NULL,0,0,NULL,NULL);
		v -> right = next;

		to_return ++;

		v = next;
	}

	v -> kind = ND_BLOCKEND;
	return to_return;
	
}

Node_t *new_node_function_call(Token_t **token) {
	if(find_lvar((*token) -> str, (*token) -> length, &global) == NULL)
	{
		error_at((*token) -> str, "this is not exist in the ordinary name space");
	}
	Node_t *node = new_Node_t(ND_FUNCTIONCALL,NULL,NULL,0,0,NULL,NULL);
	node -> tp = Type_function_return(&node -> name,token);
	
	expect('(',token);

	Node_t *node_end = new_Node_t(0,NULL,NULL,0,0,NULL,NULL);
	node -> left = node_end;

	while (!find(')',token))
	{

		node -> val++;

		node_end -> kind = ND_ARGMENT;
		Node_t *node_rightend = new_Node_t(0,NULL,NULL,0,0,NULL,NULL);
		node_end ->left = add(token);
		node_end ->right = node_rightend;
		node_end = node_rightend;
		find(',',token);
	}
	node_end -> kind = ND_BLOCKEND;
	return node;
}

Node_t *new_node_function_definition(Token_t **token) {
	Node_t *node = new_Node_t(ND_FUNCTIONDEF,NULL,NULL,0,0,NULL,NULL);
	node = declere_specify(token, node);
	node = ident_specify(token, node); 

	declere_glIdent(node -> tp,node -> name,String_len(node -> name),&global);
	
	expect('(',token);	
	node -> val = Node_read_function_parameters(token,&node -> left);
	
	node -> right = stmt(token);
	return node;

}

Node_t *new_node_var(Token_t **token) {
	Lvar *lvar = NULL;
	Lvar *table = Vector_get_tail(nameTable);
	Token_t *ident = consume(token);
	if(ident != NULL)
		lvar = find_lvar(ident -> str,ident -> length,&table);
	else
	{
		fprintf(stderr,"Fail to consume token\n");
	}
	if(lvar != NULL && ScopeInfo_inscope(lvar -> scope))
	{//local variable
	
		return new_Node_t(ND_LVAL,NULL,NULL,0,lvar -> offset,lvar -> tp,lvar -> name);
	}
	lvar = find_lvar(ident -> str,ident -> length,&global);
	if(lvar != NULL)
	{//global variable
		return new_Node_t(ND_GLOBVALCALL,NULL,NULL,0,0,lvar -> tp,lvar -> name);
	}
	error_at(ident -> str,"Anonymous identifier: %*s", ident -> length, ident -> str);
}

Node_t *new_node_ident(Token_t**token)
{

	if(is_functioncall(token))
	{
		return new_node_function_call(token);
	}

	return new_node_var(token);
	
}

Node_t *new_node_block(Token_t ** token){
	Node_t *node = new_Node_t(ND_BLOCK,NULL,NULL,0,0,NULL,NULL);
	Node_t *node_top = node;

	while (!find('}',token))
	{
		node -> kind = ND_BLOCK;
		node -> left = stmt(token);

		Node_t *right = new_Node_t(ND_BLOCK,NULL,NULL,0,0,NULL,NULL);
		node -> right = right;

		node = right;
	}
	node -> kind = ND_BLOCKEND;
	return node_top;
}

Node_t *new_node_if(Token_t** token)
{
	expect('(',token);
	Node_t *condition = expr(token);
	expect(')',token);
	Node_t *statement = stmt(token);
	if((*token) -> kind == TK_ELSE)
	{
		consume(token);
		return new_Node_t(
				ND_ELSE,
				new_Node_t(ND_IFE,condition,statement,0,0,NULL,NULL),
				stmt(token),
				0,0,NULL,NULL);
	}
	return new_Node_t(
			ND_IF,
			condition,
			statement,
			0,0,NULL,NULL);
}

Node_t *new_node_while(Token_t **token)
{
	Node_t *condition = expr(token);
	Node_t *statement = stmt(token);
	return new_Node_t(ND_WHILE,condition,statement,0,0,NULL,NULL);
}

Node_t *new_node_for(Token_t **token)
{
	Node_t *init,*check,*update = NULL;
	expect('(',token);
	if(!find(';',token))
	{// this is not infinite loop
		if(is_lvardec(token))
			init = declere(token);
		else
			init = expr(token);
		expect(';',token);
		check = expr(token);
		expect(';',token);
		update = expr(token);
	}
	else
	{
		expect(';',token);
	}
	expect(')',token);
	return new_Node_t(
			ND_FOR,
			new_Node_t(
				ND_FORUPDATE,
				new_Node_t(
					ND_FORINITCONDITION,
					init,
					check,
					0,0,NULL,NULL),
				update,
				0,0,NULL,NULL),
			stmt(token),
			0,0,NULL,NULL);
}

Node_t *new_node_return(Token_t **token) {
	Node_t *node = new_Node_t(ND_RETURN, expr(token),NULL,0,0,NULL,NULL);
	expect(';',token);
	return node;
}

Node_t *new_node_flow_operation(Token_kind kind,Token_t **token) {
	switch(kind) {
	case TK_IF: return new_node_if(token);			
	case TK_WHILE: return new_node_while(token);
	case TK_FOR: return new_node_for(token);
	case TK_RETURN: return new_node_return(token);
	default:
		return NULL;
	}
}

Node_t *new_node_num(int val) {
	Node_t *node = new_Node_t(ND_NUM,NULL,NULL,val,0,new_tp(TP_INT,NULL,4),NULL);
	return node;
}



Node_t *new_node_stringiter(Token_t ** token) {
	Node_t *node = new_Node_t(ND_STRINGLITERAL,NULL,NULL,0,0,new_tp(TP_POINTER,new_tp(TP_CHAR,NULL,1),8),NULL);
	
	Lvar *iter = declere_ident(node -> tp,(*token) -> str,(*token) -> length,&string_iter);
	
	node -> name = calloc(iter -> length, sizeof(char));
    Memory_copy(node -> name,iter -> name,iter -> length);
	node -> offset = iter -> offset;
	
	consume(token);
	expect('\"',token);
	return node;
}

Node_t *new_node_glob_ident(Token_t**token) {

	if(is_functioncall(token))
	{
		return new_node_function_definition(token);
	}

	Node_t *node = new_Node_t(ND_GLOBVALDEF,NULL,NULL,0,0,NULL,NULL);
	node = declere_specify(token, node);
	if(node -> tp -> Type_label != TP_STRUCT || (*token) -> kind == TK_IDENT || (*token) -> kind == TK_OPERATOR)
		node = ident_specify(token, node);

	if(node -> tp -> Type_label == TP_STRUCT && node -> name == NULL)
	{
		expect(';', token);
		return node;
	}
	
	Lvar *lvar = find_lvar(node -> name, String_len(node -> name), &global);
	if(lvar == NULL)
		lvar = declere_glIdent(node -> tp,node -> name, String_len(node -> name),&global);
	if (find('=',token))//変数の代入
	{
		if(node -> tp -> Type_label == TP_STRUCT || node -> tp -> Type_label == TP_ARRAY)
			node = init(token, node);
		else
			node -> val = expect_num(token);
	}
	expect(';',token);
	return node;
	
}

Node_t *new_node_ref_deref(Token_t **token) {

	Node_t *node = NULL;
	switch ((*token)->str[0])
		{
		case '*':

			(*token) = (*token) -> next;

			node = new_Node_t(ND_DEREF, unitary(token), NULL,0,0,NULL,NULL);
			if( node -> left -> tp -> Type_label == TP_ARRAY)
			{
				node -> tp = node -> left -> tp -> pointer_to;
				return node;
			}
			if( node -> left -> tp -> Type_label != TP_POINTER )
			{
				error_at((*token)->str, "Cannot accsess this variable\n");
			}
			else
			{
				node -> tp = node -> left -> tp -> pointer_to;
				return node;
			}
		
		case '&':

			(*token) = (*token) -> next;

			node = new_Node_t(ND_ADDR,unitary(token), NULL,0,0,NULL,NULL);
			node -> tp = new_tp(TP_POINTER,node -> left -> tp,8);
			return node;
		}
	return node;
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
 * declere = declere_specify ident_specify ( "=" init  )?
 * init =  expr | "{" init_list ","? "}"
 * init_list = init ( "," init)*
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
 * 			|( primary [expr] | primary '++' | primary '--' | primary "." ident | primary "->" ident)*
 * primary = num 
 * 			| indent 
 * 			| "(" expr ")"
 * 			| "\"" string literal "\""
 * end markers:
 * 		"..."
 * 		num
 * 		indent
 * 		string literal
 */

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
/*
 * implimentation
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

		error_at((*token) -> str , "Expected type specifyer");

	}
	if( !( (*token) ->next ) && (*token) -> next -> kind != TK_IDENT ){


		error_at((*token) -> str,"Expected function identifier");
	}
	return new_node_glob_ident(token);
}

Node_t *stmt(Token_t **token) {

	Node_t *node = NULL;

	if( (*token) -> kind > TOKEN_FLOW_OPERATION_START - 1 && (*token) -> kind < TOKEN_SIZEOF)
	{//if (else) while for return
		node = new_node_flow_operation(consume(token) -> kind,token);
	}
	else if(find('{',token))
	{
		ScopeController_nest_appeared(controller);
		node = new_node_block(token);
		ScopeController_nest_disappeared(controller);
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
	ScopeInfo* currentNest = ScopeController_get_current_scope(controller);
	Lvar *table = Vector_get_tail(nameTable);
	Node_t* node = new_Node_t(ND_LVAL, NULL, NULL, 0, 0, NULL, NULL);

	node = declere_specify(token, node);
	if(node -> tp -> Type_label != TP_STRUCT || (*token) -> kind == TK_IDENT || (*token) -> kind == TK_OPERATOR)
	{
		node = ident_specify(token, node);
	}

	if(node -> tp -> Type_label == TP_STRUCT && node -> name == NULL)
	{//  struct definition only
		return node;
	}
	Lvar* lvar = find_lvar(node -> name, String_len(node -> name), &table);
	if(lvar == NULL || !ScopeInfo_equal(currentNest, lvar -> scope))
		lvar = declere_ident(node -> tp, node -> name,String_len(node -> name),&table);
	else
		error_at((*token) -> str, "Can't use same identifier: %s", node -> name);
	node -> offset = lvar -> offset;
	Vector_replace(nameTable,Vector_get_length(nameTable)-1,table);
	if(find('=',token))
	{
		parsing_here = (*token) -> str;
		if(node -> tp -> Type_label == TP_STRUCT || node -> tp -> Type_label == TP_ARRAY)
			node = init(token, node);
		else
			node = new_node(ND_ASSIGN, node, init(token, node), (*token) -> str);
		return node;
	}
	return node;

}

Node_t* init(Token_t** token, Node_t* node) {
	if(find('{', token))
	{
		node = init_list(token, node);
		find(',', token);
		expect('}', token);
		return node;
	}
	return assign(token);
}

Node_t* init_list(Token_t** token, Node_t* node) {
	if(node -> tp -> Type_label == TP_STRUCT)
	{
		StructData *data = Map_at(tagNameSpace, node -> tp -> name);
		int i = 0;
		char* member_name = Vector_at(data -> memberNames, i);
		Node_t* member = Map_at(data -> memberContainer, member_name);
		Node_t *initBranch = new_Node_t(ND_BLOCK, NULL, NULL, 0, 0, NULL, NULL);
		node = new_Node_t(ND_INITLIST, node, initBranch, 0, 0, node -> tp, NULL);
		initBranch -> left = init(token, member);
		while(find(',', token))
		{
			if(i < Vector_get_length(data -> memberNames))
			{
				i++;
				member_name = Vector_at(data -> memberNames, i);
				member = Map_at(data -> memberContainer, member_name);
			}
			else
				error_at((*token) -> str, "too many initializer");

			initBranch -> right = new_Node_t(ND_BLOCK, init(token, member), NULL, 0, 0, NULL, NULL);
			initBranch = initBranch -> right;
		}
		initBranch -> right = new_Node_t(ND_BLOCKEND, NULL, NULL, 0, 0, NULL, NULL);
		return node;
	}
	if(node -> tp -> Type_label == TP_ARRAY)
	{
		Node_t* initBranch = new_Node_t(ND_BLOCK, init(token, node), NULL, 0, 0, NULL, NULL);
		node = new_Node_t(ND_INITLIST, node, initBranch, 0, 0, NULL, NULL);
		while(find(',', token))
		{
			initBranch -> right = new_Node_t(ND_BLOCK, init(token, node), NULL, 0, 0, NULL, NULL);
			initBranch = initBranch -> right;
		}
		initBranch -> right = new_Node_t(ND_BLOCKEND, NULL, NULL, 0, 0, NULL, NULL);
		return node;
	}
	return NULL;
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
	node -> tp -> size = tail -> offset + tail -> tp -> size;
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
		unsigned threshold = prev -> offset + prev -> tp -> size + (8 - ((prev -> offset + prev -> tp -> size) % 8));
		if(prev -> offset + prev -> tp -> size + member -> tp -> size <= threshold)
		{
			member -> offset = prev -> offset + prev -> tp -> size;
		}
		else{
			member -> offset = threshold;
		}
	}
	else
	{
		member -> offset = 0;
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



Node_t *unitary(Token_t **token) {
	Node_t *node = NULL;
	if((*token)-> kind == TK_SIZEOF)
	{
		(*token) = (*token) -> next;
		node = unitary(token);
		if(node -> tp -> Type_label == TP_STRUCT)
		{
			StructData *data = Map_at(tagNameSpace, node -> tp -> name);
			return new_node_num(data -> size);
		}
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
		parsing_here = (*token) -> str;
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
		if(find('.', token))
		{
			if(!is_lval(node))
			{
				error_at(parsing_here, "lval is expected");
			}
			if(node -> tp -> Type_label != TP_STRUCT)
				error_at((*token) -> str, "member access is expected");
			char *memberName = expect_ident(token);
			StructData* data = Map_at(tagNameSpace, node -> tp -> name);
			if(!Map_contains(data -> memberContainer, memberName))
				error_at((*token) -> str, "unknown member name");
			Node_t* member = Map_at(data -> memberContainer, memberName);
			node= new_Node_t(ND_DOT, node, member, 0, 0, member -> tp, member -> name);
			continue;
		}
		if(find(ARROW, token))
		{
			if(node -> tp -> Type_label != TP_POINTER)
				error_at(parsing_here, "Pointer type is expected");
			node = new_Node_t(ND_DEREF, node, NULL, 0, 0, node -> tp -> pointer_to, NULL);
			if(node -> tp -> Type_label != TP_STRUCT)
				error_at((*token) -> str, "member access is expected");
			char *memberName = expect_ident(token);
			StructData* data = Map_at(tagNameSpace, node -> tp -> name);
			if(!Map_contains(data -> memberContainer, memberName))
				error_at((*token) -> str, "unknown member name");
			Node_t* member = Map_at(data -> memberContainer, memberName);
			node = new_Node_t(ND_DOT, node, member, 0, 0, member -> tp, member -> name);
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

