/* bnf 
 * (something)* : something appears at least 0 times.
 * syntax
 *
 * program = func*
 * 
 * func = declare_specify ident_specify "(" type_parameter_list  ")"  stmt
 * type_parameter_list = parameter_list 
 * parameter_list = parameter_declare ("," parameter_declare )*
 * parameter_declare = declare_specify ident_specify | "..."
 * stmt = expr";"
 * 		| declare ";"
 * 		| "{" stmt* "}"
 * 		| "if" "(" expr  ")" stmt ( "else" stmt  )?
 * 		| "while"  "(" expr ")" stmt
 * 		| "for"  "(" expr?; expr? ; expr? ")"stmt
 * 		| "return" expr";"
 * declare = declare_specify* ident_specify ( "=" init  )?
 * init =  expr | "{" init_list ","? "}"
 * init_list = init ( "," init)*
 * ident_specify = pointer? ident ("[" expr "]")*
 * declare_specify =  type_specify
 * type_specify = "void"
 * 		| "int"
 * 		| "unsigned int"
 * 		| "unsigned"
 * 		| "char"
 * 		| enum_specify
 * 		| struct_union_specify
 * 	struct_union_specify = ("struct" | "union") ( ident? "{" struct_declare* "}" | ident )
 * 	struct_declare = struct_declare_inside ("," struct_declare_inside)* ";"  
 * 	struct_declare_inside = type_specify ident_specify
 * 	enum_specify = "enum" ( ident | ident? "{" enum_list "}" )
 * 	enum_list = enum ( "," enum )*
 * 	enum = indent ( "-" expr)
 * pointer = "*"*
 * expr = assign
 * assign = conditional ("=" expr )?
 * conditional = log_or | log_or "?" expr ":" conditional 
 * log_or = log_and (|| log_or)?
 * log_and = equality (&& log_and)?
 * equality = relational("==" relational | "!=" relational)*
 * relational = add( "<=" add | "<" add | ">=" add | ">" add  )*
 * add = mul( "+"mul | "-"mul)* 
 * mul = cast  | cast ("*" cast | "/" cast )*
 * cast = unitary | "(" type_name ")" cast 
 * type_name = type_specify pointer?
 * unitary = postfix
 * 			|"sizeof" unitary
 * 			| ('+' | '-' | '*' | '&' | '!' ) postfix
 * 			| ('++' | '--') unitary
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
#include "cc.h"


extern char* new_String(unsigned int len);
extern unsigned int String_len(char*);
extern int String_compare(char*,char*,unsigned int);
extern char* String_add(char*, char*);
extern char* i2a(int);
extern void Memory_copy(void*,void*,unsigned int);
static int struct_number = 0;

extern void exit(int);
static void* NULL = (void*) 0;
extern void* calloc(unsigned nmem, unsigned size);
extern void free(void*);

// Scope is set with a current scope.
NameData* new_NameData(int tag) {
	NameData* data = calloc(1, sizeof(NameData));
	data -> tp = NULL;
	data -> tag = tag;
	data -> scope = ScopeInfo_copy(ScopeController_get_current_scope(controller));
	return data;
}

NameData* set_tag_obj_to_ordinary_namespace(char* name) {
	NameData* data = new_NameData(TAG_OBJECT);
	Map_add(ordinaryNameSpace, name, data);
	return data;
}

NameData* search_from_ordinary_namespace(char* name, ScopeInfo* scope) {
	Vector* v = Map_get_all(ordinaryNameSpace, name);
	for (unsigned i = 0; i < Vector_get_length(v); i++)
	{
		NameData* maybe = Vector_at(v, i);
		if(ScopeInfo_in_right(scope, maybe -> scope))
			return maybe;
	}
	return NULL;
}

StructData* search_from_tag_namespece(char* name, ScopeInfo* scope) {
	Vector* v = Map_get_all(tagNameSpace, name);
	for (unsigned i = 0; i < Vector_get_length(v); i++)
	{
		StructData* maybe = Vector_at(v, i);
		if(ScopeInfo_in_right(scope, maybe -> scope))
			return maybe;
	}
	return NULL;
}

ScopeInfo* new_ScopeInfo(unsigned number,ScopeInfo* parent) {
	ScopeInfo* info = calloc(1, sizeof(ScopeInfo));
	info ->number = number;
	info -> parent = parent;
	return info;
}
ScopeInfo* ScopeInfo_copy(ScopeInfo* info) {
	return new_ScopeInfo(info -> number, info -> parent);
}
int ScopeInfo_equal(ScopeInfo* self, ScopeInfo* other) {
	if(self -> number == other -> number) {
		return 1;
	}
	return 0;
}
int ScopeInfo_in_right(ScopeInfo* info, ScopeInfo* current) {
	while (info != NULL)
	{
		if(ScopeInfo_equal(info, current)) {
			return 1;
		}
		info = info -> parent;
	}
	return 0;	
}
// compare Scope with current Scope
int ScopeInfo_inscope(ScopeInfo* info) {
	ScopeInfo* current_scope = ScopeController_get_current_scope(controller);
	return ScopeInfo_in_right(current_scope, info);
}

ScopeController* ScopeController_init() {
	ScopeController* controller = calloc(1, sizeof(ScopeController));
	controller ->nestedScopeData = make_vector();
	controller -> current_number = 0;
	controller -> total_number = 0;
	Vector_push(controller ->nestedScopeData, new_ScopeInfo(0, NULL));
	return controller;
}
void ScopeController_nest_appeared(ScopeController* con) {
	ScopeInfo* current_scope = ScopeController_get_current_scope(con);
	con ->total_number ++;
	ScopeInfo* newNest = new_ScopeInfo(con ->total_number, current_scope);
	Vector_push(con ->nestedScopeData, newNest);
	con -> current_number = newNest -> number;
}
void ScopeController_nest_disappeared(ScopeController* con) {
	// con ->current_number--;
	ScopeInfo* current_scope = ScopeController_get_current_scope(con);
	con -> current_number = current_scope -> parent -> number;
	return;
}
ScopeInfo* ScopeController_get_current_scope(ScopeController* con) {
	return Vector_at(con ->nestedScopeData, con ->current_number);
}
StructData* make_StructData() {
	StructData* data = calloc(1, sizeof(StructData));
	data -> size = 0;
	data -> scope = NULL;
	data -> memberNames = make_vector();
	data -> memberContainer = make_Map();
	return data;
}

// add member to data. update data -> size
void StructData_add(StructData* data, Node_t* member) {
	if(data -> tag == TAG_STRUCT) {
		data -> size = data -> size +  member -> tp -> size;
	}
	else if(data -> tag == TAG_UNION)
		data -> size = data -> size < member -> tp -> size? member -> tp -> size: data -> size;
	Vector_push(data -> memberNames, member -> name);
	Map_add(data -> memberContainer, member -> name, member);
}


Vector* init_parser() {
	ordinaryNameSpace = ordinaryNameSpace == NULL?make_Map(): ordinaryNameSpace;
	tagNameSpace = make_Map();

	nameTable = make_vector();

	if(controller == NULL)
		controller = ScopeController_init();

	// code for each function
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

Type* new_tp(int label,Type* pointer_to,unsigned int size) {

	
	return new_Type(label, pointer_to, size, NULL);
}


Type *Type_function_return(char **name,Token_t** token) {
	Token_t *buf = consume(token);
	if(*name)
		free(*name);
	*name = new_String(buf -> length);
	Memory_copy(*name,buf -> str,buf -> length);
	
	NameData* data = search_from_ordinary_namespace(*name, ScopeController_get_current_scope(controller));
	if(data != NULL)
		return data -> tp;
	return NULL;
}

Lvar *find_lvar(char *name,int length,Lvar **locals) {
	for(Lvar *var = *locals; var;var = var -> next)
	{
		if( var -> length == length && String_compare( name, var ->name,length) && ScopeInfo_inscope(var -> scope))
		{
			return var; 
		}
	}
	return NULL;
}

// add variable to lvar and store ordinary name space. storage_class is initialized by SC_AUTO
Lvar *new_lvar(Type *tp,char *name, int length,Lvar *next) {

	Lvar *lvar = calloc(1,sizeof(Lvar));
	lvar -> next = next;
	// identifier name copy
	lvar -> name = new_String(length);
	Memory_copy(lvar -> name,name,length);

	lvar -> length = length;
	lvar -> tp = tp;
	lvar -> storage_class = SC_AUTO;

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

#if defined Min
	#define Is_type_integer(t) INTEGER_TYPE_START == Min(t,INTEGER_TYPE_START) ? t <= INTEGER_TYPE_END ? 1: t == TP_ENUM? 1: 0: 0 
#endif
#define Is_type_pointer(t) (t == TP_POINTER || t == TP_ARRAY? 1: 0)
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
	if((*token) -> kind > TK_DECLARATION_SPECIFIER_START)
		return 1;
	if(is_type_alias(token))
	{
		return 1;
	}
	return 0;
}

Lvar *declare_ident(Type *tp, char *name,int len ,Lvar **table) {
	Lvar *lvar = new_lvar(tp,name,len,*table);
	*table = lvar;
	return lvar;
}

Lvar *declare_glIdent(Type *tp,char *name, int len, Lvar **table) {
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
Node_t* arrmemaccess(Token_t **token , Node_t** prev) {
	expect('[',token);
	Node_t *node = expr(token);
	expect(']',token);
	
	/**
	 * a[expr] is a syntax sugar of *(a + expr) 
	 * so "10[a]" for a being some pointer type is ok.
	 * 
	 */
	if(
		(Is_type_pointer(node -> tp -> Type_label) && Is_type_integer((*prev) -> tp -> Type_label)) ||
		(Is_type_integer(node -> tp -> Type_label) && Is_type_pointer((*prev) -> tp -> Type_label))
	){
		Node_t *get_address = new_node(ND_ADD,*prev,node, (*token) -> str);
		return new_Node_t(ND_DEREF,get_address,NULL,0,0,get_address -> tp -> pointer_to,NULL);
	}

	error( "type : %d\n", (*prev) -> tp -> Type_label);
	error_at((*token) -> str,"lval is expected");
}

//Node_t making function with type check typically used in parsing formula
Node_t *new_node( Node_kind kind,Node_t *l,Node_t *r, char *parsing_here) {
	Node_t *node = new_Node_t(kind,l,r,0,0,NULL,NULL);
	//type check
	if(typecheck(node) == 0)
	{
		error_at(parsing_here,"Type cast error: kind %d", kind);
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
		error_at(parsing_here,"Type cast error: kind: %d", kind);
	}
}

Node_t *new_node_function_call(Token_t **token) {
	char* name = new_String((*token) -> length);
	Memory_copy(name, (*token) -> str, (*token) -> length);
	NameData* data = search_from_ordinary_namespace(name, ScopeController_get_current_scope(controller));
	if(data == NULL || data -> tag != TAG_FUNCTION)
	{
		if(data == NULL)
			error_at((*token) -> str, "this is not exist in the ordinary name space: %s", name);
		else
			error_at((*token) -> str, "%s is not function", name);
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
		Node_t *node_right_end = new_Node_t(0,NULL,NULL,0,0,NULL,NULL);
		node_end ->left = conditional(token);
		node_end ->right = node_right_end;
		node_end = node_right_end;
		find(',',token);
	}
	node_end -> kind = ND_BLOCKEND;
	return node;
}

// to make a scope of function global, we update scope after parse function name. 
Node_t *new_node_function_definition(Token_t **token) {
	Node_t *node = new_Node_t(ND_FUNCTIONDEF,NULL,NULL,0,0,NULL,NULL);
	node = declare_specify(token, node, is_type_alias(token));
	node = ident_specify(token, node); 

	NameData* data = new_NameData(TAG_FUNCTION);
	data -> tp = node -> tp;
	Map_add(ordinaryNameSpace, node -> name, data);
	
	expect('(',token);	
	ScopeController_nest_appeared(controller); // start function nest
	node -> val = type_parameter_list(token,&node -> left);
	
	node -> right = stmt(token);

	ScopeController_nest_disappeared(controller);
	return node;

}

Node_t *new_node_var(Token_t **token) {
	Lvar *lvar = NULL;
	Lvar *table = Vector_get_tail(nameTable);
	Token_t *ident = consume(token);
	if(ident != NULL)
	{
		char* name = NULL;
		name = new_String(ident -> length);
		Memory_copy(name, ident -> str, ident -> length);

		NameData* data = search_from_ordinary_namespace(name, ScopeController_get_current_scope(controller));
		if(data != NULL && data -> tag == TAG_ENUMCONSTANT)
		{
			return new_node_num(data -> val);
		}
		if(data != NULL && data -> tag != TAG_OBJECT)
			error_at(ident -> str, "%s is already defined as not object name", name);
		lvar = find_lvar(ident -> str,ident -> length,&table);
	}
	else
	{
		error("Fail to consume token\n");
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
	ScopeController_nest_appeared(controller);

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
	Node_t* node = new_Node_t(
			ND_IF,
			condition,
			statement,
			0,0,NULL,NULL);
	ScopeController_nest_disappeared(controller);
	return node;
}

Node_t *new_node_while(Token_t **token) {
	ScopeController_nest_appeared(controller);

	Node_t *condition = expr(token);
	Node_t *statement = stmt(token);
	Node_t* node = new_Node_t(ND_WHILE,condition,statement,0,0,NULL,NULL);

	ScopeController_nest_disappeared(controller);
	return node;
}

Node_t* new_node_do_while(Token_t** token) {
	ScopeController_nest_appeared(controller);

	Node_t* statement = stmt(token);
	if((*token) -> kind != TK_WHILE) {
		error_at((*token) -> str, "while was expected");
	}
	consume(token);
	Node_t* condition = expr(token);
	Node_t* node = new_Node_t(ND_DO, condition, statement,0, 0, NULL, NULL);

	ScopeController_nest_disappeared(controller);
	return node;
}

Node_t *new_node_for(Token_t **token) {
	Node_t *init,*check,*update = NULL;
	expect('(',token);
	ScopeController_nest_appeared(controller);
	if(!find(';',token))
	{// this is not infinite loop
		if(is_lvardec(token))
			init = declare(token);
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
	Node_t* node = new_Node_t(
		ND_FOR,
		new_Node_t(
			ND_FORUPDATE,
			new_Node_t(
				ND_FORINITCONDITION,
				init,
				check,
				0,0,NULL,NULL
			),
			update,
			0,0,NULL,NULL
		),
		stmt(token),
		0,0,NULL,NULL
	);
	ScopeController_nest_disappeared(controller);
	return node;
}

Node_t *new_node_return(Token_t **token) {
	Node_t *node = new_Node_t(ND_RETURN, expr(token),NULL,0,0,NULL,NULL);
	expect(';',token);
	return node;
}

// helper function for new_node_switch
int parse_case_default(int depth, Token_t** token, Node_t** _caseLabel, Node_t** _caseStatement) {
	Node_t* caseLabel = *_caseLabel;
	Node_t* caseStatement = *_caseStatement;

	caseLabel -> left = primary(token);
	expect(':', token);

	Node_t* statements = new_Node_t(ND_BLOCK, stmt(token), NULL, 0, 0, 0, 0);
	caseStatement -> left = statements;

	statements -> right = new_Node_t(ND_BLOCK, NULL, NULL, 0, 0, NULL, NULL);
	statements = statements -> right;
	while((*token) -> kind != TK_CASE && (*token) -> kind != TK_DEFAULT && ((*token) -> str)[0] != '}')
	{
		statements -> left = stmt(token);

		statements -> right = new_Node_t(ND_BLOCK, NULL, NULL, 0, 0, NULL, NULL);
		statements = statements -> right;
	}
	statements -> kind = ND_BLOCKEND;

	caseLabel -> right = new_Node_t(ND_CASE, NULL, NULL, depth, 0, NULL, NULL);
	caseStatement -> right = new_Node_t(ND_BLOCK, NULL, NULL, depth, 0, NULL, NULL);

	// return
	*_caseLabel = caseLabel -> right;
	*_caseStatement = caseStatement -> right;
}

Node_t* new_node_switch(Token_t** token) {

	Node_t* node = new_Node_t(ND_SWITCH, NULL, NULL, 0, 0, NULL, NULL);
	expect('(', token);
	node -> left = expr(token);
	expect(')', token);

	Node_t* branch = new_Node_t(ND_BLOCK, NULL, NULL, 0, 0, NULL, NULL);
	node -> right = branch;

	int numberOfCase = 0;
	Node_t* caseLabel = new_Node_t(ND_CASE, NULL, NULL, 0, 0, NULL, NULL);
	Node_t* caseStatement = new_Node_t(ND_BLOCK, NULL, NULL, 0, 0, NULL, NULL);

	Node_t** _caseLabel = &caseLabel; // helper pointer
	Node_t** _caseStatement = &caseStatement; // helper pointer

	// val is depth: initial = 0
	branch -> left = caseLabel;
	branch -> right = caseStatement;

	// case start:
	expect('{', token);
	while(!find('}', token))
	{
		if((*token) -> kind == TK_CASE)
		{
			consume(token);

			numberOfCase ++;
			parse_case_default(numberOfCase, token, _caseLabel, _caseStatement);

			continue;
		}

		if((*token) -> kind == TK_DEFAULT)
		{
			consume(token);
			(*_caseLabel) -> kind = ND_DEFAULT;
			(*_caseStatement) -> kind = ND_DEFAULT;
			numberOfCase ++;
			parse_case_default(numberOfCase, token, _caseLabel, _caseStatement);

			expect('}', token);
			break;
		}

		error_at((*token) -> str, "Can't find case or default label");
	}
	(*_caseLabel) -> kind = ND_BLOCKEND;
	(*_caseStatement) -> kind = ND_BLOCKEND;
	node -> val = numberOfCase;

	return node;
}

Node_t *new_node_flow_operation(Token_kind kind,Token_t **token) {
	switch(kind) {
	case TK_IF: return new_node_if(token);
	case TK_DO: return new_node_do_while(token);			
	case TK_WHILE: return new_node_while(token);
	case TK_FOR: return new_node_for(token);
	case TK_RETURN: return new_node_return(token);
	case TK_SWITCH: return new_node_switch(token);
	case TK_ELSE: // ERROR
		error_at((*token) -> str, "'else' follows after 'if' or 'else if' statement");
	case TK_CONTINUE:
		return new_Node_t(ND_CONTINUE, NULL, NULL, 0, 0, NULL, NULL);
	case TK_BREAK:
		return new_Node_t(ND_BREAK, NULL, NULL, 0, 0, NULL, NULL);
	default:
		return NULL;
	}
}

Node_t *new_node_num(int val) {
	Node_t *node = new_Node_t(ND_NUM,NULL,NULL,val,0,new_tp(TP_INT,NULL,SIZEOF_INT),NULL);
	return node;
}



Node_t *new_node_string_literal(Token_t ** token) {
	Node_t *node = new_Node_t(ND_STRINGLITERAL,NULL,NULL,0,0,new_tp(TP_POINTER,new_tp(TP_CHAR,NULL,1),SIZEOF_POINTER),NULL);
	
	Lvar *literal = declare_ident(node -> tp,(*token) -> str,(*token) -> length,&string_literal);
	
	node -> name = new_String(literal -> length);
    Memory_copy(node -> name,literal -> name,literal -> length);
	node -> offset = literal -> offset;
	
	consume(token);
	expect('\"',token);
	return node;
}

Node_t *new_node_glob_ident(Token_t**token) {

	if(is_functioncall(token))
	{
		return new_node_function_definition(token);
	}

	if((*token) -> kind == TK_TYPEDEF) {
		Node_t* node = new_Node_t(ND_LVAL, NULL, NULL, 0, 0, NULL, NULL);
		consume(token);
		node = new_node_set_type_alias(token, node);
		expect(';', token);
		return node;
	}

	// global variable declaration or struct-union declaration 
	Node_t *node = new_Node_t(ND_GLOBVALDEF,NULL,NULL,0,0,NULL,NULL);
	node = declare_specify(token, node, is_type_alias(token));
	node = ident_specify(token, node);

	if(// struct , union or enum declaration 
		(node -> tp -> Type_label == TP_STRUCT || node -> tp -> Type_label == TP_UNION || node -> tp -> Type_label == TP_ENUM)
		&& node -> name == NULL
	) { 
		expect(';', token);
		node -> kind = ND_LVAL;
		return node;
	}

	// global variable declaration 
	Lvar *lvar = find_lvar(node -> name, String_len(node -> name), &global);
	if(lvar == NULL) {
		lvar = declare_glIdent(node -> tp,node -> name, String_len(node -> name),&global);
		set_tag_obj_to_ordinary_namespace(lvar -> name);
	}
	if (find('=',token))// initialization
	{
		if(node -> tp -> Type_label == TP_STRUCT || node -> tp -> Type_label == TP_ARRAY) {
			node = init(token, node);
		} else {
			node -> val = expr(token) -> val;
		}
	}

	lvar -> storage_class = node -> storage_class;
	expect(';',token);
	return node;
	
}

Node_t *new_node_ref_deref(Token_t **token) {

	Node_t *node = NULL;
	switch ((*token)->str[0])
		{
		case '*':

			(*token) = (*token) -> next;

			node = new_Node_t(ND_DEREF, postfix(token), NULL,0,0,NULL,NULL);
			if( node -> left -> tp -> Type_label == TP_ARRAY)
			{
				node -> tp = node -> left -> tp -> pointer_to;
				return node;
			}
			if( node -> left -> tp -> Type_label != TP_POINTER )
			{
				error_at((*token)->str, "Cannot access this variable\n");
			}
			else
			{
				node -> tp = node -> left -> tp -> pointer_to;
				return node;
			}
		
		case '&':

			(*token) = (*token) -> next;

			node = new_Node_t(ND_ADDR,postfix(token), NULL,0,0,NULL,NULL);
			node -> tp = new_tp(TP_POINTER,node -> left -> tp,SIZEOF_POINTER);
			return node;
		}
	return node;
}

// assume typedef keyword is consumed.
Node_t* new_node_set_type_alias(Token_t** token, Node_t* node) {
	// type qualifier?
	node = declare_specify(token, node, 0);
	node = ident_specify(token, node);

	// set to ordinary name space
	NameData* data = new_NameData(TAG_TYPEDEF);
	data -> tp = node -> tp;
	Map_add(ordinaryNameSpace, node -> name, data);
	return node;
}


/*
 * generate ast from token list 
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
 * implementation
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
	return new_node_glob_ident(token);
}


// parameter parsing
int type_parameter_list(Token_t** token, Node_t** parameter_container) {
	return parameter_list(token, parameter_container);
}

// parse parameters to tree and count number of parameters for return.
// after this function being called, parameter_list points head of list of nodes which has parameter info.
int parameter_list(Token_t** token, Node_t** parameter_container) {
	int number_of_parameters = 0;
	Node_t* param_tree = new_Node_t(ND_ARGMENT,NULL,NULL,0,0,NULL,NULL);
	*parameter_container = param_tree;
	while (!find(')',token))
	{
		param_tree -> left = parameter_declare(token, number_of_parameters);
		find(',',token);

		Node_t *next = new_Node_t(ND_ARGMENT,NULL,NULL,0,0,NULL,NULL);
		param_tree -> right = next;
		param_tree = next;
		if((*token) -> kind == TK_PLACE_HOLDER) {
			// quit parsing parameters
			consume(token);
			expect(')', token);
			break;
		}
		number_of_parameters ++;
	}

	param_tree -> kind = ND_BLOCKEND;
	return number_of_parameters;
}

// parse parameter.
// number_of_parameter is used for unamed parameter
Node_t* parameter_declare(Token_t** token, int number_of_parameter) {
	ScopeInfo* current_scope = ScopeController_get_current_scope(controller);
	Lvar *table = Vector_get_tail(nameTable);
	Node_t* node = new_Node_t(ND_LVAL, NULL, NULL, 0, 0, NULL, NULL);

	node = declare_specify(token, node, is_type_alias(token));
	node = ident_specify(token, node);
	

	// parse pointer argment only 

	if((node -> tp -> Type_label == TP_STRUCT || node -> tp -> Type_label == TP_UNION || node -> tp -> Type_label == TP_ENUM) 
				&& node -> name == NULL)
	{//  struct definition only
		return node;
	}

	// check: scope && name space 
	if(node -> name == NULL) {
		node -> name = String_add("parameter_",i2a(number_of_parameter));
	}
	Lvar* lvar = find_lvar(node -> name, String_len(node -> name), &table); //
	if(lvar == NULL || !ScopeInfo_equal(current_scope, lvar -> scope)) {
		lvar = declare_ident(node -> tp, node -> name,String_len(node -> name),&table);
		set_tag_obj_to_ordinary_namespace(lvar -> name);
	}
	else
		error_at((*token) -> str, "Can't use same identifier in SameScope: %s", node -> name);

	node -> offset = lvar -> offset;
	Vector_replace(nameTable,Vector_get_length(nameTable)-1,table);

	return node;
}

Node_t *stmt(Token_t **token) {

	Node_t *node = NULL;

	if( (*token) -> kind > TOKEN_FLOW_OPERATION_START - 1 && (*token) -> kind < TK_FLOW_OPERATION_END)
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
		node = declare(token);
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

Node_t *declare(Token_t **token) {
	ScopeInfo* current_Scope = ScopeController_get_current_scope(controller);
	Lvar *table = Vector_get_tail(nameTable);
	Node_t* node = new_Node_t(ND_LVAL, NULL, NULL, 0, 0, NULL, NULL);
	int isTypeDef = (*token) -> kind == TK_TYPEDEF;
	int isTypeAlias = (*token) -> kind == TK_IDENT? is_type_alias(token): 0;

	do{
		node = declare_specify(token, node, isTypeAlias);
		isTypeAlias = (*token) -> kind == TK_IDENT? is_type_alias(token): 0;
	}while(((*token) -> kind > TK_DECLARATION_SPECIFIER_START && (*token) -> kind < TK_DECLARATION_SPECIFIER_END) || isTypeAlias);


	if((node -> tp -> Type_label != TP_STRUCT && node -> tp -> Type_label != TP_UNION && node -> tp -> Type_label != TP_ENUM) 
			|| (*token) -> kind == TK_IDENT 
			|| (*token) -> kind == TK_OPERATOR)
	{
		node = ident_specify(token, node);
	}
	if((node -> tp -> Type_label == TP_STRUCT || node -> tp -> Type_label == TP_UNION || node -> tp -> Type_label == TP_ENUM) 
				&& (node -> name == NULL || (isTypeDef && (*token) -> kind == TK_PUNCTUATOR)))
	{//  struct definition only
		return node;
	}

	// check: scope && name space 
	Lvar* lvar = find_lvar(node -> name, String_len(node -> name), &table);
	if(lvar == NULL || !ScopeInfo_equal(current_Scope, lvar -> scope)) {
		lvar = declare_ident(node -> tp, node -> name,String_len(node -> name),&table);
		lvar -> storage_class = node -> storage_class;
		set_tag_obj_to_ordinary_namespace(lvar -> name);
	} else if(lvar -> storage_class != SC_AUTO) {
		// just skip
	} else 
		error_at((*token) -> str, "Can't use same identifier in SameScope: %s", node -> name);

	node -> offset = lvar -> offset;
	Vector_replace(nameTable,Vector_get_length(nameTable)-1,table);

	if(find('=',token))
	{ // initialize
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
		node -> scope = ScopeInfo_copy(ScopeController_get_current_scope(controller));
		return node;
	}
	return assign(token);
}

Node_t* init_list(Token_t** token, Node_t* node) {
	if(node -> tp -> Type_label == TP_STRUCT)
	{
		StructData* data = search_from_tag_namespece(node -> tp ->name, ScopeController_get_current_scope(controller));
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

// specify identifier : is it an pointer to something ? identifier name?
// this function does not guarantee that node -> name != NULL
Node_t* ident_specify(Token_t** token, Node_t* node) {
	if((*token) -> kind == TK_OPERATOR)
	{
		node = pointer(token, node);
	}
	if((*token) -> kind == TK_IDENT) {
		node -> name = expect_ident(token);
	}
	while(find('[', token))
	{
		int array_size = expect_num(token) * (node -> tp -> size);
		expect(']', token);
		node -> tp = new_tp(TP_ARRAY, node -> tp, array_size);
	}
	return node;
}
/*@brief specify declaration type struct static or extern?
 * */
Node_t* declare_specify(Token_t** token, Node_t* node, int isTypeAlias) {
	int is_typedef = (*token) -> kind == TK_TYPEDEF? 1: 0;
	if((*token) -> kind >= TOKEN_TYPE && (*token) -> kind < TK_TYPEEND)
		return type_specify(token, node);
		
	if((*token) -> kind == TK_TYPEDEF || is_typedef) {
		consume(token);
		return new_node_set_type_alias(token, node);
	}
	if(isTypeAlias)
	{
		char* name = expect_ident(token);
		NameData* data = search_from_ordinary_namespace(name, ScopeController_get_current_scope(controller));
		node -> tp = data -> tp;
		return node;
	}
	if((*token) -> kind == TK_STATIC) {
		if(node -> storage_class != SC_AUTO) {
			// static or extern can't declare twice.
			error_at((*token) -> str, "static or extern can't declare twice.");
		}
		consume(token);
		node -> storage_class = SC_STATIC;
		int ita = (*token) -> kind == TK_IDENT? is_type_alias(token): 0; 
		return declare_specify(token,node, ita);
	}
	if((*token) -> kind == TK_EXTERN) {
		if(node -> storage_class != SC_AUTO) {
			// static or extern can't declare twice.
			error_at((*token) -> str, "static or extern can't declare twice.");
		}
		consume(token);
		node -> storage_class = SC_EXTERN;
		int ita = (*token) -> kind == TK_IDENT? is_type_alias(token): 0;
		return declare_specify(token, node, ita);
	}

	error_at((*token) -> str, "type name, storage class specifier or type qualifier was expected");
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
			Type* tp = new_tp(TP_POINTER, node -> tp, SIZEOF_POINTER);
			node -> tp = tp;
			continue;
		}
		break;
	}
	return node;
}
/*
 * @brief parse type and struct declaration. change assigned node -> tp only. 
 * */
Node_t *type_specify(Token_t** token, Node_t* node) {
	if((*token) -> kind >= TOKEN_TYPE && (*token) -> kind < TK_STRUCT)
	{	
		node -> tp = (*token) -> tp;
		*token = (*token) -> next;
		return node;
	}
	if((*token) -> kind == TK_ENUM)
	{
		node -> tp = new_tp(TP_ENUM, NULL, SIZEOF_INT);
		consume(token);
		return enum_specify(token, node);		
	}
	if((*token) -> kind == TK_STRUCT || (*token) -> kind == TK_UNION)
	{
		node -> tp = consume(token) -> kind == TK_STRUCT? new_tp(TP_STRUCT, NULL, 0): new_tp(TP_UNION, NULL, 0);
		return struct_union_specify(token, node);
	}
	if(is_type_alias(token)) {
		// check ordinary name space
		char* name = new_String((*token) -> length);
		Memory_copy(name, (*token) -> str, (*token) -> length);
		NameData* data = search_from_ordinary_namespace(name, ScopeController_get_current_scope(controller));
		node -> tp = data -> tp;
		consume(token);
		return node;
	}

	error_at((*token) -> str, "Type keyword is expected");
}

/*
 * @brief Doesn't change node address
 * */
Node_t* struct_union_specify(Token_t** token, Node_t* node) {
	char* name = NULL; 
	if((*token) -> kind == TK_IDENT)
	{
		name = new_String((*token) -> length);
		Memory_copy(name, (*token) -> str, (*token) -> length);
		consume(token);
	}
	else
	{
		name = String_add("_", i2a(struct_number++));
	}
	node -> tp -> name = name;
	int tag = node -> tp -> Type_label == TP_STRUCT? TAG_STRUCT: TAG_UNION;
	StructData* structData = search_from_tag_namespece(name, ScopeController_get_current_scope(controller));
	if(structData == NULL)
	{// declare struct or union
		structData = make_StructData();
		structData -> tag = node -> tp -> Type_label == TP_STRUCT? TAG_STRUCT: TAG_UNION;
		structData -> scope = ScopeInfo_copy(ScopeController_get_current_scope(controller));
		Map_add(tagNameSpace, name, structData);
	}
	if(structData -> tag != tag)
	{
		error_at((*token) -> str, "%s is defined as wong kind of tag", name);
	}
	

	if(find('{', token))
	{
		while(!find('}', token))
		{
			struct_declare(token, node);
		}
	}
	if(node -> tp -> Type_label == TP_UNION)
	{
		StructData* data = Map_at(tagNameSpace, node -> tp -> name);
		node -> tp -> size = data -> size;
		return node;
	}

	//calculate alignment size when struct
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
Node_t* struct_declare(Token_t** token, Node_t* node) {
	node = struct_declare_inside(token, node);
	while(find(',', token))
	{
		node = struct_declare_inside(token, node);
	}
	expect(';', token);
	return node;
}
/*
 * @brief Doesn't change node address
 * */
Node_t* struct_declare_inside(Token_t** token, Node_t* node) {
	StructData* data = search_from_tag_namespece(node -> tp -> name, ScopeController_get_current_scope(controller));
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

	if(node -> tp -> Type_label == TP_UNION)
	{
		StructData_add(data, member);
		return node;
	}
	
	if(previousMember != NULL)
	{// offset calculation when struct
		Node_t* prev = Map_at(data -> memberContainer, previousMember);
		unsigned threshold = prev -> offset + prev -> tp -> size + (SIZEOF_POINTER - ((prev -> offset + prev -> tp -> size) % SIZEOF_POINTER));
		if(prev -> offset + prev -> tp -> size + member -> tp -> size <= threshold)
		{
			member -> offset = prev -> offset + prev -> tp -> size;
		}
		else
		{
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

Node_t* enum_specify(Token_t** token, Node_t* node) {
	char* name = NULL;
	if((*token) -> kind == TK_IDENT)
	{
		Token_t* buf = consume(token);
		name = new_String(buf -> length);
		Memory_copy(name, buf -> str, buf -> length);
	}
	else
	{
		name = String_add("_", i2a(struct_number++));
	}
	node -> name = name;

	StructData* data = search_from_tag_namespece(name, ScopeController_get_current_scope(controller));
	if(data == NULL)
	{
		data = make_StructData();
		data -> tag = TAG_ENUM;
		data -> scope = ScopeInfo_copy(ScopeController_get_current_scope(controller));
		Map_add(tagNameSpace, name, data);
	}
	else if(data -> tag != TAG_ENUM)
	{
		error_at((*token) -> str, "%s is defined by wong tag name", name);
	}
	if(find('{', token))
	{
		node = enum_list(token, node);
		expect('}', token);
		return node;
	}
	return node;
}

Node_t* enum_list(Token_t** token, Node_t* node) {
	node = enumerator(token, node);
	while(find(',', token))
	{
		node = enumerator(token, node);
	}
	return node;
}

Node_t* enumerator(Token_t** token, Node_t* node) {
	if((*token) -> kind == TK_IDENT)
	{
		StructData* data = Map_at(tagNameSpace, node -> name);
		char* prevName = Vector_get_tail(data -> memberNames);
		Node_t* member = NULL;
		if(prevName != NULL)
			member = Map_at(data -> memberContainer, prevName);
		parsing_here = (*token) -> str;

		char* name = expect_ident(token);
		NameData* nameData = search_from_ordinary_namespace(name, ScopeController_get_current_scope(controller));
		if(nameData != NULL)
			error_at(parsing_here, "%s is defined as wong identifier name", name);
		
		nameData = new_NameData(TAG_ENUMCONSTANT);
		Map_add(ordinaryNameSpace, name, nameData);

		if(find('=', token))
		{ // allow minus
			if((*token) -> kind == TK_OPERATOR && (*token) -> str[0] == '-') {
				consume(token);
				nameData -> val = - expect_num(token);
			} else {
				nameData -> val = expect_num(token);
			}
		}
		else
		{
			nameData -> val = member != NULL?member -> val + 1: 0;
		}
		StructData_add(data, new_Node_t(ND_LVAL, NULL, NULL, nameData -> val, 0, NULL, name));
		
	}
	return node;
}

Node_t *expr(Token_t** token) {
	return assign(token);
}
Node_t *assign(Token_t **token) {
	char* parsing_here = (*token) -> str;// for error detection
	Node_t *node = conditional(token);
	
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

Node_t* conditional(Token_t** token) {
	Node_t* node = log_or(token);

	if(find('?', token)) {
		node = new_Node_t(ND_CONDITIONAL, node, NULL, 0, 0, NULL, NULL);
		Node_t* exprs = new_Node_t(ND_CONDITIONAL_EXPRS, NULL, NULL, 0, 0, NULL, NULL);
		node -> right = exprs;

		exprs -> left = expr(token);
		expect(':',token);
		exprs -> right = conditional(token);

		if(exprs -> left -> tp -> size >= exprs -> right -> tp -> size) {
			exprs -> tp = exprs -> left -> tp;
		} else {
			exprs -> tp = exprs -> right -> tp;
		}
		node -> tp = exprs -> tp;
	}
	return node;
}

Node_t* log_or(Token_t **token) {
	Node_t *node = log_and(token);
	if(find(LOG_OR, token))
	{
		node = new_Node_t(ND_LOGOR, node, log_or(token), 0, 0, new_tp(TP_INT, NULL, SIZEOF_INT), NULL);
	}
	return node;
}

Node_t* log_and(Token_t **token) {
	Node_t *node = equality(token);
	if(find(LOG_AND, token))
	{
		node = new_Node_t(ND_LOGAND, node, log_and(token), 0, 0, new_tp(TP_INT, NULL, SIZEOF_INT), NULL);
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
	Node_t *node = cast(token, NULL);
	for(;;)
	{
		parsing_here = (*token) -> str;
		if(find('*',token))
		{
			node = new_node(ND_MUL,node,cast(token, NULL), (*token) -> str);
		}
		else if(find('/',token))
		{
			node = new_node(ND_DIV,node,cast(token, NULL), (*token) -> str);
		}
		else
		{
			return node;
		}
	}
}

Node_t* cast(Token_t** token, Node_t* node) {
	if(is_cast(token)) {
		Node_t* node_tp = new_Node_t(ND_LVAL, NULL, NULL, 0, 0, NULL, NULL);
		expect('(', token); // consume '('
		node_tp = type_name(token, node_tp);
		expect(')', token);
		
		node = cast(token, node);
		if(node == NULL) {
			error_at((*token) -> str, "failed to parse.");
		}
		node -> tp = node_tp -> tp;
		return node;
	}
	return unitary(token);
}

// assume '(' is consumed
Node_t* type_name (Token_t** token, Node_t* node) {
	node = type_specify(token, node);
	node = pointer(token, node);
	return node;
}



Node_t *unitary(Token_t **token) {
	Node_t *node = NULL;
	if((*token)-> kind == TK_SIZEOF)
	{
		Node_t* node = new_Node_t(ND_LVAL, NULL, NULL, 0, 0, NULL, NULL); // for get type

		consume(token);
		if(is_cast(token)) {
			expect('(', token);
			node = type_name(token, node);
			expect(')', token);
		}
		else {
			node = unitary(token);
		}
		
		// if(node -> tp -> Type_label == TP_STRUCT)
		// {
		// 	StructData *data = Map_at(tagNameSpace, node -> tp -> name);
		// 	return new_node_num(data -> size);
		// }
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
		node = postfix(token);
		return node;
		
	}
	else if( find('-',token) )
	{
		node = new_node(ND_SUB,new_node_num(0),postfix(token), (*token) -> str);
		node -> tp = node -> right -> tp;
		return node;
	}
	else if((*token) -> kind == TK_OPERATOR && ( (*token) -> str[0] == '*' || (*token) -> str[0] == '&'))
	{
		node = new_node_ref_deref(token);
		return node;
	}
	else if(find('!', token))
	{
		node = new_Node_t(ND_LOGNOT, postfix(token), NULL, 0, 0, new_tp(TP_INT, NULL, SIZEOF_INT), NULL);
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
			if(node -> tp -> Type_label != TP_STRUCT && node -> tp -> Type_label != TP_UNION)
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
			if(node -> tp -> Type_label != TP_STRUCT && node -> tp -> Type_label != TP_UNION)
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
		node = new_node_string_literal(token);
	}
	else if((*token) -> kind == TK_CONST)
	{
		node = new_node_num(expect_num(token));
	}
	return node;
}

