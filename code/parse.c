#include "cc.h"
//#include<stdlib.h>
//#inclide<string.h>




bool find(char *string,Token_t **token){

	if( (*token) -> kind > 100 | strlen(string) != (*token) -> length | memcmp( (*token) -> str,string,(*token)-> length ) != 0  ){


		return false;

	}else{


		*token = (*token) -> next;
		return true;
	}
}




void expect(char *string ,Token_t **token){


	if( (*token) -> kind > 100 | (*token) -> length != strlen(string) | memcmp( (*token) -> str,string,(*token) -> length ) ){
		
		error_at( (*token) -> str ,"不正な文字");
	
	}else{
	
		(*token) = (*token) -> next;
		
	}
}

char expect_ident(Token_t **token){


	if( (*token)->kind != TK_IDENT ){


		error_at( (*token)-> str,"無効な変数名" );
	
	}else{


		char name = (*token)-> str[0];
		(*token) = (*token) -> next;
		return name;
	}
}

int expect_num(Token_t **token){

	
	if( (*token) -> kind != TK_CONST  ){


		error_at( (*token) -> str ,"数ではありません");
	
	}else{
		
		int v = (*token) -> val;	
		(*token) = (*token) -> next;
		return v;
	}
}

bool at_eof(Token_t **token){


	if( (*token)-> kind != TK_EOF ){


		return false;
	
	}else{


		return true;
	}
}



Type* new_tp(int label,Type* pointerto,long int size){

	Type* tp = calloc(1,sizeof(Type));
	tp -> Type_label = label;
	tp -> pointer_to = pointerto;
	tp -> size = size;
	return tp;
}




Lvar *find_lvar(char *name,int length,Lvar **locals){//array は飛ばす
	

	for(Lvar *var = *locals; var;var = var -> next){


		if( var -> length == length && !memcmp( name, var ->name,length )  ){


			return var; 
		}
	}
	return NULL;
}

Lvar *new_lvar(Type *tp,char *name, int length,Lvar *next){

	Lvar *lvar = calloc(1,sizeof(Lvar));
	lvar -> next = next;
	lvar -> name = name;
	lvar -> length = length;
	lvar -> tp = tp;
	
	


	if( next ){


		lvar -> offset = next -> offset +(lvar -> tp -> size);

	}else{


		lvar -> offset = (lvar -> tp->size);
	}
	return lvar;
}





Token_t *consume_ident(Token_t **token){
	Token_t *tok = *token;
	*token = (*token)-> next;
	return tok;
}




//Node_t を作る関数
Node_t *new_node( Node_kind kind,Node_t *l,Node_t *r){


	Node_t *node=calloc(1,sizeof( Node_t ));
	node ->kind = kind;
	node -> left = l;
	node -> right =r;
	//型チェック
	if( l && ! l -> tp){


		fprintf(stderr,"型が未定義です\n");
		exit(1);
	}else if ( r  && ! r -> tp){


		fprintf(stderr,"型が未定義です\n");
		exit(1);
	}
	if ( l && r &&l -> tp -> Type_label != r -> tp -> Type_label){// 数値型とポインタの演算

		if( l -> tp -> Type_label == TP_POINTER || r -> tp -> Type_label == TP_POINTER){

		
			if(l -> tp -> Type_label != TP_POINTER || r -> tp -> Type_label != TP_POINTER ){


				if( l -> tp -> Type_label == TP_POINTER && r -> tp -> size < 9){
					
					
					node -> tp = l -> tp;
					node -> right -> val = node -> right -> val * ( l -> tp -> pointer_to -> size);
				
				}
				else if(r -> tp -> Type_label == TP_POINTER && l -> tp -> size < 9)
				{

					node -> tp = r -> tp;
					node -> left -> val = node -> right -> val * (r -> tp -> pointer_to -> size);
				}
				else
				{
					fprintf(stderr,"Pointer型と演算できません\n");
					exit(1);
				}
				return node;
			}
			else if(l -> kind == ND_LVAL && r -> kind == ND_STRINGITERAL)
			{
				if(l -> tp -> Type_label = TP_POINTER && l -> tp -> pointer_to ->Type_label == TP_CHAR)
				{
					node -> tp = l -> tp;
					return node;
				}
			}

		}
		else if( l -> tp -> size < 9 && r -> tp -> size < 9)
		{//char と INT　の演算は許す
			if( l -> tp -> size > r -> tp -> size)
			{
				node -> tp = l -> tp;
			}
			else
			{
				node -> tp = r -> tp;
			}
			return node;
		}
		else
		{

			fprintf(stderr,"型が一致しません\n");
			exit(1);
		}
	}
	if(l)
		node -> tp = l -> tp;

	return node;
}

Node_t *new_node_ident(Token_t**token){

	int flag_def = 0;// 0 : 型宣言なし
	Type *tp;

	if((*token)-> kind > 299){


		tp = (*token) -> tp;
		flag_def = 1;//型宣言あり
		(*token) = (*token)->next; 
	}

	Token_t *ident = consume_ident(token);//識別子読み込み
	Node_t *node;

	/**
	 * function call ====================================================
	 */
	if( find("(",token) ){


		node = calloc(1,sizeof(Node_t));
		node -> tp = new_tp(TP_INT,NULL,4);//後出かけ
		node -> kind = ND_FUNCTIONCALL;
		node -> val = 0; // 引数の個数

		//名前読み込み
		node -> name = calloc( ((*token)->length),sizeof(char));
		memcpy(node -> name,ident->str,ident -> length );

		Node_t *node_end = calloc(1,sizeof(Node_t));
		node -> left = node_end;

		while (!find(")",token))
		{


			node -> val++;

			node_end -> kind = ND_ARGMENT;
			Node_t *node_rightend = calloc(1,sizeof(Node_t));
			node_end ->left = unitary(token);
			node_end ->right = node_rightend;
			node_end = node_rightend;
			find(",",token);
		}
		node_end -> kind = ND_BLOCKEND;

		return node;
	}
	/*
	*	end function call ==================================================== 
	*/

	if(ident){
		

		node = calloc(1,sizeof(Node_t));
		node -> kind = ND_LVAL;

		Lvar *lvar = find_lvar( ident -> str,ident -> length,&(nametable->locals) );
		Lvar *glvar = find_lvar(ident -> str,ident -> length,&global);

		if(lvar){

			node -> tp = lvar -> tp;

			if(lvar -> tp -> Type_label == TP_POINTER 
				&& lvar -> next 
				&& lvar -> next -> tp -> Type_label == TP_ARRAY)
			{
				node -> val = lvar -> next -> tp -> size;
			}
			node -> offset = lvar -> offset;
			return node;

		}
		else if( glvar ){// global var

			node -> kind = ND_GLVALCALL;
			
			//名前コピー
			node -> name = calloc((ident -> length),sizeof(char));
			memcpy( node -> name , ident -> str, ident -> length);

			
			if( glvar -> tp -> Type_label == TP_ARRAY )
			{

				node -> tp = new_tp(TP_POINTER,glvar -> tp,8);
			}
			else
			{
				node -> tp = glvar -> tp;
			}
			return node;


		
		}else{// local var def =========================


			if( flag_def == 0 ){


				error_at(ident-> str,"型宣言がありません\n");
			}

			
			//is array?====================================================
			if ( find("[",token)){

				Type *top_tp = new_tp(TP_ARRAY , tp , expect_num(token) * (tp -> size));
				Lvar *array_top = new_lvar(top_tp , ident -> str , ident -> length , nametable -> locals);
				expect("]",token);
				
				lvar = new_lvar(new_tp(TP_POINTER,tp, 8 ),ident -> str,ident -> length,array_top);

				

			}else{//====================================================

				lvar = new_lvar(tp , ident -> str , ident -> length , nametable -> locals);

			}
			
			nametable -> locals = lvar;
			node -> val = 0;
			node -> offset = lvar -> offset;
			node ->tp = lvar -> tp;
			if( (*token) -> kind != TK_OPERATOR ){
				Node_t *top = new_node(ND_ASSIGN,node,node);
				return top;
			}
			return node;
		}//=========================
	}
}


Node_t *new_node_num(int val){


	Node_t *node=calloc(1,sizeof( Node_t ));
	node -> kind = ND_NUM;
	node -> val = val;
	node -> tp = new_tp(TP_INT,NULL,4);
	return node;
}

Node_t *new_node_keyword(Token_kind kind,Token_t **token){

	(*token) = (*token) -> next;

	Node_t *node = calloc(1,sizeof(Node_t));

	switch(kind){
	case TK_IF:

		node -> kind = ND_IF;

		expect("(",token);

		node -> left = assign(token);

		expect(")",token);

		node -> right = stmt(token);
		
		if( (*token)->kind == TK_ELSE ){

			Node_t *nodeup = calloc(1,sizeof(Node_t));
			nodeup ->kind = ND_ELSE;
			nodeup -> left = node;
			node -> kind = ND_IFE;

			(*token) = (*token)->next;

			nodeup -> right = calloc(1,sizeof(Node_t));
			nodeup -> right =stmt(token);
			
			
			return nodeup;

		}else{
			

			return node;
		}
	
	case TK_WHILE:
		
		node -> left = calloc(1,sizeof(Node_t));
		node ->right = calloc(1,sizeof(Node_t));

		node -> kind = ND_WHILE;
		node-> left = assign(token);
		node ->right =stmt(token);
		return node;
	
	case TK_FOR:

		node -> kind = ND_FOR;

		if((*token)-> str[0] != ';'){// no conditons
			
			
			expect("(",token);

			Node_t *conditions = calloc(1,sizeof(Node_t));
			conditions -> kind = ND_FORUPDATE;
			
			Node_t *init_condition = calloc(1,sizeof(Node_t));
			init_condition -> kind = ND_FORINITCONDITION;
			init_condition ->left = assign(token);

			expect(";",token);

			init_condition -> right = assign(token);

			expect(";",token);

			conditions -> left = init_condition;// first two conditions 
			conditions ->right = assign(token);// last update segment

			expect(")",token);

			node -> left = conditions;

		}

		node -> right = stmt(token);
		
		
		return node;
	
	case TK_RETURN:

		node -> kind = ND_RETURN;
		node -> left = assign(token);
		expect(";",token);
		return node;
	}
}

Node_t *new_node_stringiter(Token_t ** token)
{
	Node_t * node = calloc(1,sizeof(Node_t));
	node -> kind = ND_STRINGITERAL;
	node -> tp = new_tp(TP_POINTER,new_tp(TP_CHAR,NULL,1),8);
	
	Lvar *iter = calloc(1,sizeof(Lvar));
	iter -> name = calloc((*token) -> length , sizeof(char) );
	iter -> name = memcpy(iter -> name , (*token) -> str , (*token) -> length);
	iter -> next = string_iter;
	if(string_iter)
	{
		iter -> offset =  string_iter -> offset;
	}
	else
	{
		iter -> offset = 0;
	}
	node -> name = iter -> name;
	node -> offset = iter -> offset;
	string_iter = iter;

	(*token) = (*token) -> next;
	expect("\"",token);
	return node;
}

Node_t *new_node_globalident(Token_t**token){


	Node_t *node = calloc(1,sizeof(Node_t));

	node -> tp = (*token) -> tp;

	(*token) = (*token) ->next;

	//名前読み込み
	node -> name = calloc(((*token)->length),sizeof(char));
	memcpy(node -> name,(*token)->str,(*token)-> length );
	consume_ident(token);

	if(find("(",token)){//関数定義

		if(nametable != 0){//関数の変数情報更新

			nametable ->next = calloc(1,sizeof(Tables));
			nametable -> next -> head = nametable -> head;
			nametable = nametable -> next;
			
		}else{

			nametable = calloc(1,sizeof(Tables));
			nametable -> head = nametable;

		}


		node -> kind = ND_FUNCTIONDEF;
		

		node -> val = 0; // 引数の個数
	
		Node_t *left_argnames = calloc(1,sizeof(Node_t));
		node -> left = left_argnames;
		while (!find(")",token) && node -> val < 7){
			
			node -> val ++;

			left_argnames -> kind = ND_ARGMENT;
			left_argnames ->left = primary(token);

			Node_t *node_rightend = calloc(1,sizeof(Node_t));
			left_argnames -> right = node_rightend;
			left_argnames = node_rightend;

			find(",",token);
		}
		if( node -> val > 6){

			fprintf(stderr,"引数の個数が6個より大きいです");
			exit(1);
		}
		left_argnames -> kind = ND_BLOCKEND;//引数読み込み修了
		
		node -> right = stmt(token);// 定義本文
		return node;
	
	}else{//グローバル変数の定義

		node -> kind = ND_GLVALDEF;
		Lvar *lvar = find_lvar(node -> name , strlen(node -> name) , &global);
		
		if(lvar == NULL){//定義

			if(find("[",token)){// is array?==================================

				Type *array_tp = new_tp(TP_ARRAY,node -> tp,expect_num(token)*8);
				Lvar *array_top = new_lvar(array_tp,node -> name, strlen(node->name),global);
				lvar = array_top;
				expect("]",token);
				

			}else{// ==================================


				lvar = new_lvar(node -> tp,node -> name,strlen(node -> name),global);
			}
			
			global = lvar;
		}

		node -> tp = lvar -> tp;
		if (find("=",token))//変数の代入
		{
			node -> val = expect_num(token);
		}
		expect(";",token);
		return node;
	}
}

Node_t *new_node_block(Token_t ** token){


	Node_t *node = calloc(1,sizeof(Node_t));
		
	node -> kind = ND_BLOCK;

	node -> left = stmt(token);

	Node_t *node_top = node;

	while (!find("}",token))
	{
		
		Node_t *right = calloc(1,sizeof(Node_t));

		node -> right = right;

		node = right;
		node -> kind = ND_BLOCK;
		node -> left = stmt(token);
	}

	Node_t *end = calloc(1,sizeof(Node_t));
	end -> kind = ND_BLOCKEND;
	
	node -> right = end;
	return node_top;
}

Node_t *new_node_ref_deref(Token_t **token){

	Node_t *node;
	switch ((*token)->str[0])
		{
		case '*':

			(*token) = (*token) -> next;

			node = calloc(1,sizeof(Node_t));
			node -> kind = ND_DEREF;
			
			node -> left = unitary(token);

			if( node -> left -> tp -> Type_label != TP_POINTER ){


				error_at((*token)->str,"ポインタ型ではありません\n");
			}
			if( node -> left -> tp -> pointer_to -> Type_label == TP_ARRAY)
			{
				node -> tp = node -> left -> tp -> pointer_to -> pointer_to;
			}
			else
			{
				node -> tp = node -> left -> tp -> pointer_to;
			}
			return node;
		
		case '&':

			(*token) = (*token) -> next;

			node = calloc(1,sizeof(Node_t));
			node -> kind = ND_ADDR;
			node -> left = unitary(token);
			node -> tp = new_tp(TP_POINTER,node -> left -> tp,8);
			return node;
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
 * 		| "{" stmt* "}"
 * 		| "if" "(" assign  ")" stmt ( "else" stmt  )?
 * 		| "while"  "(" assign ")" stmt
 * 		| "for"  "(" assign?; assign? ; assign? ")"stmt
 * 		| "return" assign ";"
 * 		| ...
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
 * 			| "\"" strig iteal "\""
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

			fprintf(stderr,"型宣言がありません");
			exit(1);

	}
	if( !( (*token) ->next ) && (*token) -> next -> kind != TK_IDENT ){


		fprintf(stderr,"関数名が必要です");
		exit(1);
	
	}
	return new_node_globalident(token);
}

Node_t *stmt(Token_t **token){


	Node_t *node;

	if( (*token) -> kind > 99 && (*token) -> kind < 200 ){//if (else) while for return をパース


		node = new_node_keyword( (*token) -> kind,token);

	}else if( find("{",token) ){
		

		node = new_node_block(token);
	
	}else{

		node = assign(token);
		expect(";",token);
	}
	
	return node;
}

Node_t *assign(Token_t **token){


	Node_t *node = equality(token);
	
	if( find("=",token) ){
		

		node = new_node(ND_ASSIGN,node,assign(token));
	}
	return node;
}


Node_t *equality(Token_t **token){


	Node_t *node = relational(token);

	for(;;){
		

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

		
				
	}else if( find("+",token) ){


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
			fprintf(stderr,"N kind : %d",node -> kind);
			fprintf(stderr,"Type label : %d",node -> tp -> Type_label);
			fprintf(stderr,"識別子がありません");
			exit(1);
		}
		Node_t *node_top = calloc(1,sizeof(Node_t));
		node_top -> kind = ND_DEREF;
		if(node -> tp -> pointer_to -> Type_label == TP_ARRAY)
		{
			node_top -> tp = node -> tp -> pointer_to -> pointer_to;
		}else
		{
			node_top -> tp = node -> tp -> pointer_to;
		}
		node_top -> val = -1;
		node_top -> left = new_node(ND_ADD,node,mul(token));

		expect("]",token);

		return node_top;

	}
	return node;
}

