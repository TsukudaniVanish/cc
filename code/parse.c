#include "cc.h"
//#include<stdlib.h>
//#inclide<string.h>






Type* new_tp(int label,Type* pointer_to,long int size){

	Type* tp = calloc(1,sizeof(Type));
	tp -> Type_label = label;
	tp -> pointer_to = pointer_to;
	tp -> size = size;
	return tp;
}

Type *read_type(char **name,Token_t **token)
{
	Token_t *buf = consume(token);

	if((*token) -> kind != TK_IDENT)
	{
		error_at((*token) -> str,"識別子名がありません");
	}

	if(*name)
		free(*name);
	*name = calloc((*token) -> length, sizeof(char));
	memcpy(*name,(*token) -> str, (*token) -> length);

	consume(token);

	if(find("[",token))
	{//配列定義
		int array_size = expect_num(token) * (buf -> tp -> size);
		expect("]",token);
		return new_tp(TP_ARRAY,buf -> tp,array_size);
	}
	return buf -> tp;

}

Type *Type_function_return(char **name,Token_t** token)
{
	Token_t *buf = consume(token);
	if(*name)
		free(*name);
	*name = calloc(buf -> length, sizeof(char));
	memcpy(*name,buf -> str,buf -> length);

	return find_lvar(*name,buf -> length,&global) -> tp;
}




Lvar *find_lvar(char *name,int length,Lvar **locals){//array は飛ばす
	

	for(Lvar *var = *locals; var;var = var -> next)
	{
		if( var -> length == length && !memcmp( name, var ->name,length))
		{
			return var; 
		}
	}
	return NULL;
}

Lvar *new_lvar(Type *tp,char *name, int length,Lvar *next){

	Lvar *lvar = calloc(1,sizeof(Lvar));
	lvar -> next = next;
	//名前コピー
	lvar -> name = calloc(length,sizeof(char));
	memcpy(lvar -> name,name,length);
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


Tables *new_Tables(Tables *head,Tables *next,Lvar *locals)
{
	Tables *table = calloc(1,sizeof(Tables));
	table -> head = head;
	table -> next = next;
	table -> locals = locals;
	return table;
}


void make_nametable(Tables **table)
{
	if(*table)
	{
		(*table) -> next = new_Tables((*table) -> head,NULL,NULL);
		(*table) = (*table) -> next;
		return;
	}
	*table = new_Tables(NULL,NULL,NULL);
	(*table) -> head = *table;
	return;
}






int typecheck(Node_t *node){

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


Type *imptypecast(Node_t *node)
{
	int tp_l , tp_r;

	switch (typecheck(node))
	{
	case 0:
		return NULL;
		break;

	case 1:
		return node -> left -> tp;
	
	default:
		tp_l = node -> left -> tp -> Type_label;
		tp_r = node -> right -> tp -> Type_label;
		break;
	}

	

	if(tp_l == TP_POINTER)
	{
		if(tp_r == TP_INT)
		{
			return node -> left -> tp;
		}
		return NULL;
	}
	else if(tp_r == TP_POINTER)
	{
		if(tp_l == TP_INT)
		{
			return node -> right -> tp;
		}
		return NULL;
	}
	if(tp_l < 10 && tp_r < 10)
	{
		return tp_l < tp_r ? node -> left -> tp  : node -> right -> tp;
	}

	
}

int is_lvardec(Token_t **token)
{
	if((*token) -> kind > 299 )
		return 1;
	return 0;
}

Lvar *declere_ident(Type *tp, char *name,int len ,Lvar **table)
{
	Lvar *lvar = find_lvar(name,len,table);
	lvar = new_lvar(tp,name,len,*table);
	*table = lvar;
	if(lvar -> tp -> Type_label == TP_ARRAY)
	{
		return declere_ident(new_tp(TP_POINTER,lvar -> tp -> pointer_to,8),lvar -> name, lvar -> length,table);
	}
	return lvar;
}

Lvar *declere_glIdent(Type *tp,char *name, int len, Lvar **table)
{
	Lvar *lvar = find_lvar(name,len,table);
	if(lvar && lvar -> tp -> Type_label != TP_ARRAY)
		return NULL;
	lvar = new_lvar(tp,name,len,*table);
	*table = lvar;
	return lvar;	
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
 * assign = equality ("=" assign )?
 * equality = relational("==" relational | "!=" relational)*
 * relational = add( "<=" add | "<" add | ">=" add | ">" add  )*
 * add = mul( "+"mul | "-"mul)* 
 * mul = unitary ("*" unitary | "/" unitary )*
 * unitary = "sizeof" unitary
 * 			| ('+' | '-' )? primary
 * 			| '*' unitary
 * 			| '&' unitary
 * primary = num 
 * 			| type? indent  ( "["add"]" )? 
 * 			| "(" assign ")"
 * 			| "\"" strring literal "\""
 *
 * 終端記号:
 * 		num
 * 		indent
 *
 *
 */

/*
 * 関数実装
 */




void program(Token_t **token,Node_t **code){
	

	int i = 0;
	while(!at_eof(token)){

		code[i] = func(token);
		i++;
	}
	code[i] = NULL;
	nametable -> next = NULL;
}

Node_t *func(Token_t **token){


	if ((*token) -> kind <= 299 ){

		error_at((*token) -> str , "型宣言がありません");

	}
	if( !( (*token) ->next ) && (*token) -> next -> kind != TK_IDENT ){


		error_at((*token) -> str,"関数名が必要です");
	}
	return new_node_globalident(token);
}

Node_t *stmt(Token_t **token){


	Node_t *node;

	if( (*token) -> kind > 99 && (*token) -> kind < 200 ){//if (else) while for return をパース


		node = new_node_keyword( (*token) -> kind,token);

	}else if( find("{",token) ){
		

		node = new_node_block(token);
	
	}
	else if(is_lvardec(token))
	{
		node = Lvardec(token);
		expect(";",token);
		if(node -> kind == ND_LVAL)
			return NULL;
		return node;
	}
	else{

		node = assign(token);
		expect(";",token);
	}
	
	return node;
}

Node_t *Lvardec(Token_t **token)
{
	char *name = NULL;
	Type *tp = read_type(&name,token);
	Lvar *lvar = declere_ident(tp,name,strlen(name),&(nametable -> locals));
	Node_t *node = new_Node_t(ND_LVAL,NULL,NULL,0,lvar -> offset,lvar -> tp,lvar -> name);
	if(find("=",token))
	{
		parsing_here = (*token) -> str;
		node = new_node(ND_ASSIGN,node,assign(token));
		return node;
	}
	return node;

}

Node_t *assign(Token_t **token){


	Node_t *node = equality(token);
	
	if( find("=",token) ){
		
		parsing_here = (*token) -> str;
		node = new_node(ND_ASSIGN,node,assign(token));
	}
	return node;
}


Node_t *equality(Token_t **token){


	Node_t *node = relational(token);

	for(;;){
		
		parsing_here = (*token) -> str;
		if( find("==",token) ){
		

			node = new_node(ND_EQL,node,relational(token));
		
		}else if( find("!=",token) ){

			
			node = new_node(ND_NEQ,node,relational(token));
		
		}else{


			return node;
		}
	}
}

Node_t *relational(Token_t **token){


	Node_t *node = add(token);

	for (;;){

		parsing_here = (*token) -> str;
		if( find("<=",token) ){
			
			
			node = new_node(ND_LEQ,node,add(token));
		
		}else if( find("<",token) ){
			
	
			node = new_node(ND_LES,node,add(token));
		
		}else if( find(">=",token) ){
	
	
			node = new_node(ND_LEQ,add(token),node);
	
		}else if( find(">",token) ){
		
	
			node = new_node(ND_LES,add(token),node);
	
		}else{
		
			
			return node;
		}
	}
}

Node_t *add(Token_t **token){


	Node_t *node = mul(token);

	for(;;){

		parsing_here = (*token) -> str;
		if( find("+",token) ){


			node = new_node(ND_ADD,node,mul(token));

		}else if( find("-",token) ){


			node = new_node(ND_SUB,node,mul(token));

		}else{


			return node;
		}
	}
}

Node_t *mul(Token_t **token){



	Node_t *node = unitary(token);

	for(;;){

		parsing_here = (*token) -> str;
		if( find("*",token) ){


			node = new_node(ND_MUL,node,unitary(token));

		}else if( find("/",token) ){


			node = new_node(ND_DIV,node,unitary(token));

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

	parsing_here = (*token) -> str;

	if( find("+",token) ){


		node = primary(token);
		
	}else if( find("-",token) ){


		node = new_node(ND_SUB,new_node_num(0),primary(token));
		node -> tp = calloc(1,sizeof(Type));
		node -> tp = node -> right -> tp;
		
	}else if((*token) -> kind == TK_OPERATOR && ( (*token) -> str[0] == '*' || (*token) -> str[0] == '&')){

		node = new_node_ref_deref(token);		
	
	}else{
		
		
		node = primary(token);
	}
	return node;
}

Node_t *primary(Token_t **token){

	Node_t *node;

	if( find("(",token) ){ // '(' の次は expr


		node = assign(token);
		expect(")",token);// ')'かcheck

	}else if( (*token)-> kind == TK_IDENT || ( (*token) -> kind >299) )
	{

		node = new_node_ident(token);
	}
	else if(find("\"",token))
	{
		node = new_node_stringiter(token);
	}
	else
	{
		node = new_node_num(expect_num(token));
	}
	if(find("[",token)){// a[...] -> *(a + ...)

		if( node -> tp -> Type_label !=TP_POINTER)
		{//識別子を読んでいるかチェック
			fprintf(stderr,"N kind : %d\n",node -> kind);
			fprintf(stderr,"Type label : %d\n",node -> tp -> Type_label);
			error_at((*token) -> str,"識別子がありません");
		}
		Node_t *node_top = calloc(1,sizeof(Node_t));
		node_top -> kind = ND_DEREF;
		if(node -> tp -> pointer_to -> Type_label == TP_ARRAY)
		{
			node_top -> tp = node -> tp -> pointer_to -> pointer_to;
		}
		else
		{
			node_top -> tp = node -> tp -> pointer_to;
		}
		node_top -> val = -1;

		parsing_here = (*token) -> str;
		node_top -> left = new_node(ND_ADD,node,mul(token));

		expect("]",token);

		return node_top;

	}
	return node;
}

