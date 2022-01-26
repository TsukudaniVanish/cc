#include "cc.h"
//#include<stdlib.h>
//#inclide<string.h>


extern unsigned int String_len(char*);
extern int String_conpair(char*,char*,unsigned int);
extern void Memory_copy(void*,void*,unsigned int);


Type* new_tp(int label,Type* pointer_to,long int size) {

	Type* tp = calloc(1,sizeof(Type));
	tp -> Type_label = label;
	tp -> pointer_to = pointer_to;
	tp -> size = size;
	return tp;
}

Type *read_type(char **name,Token_t **token) {
	Token_t *buf = consume(token);

	if((*token) -> kind != TK_IDENT)
	{
		error_at((*token) -> str,"Identifier was expected");
	}

	if(*name)
		free(*name);
	*name = calloc((*token) -> length, sizeof(char));
	Memory_copy(*name,(*token) -> str, (*token) -> length);

	consume(token);

	if(find('[',token))
	{//配列定義
		int array_size = expect_num(token) * (buf -> tp -> size);
		expect(']',token);
		return new_tp(TP_ARRAY,buf -> tp,array_size);
	}
	return buf -> tp;

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




Lvar *find_lvar(char *name,int length,Lvar **locals) {//array は飛ばす
	

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

	if( next )
	{
		lvar -> offset = next -> offset +(lvar -> tp -> size);
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
	if((*token) -> kind > 299 )
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
	Node_t *node = assign(token);
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






/*
 * token から構文木を生成 
 */
/*
 * 生成文法
 *
 * program = func*
 * func = ident "(" (type? ident)*  ")"  stmt 
 * stmt = assign";"
 * 		| lvardec ";"
 * 		| "{" stmt* "}"
 * 		| "if" "(" assign  ")" stmt ( "else" stmt  )?
 * 		| "while"  "(" assign ")" stmt
 * 		| "for"  "(" assign?; assign? ; assign? ")"stmt
 * 		| "return" assign ";"
 * lvardec = Type ident ( "[" num? "]" )? ( "=" assign )? 
 * assign = log_or ("=" assign )?
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
 * 			|( primary [assign] | primary '++' | primary '--')*
 * primary = num 
 * 			| indent 
 * 			| "(" assign ")"
 * 			| "\"" string literal "\""
 *
 * 終端記号:
 * 		num
 * 		indent
 * 		string literal
 */
/*
 * 関数実装
 */




void program(Token_t **token,Vector *codes){
	

	while(!at_eof(token)){

		Vector_push(nameTable,NULL);
		Vector_push(codes, func(token));
	}
	Vector_push(nameTable,NULL);
	//nametable -> next = NULL;
}

Node_t *func(Token_t **token){

	if ((*token) -> kind <= TOKEN_TYPE - 1){

		error_at((*token) -> str , "型宣言がありません");

	}
	if( !( (*token) ->next ) && (*token) -> next -> kind != TK_IDENT ){


		error_at((*token) -> str,"関数名が必要です");
	}
	return new_node_glob_ident(token);
}

Node_t *stmt(Token_t **token){


	Node_t *node;

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
		node = Lvardec(token);
		expect(';',token);
		if(node -> kind == ND_LVAL)
			return NULL;
		return node;
	}
	else
	{
		node = assign(token);
		expect(';',token);
	}
	
	return node;
}

Node_t *Lvardec(Token_t **token)
{
	Lvar *table = Vector_get_tail(nameTable);
	char *name = NULL;
	Type *tp = read_type(&name,token);
	Lvar *lvar = declere_ident(tp,name,String_len(name),&table);
	Vector_replace(nameTable,Vector_get_length(nameTable)-1,table);
	Node_t *node = new_Node_t(ND_LVAL,NULL,NULL,0,lvar -> offset,lvar -> tp,lvar -> name);
	if(find('=',token))
	{
		parsing_here = (*token) -> str;
		node = new_node(ND_ASSIGN,node,assign(token), (*token) -> str);
		return node;
	}
	return node;

}

Node_t *assign(Token_t **token){


	Node_t *node = log_or(token);
	
	if( find('=',token) ){
		
		parsing_here = (*token) -> str;
		node = new_node(ND_ASSIGN,node,assign(token), (*token) -> str);
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

	for(;;){
		
		parsing_here = (*token) -> str;
		if( find(EQUAL,token) ){
		

			node = new_node(ND_EQL,node,relational(token), (*token) -> str);
		
		}else if( find(NEQ,token) ){

			
			node = new_node(ND_NEQ,node,relational(token), (*token) -> str);
		
		}else{


			return node;
		}
	}
}

Node_t *relational(Token_t **token){


	Node_t *node = add(token);

	for (;;){

		parsing_here = (*token) -> str;
		if( find(LEQ,token) ){
			
			
			node = new_node(ND_LEQ,node,add(token), (*token) -> str);
		
		}else if( find('<',token) ){
			
	
			node = new_node(ND_LES,node,add(token), (*token) -> str);
		
		}else if( find(GEQ,token) ){
	
	
			node = new_node(ND_LEQ,add(token),node, (*token) -> str);
	
		}else if( find('>',token) ){
		
	
			node = new_node(ND_LES,add(token),node, (*token) -> str);
	
		}else{
		
			
			return node;
		}
	}
}

Node_t *add(Token_t **token){


	Node_t *node = mul(token);

	for(;;){

		parsing_here = (*token) -> str;
		if( find('+',token) ){


			node = new_node(ND_ADD,node,mul(token), (*token) -> str);

		}else if( find('-',token) ){


			node = new_node(ND_SUB,node,mul(token), (*token) -> str);

		}else{


			return node;
		}
	}
}

Node_t *mul(Token_t **token){



	Node_t *node = unitary(token);

	for(;;){

		parsing_here = (*token) -> str;
		if( find('*',token) ){


			node = new_node(ND_MUL,node,unitary(token), (*token) -> str);

		}else if( find('/',token) ){


			node = new_node(ND_DIV,node,unitary(token), (*token) -> str);

		}else{


			return node;
		}
	}
}



Node_t *unitary(Token_t **token){

	Node_t *node;
	if( (*token)-> kind == TK_SIZEOF ){


		(*token) = (*token) -> next;
		node = unitary(token);
		if( node -> val != 0 ){
			

			node = new_node_num(  node -> val );
		}else{

			
			node = new_node_num(  node ->tp -> size );
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

	if( find('+',token) ){

		node = unitary(token);
		return node;
		
	}else if( find('-',token) ){

		node = new_node(ND_SUB,new_node_num(0),unitary(token), (*token) -> str);
		node -> tp = node -> right -> tp;
		return node;
		
	}else if((*token) -> kind == TK_OPERATOR && ( (*token) -> str[0] == '*' || (*token) -> str[0] == '&')){

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

Node_t *primary(Token_t **token){

	parsing_here = (*token) -> str;
	Node_t *node = NULL;

	if( find('(',token) ){ // '(' の次は expr


		node = assign(token);
		expect(')',token);// ')'かcheck

	}else if( (*token)-> kind == TK_IDENT || ( (*token) -> kind >299) )
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

