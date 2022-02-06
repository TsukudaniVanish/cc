#include "cc.h"

extern unsigned int String_len(char*);
extern void Memory_copy(void*, void*, unsigned int);
Node_t *new_Node_t(Node_kind kind,Node_t *l,Node_t *r,int v,long int off,Type* tp,char *name)
{
	Node_t *node = calloc(1,sizeof(Node_t));
	node -> kind = kind;
	node -> left = l;
	node -> right = r;
	node -> val = v;
	node -> offset = off;
	node -> tp = tp;
	node -> name = name;
	return node;
}



//Node_t making function with type check typically used in parsing formula
Node_t *new_node( Node_kind kind,Node_t *l,Node_t *r, char *parsing_here){
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

Node_t *new_node_function_call(Token_t **token)
{
	if(find_lvar((*token) -> str, (*token) -> length, &global) == NULL)
	{
		error_at((*token) -> str, "this is not exist int name space");
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

int Node_read_function_parameters(Token_t **token,Node_t **vector)
{
	expect('(',token);

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

Node_t *new_node_function_definition(Token_t **token)
{
	Node_t *node = new_Node_t(ND_FUNCTIONDEF,NULL,NULL,0,0,NULL,NULL);
	node = declere_specify(token, node);
	node = ident_specify(token, node); 

	declere_glIdent(node -> tp,node -> name,String_len(node -> name),&global);

	node -> val = Node_read_function_parameters(token,&node -> left);
	
	node -> right = stmt(token);
	return node;

}

Node_t *new_node_var(Token_t **token)
{
	Lvar *lvar = NULL;
	Lvar *table = Vector_get_tail(nameTable);
	Token_t *ident = consume(token);
	if(ident)
		lvar = find_lvar(ident -> str,ident -> length,&table);
	else
	{
		fprintf(stderr,"Fail to consume token\n");
	}
	if(lvar)
	{//local 変数
	
		return new_Node_t(ND_LVAL,NULL,NULL,0,lvar -> offset,lvar -> tp,lvar -> name);
	}
	lvar = find_lvar(ident -> str,ident -> length,&global);
	if(lvar)
	{//グローバル変数
		return new_Node_t(ND_GLOBVALCALL,NULL,NULL,0,0,lvar -> tp,lvar -> name);
	}
	error_at(ident -> str,"不明な識別子");
}

Node_t *new_node_ident(Token_t**token)
{

	if(is_functioncall(token))
	{
		return new_node_function_call(token);
	}

	return new_node_var(token);
	
}


Node_t *new_node_num(int val){


	Node_t *node = new_Node_t(ND_NUM,NULL,NULL,val,0,new_tp(TP_INT,NULL,4),NULL);
	return node;
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

Node_t *new_node_return(Token_t **token)
{
	Node_t *node = new_Node_t(ND_RETURN, expr(token),NULL,0,0,NULL,NULL);
	expect(';',token);
	return node;
}

Node_t *new_node_flow_operation(Token_kind kind,Token_t **token) {

	Node_t *node = calloc(1,sizeof(Node_t));

	switch(kind){
	case TK_IF: return new_node_if(token);			
	case TK_WHILE: return new_node_while(token);
	case TK_FOR: return new_node_for(token);
	case TK_RETURN: return new_node_return(token);
	default:
		return NULL;
	}
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

	//グローバル変数の定義
	Node_t *node = new_Node_t(ND_GLOBVALDEF,NULL,NULL,0,0,NULL,NULL);
	node = declere_specify(token, node);
	if(node -> tp -> Type_label != TP_STRUCT || (*token) -> kind == TK_IDENT)
		node = ident_specify(token, node);

	Lvar *lvar = declere_glIdent(node -> tp,node -> name, String_len(node -> name),&global);
	if (find('=',token))//変数の代入
	{
		node -> val = expect_num(token);
	}
	expect(';',token);
	return node;
	
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

Node_t *new_node_ref_deref(Token_t **token){

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
