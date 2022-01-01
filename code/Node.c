#include "cc.h"

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



//Node_t を作る関数
Node_t *new_node( Node_kind kind,Node_t *l,Node_t *r){


	Node_t *node = new_Node_t(kind,l,r,0,0,NULL,NULL);
	//型チェック
	if(typecheck(node) == 0)
	{
		error_at(parsing_here,"型エラー");
	}
	
	node -> tp = imptypecast(node);
	if(node -> tp)
	{
		return node;
	}
	else
	{
		error_at(parsing_here,"型エラー");
	}
}

Node_t *new_node_funcCall(Token_t **token)
{
	Node_t *node = new_Node_t(ND_FUNCTIONCALL,NULL,NULL,0,0,NULL,NULL);
	node -> tp = Type_function_return(&node -> name,token);

	expect("(",token);

	Node_t *node_end = calloc(1,sizeof(Node_t));
	node -> left = node_end;

	while (!find(")",token))
	{

		node -> val++;

		node_end -> kind = ND_ARGMENT;
		Node_t *node_rightend = calloc(1,sizeof(Node_t));
		node_end ->left = add(token);
		node_end ->right = node_rightend;
		node_end = node_rightend;
		find(",",token);
	}
	node_end -> kind = ND_BLOCKEND;
	return node;
}

int Node_read_funcarg(Token_t **token,Node_t **vector)
{
	expect("(",token);

	int to_return = 0;
	Node_t* v = new_Node_t(ND_ARGMENT,NULL,NULL,0,0,NULL,NULL);
	*vector = v;
	while (!find(")",token))
	{
		v -> left = Lvardec(token);
		find(",",token);

		Node_t *next = new_Node_t(ND_ARGMENT,NULL,NULL,0,0,NULL,NULL);
		v -> right = next;

		to_return ++;

		v = next;
	}

	v -> kind = ND_BLOCKEND;
	return to_return;
	
}

Node_t *new_node_funcDef(Token_t **token)
{
	Node_t *node = new_Node_t(ND_FUNCTIONDEF,NULL,NULL,0,0,NULL,NULL);
	node -> tp = read_type(&node -> name,token);

	declere_glIdent(node -> tp,node -> name,strlen(node -> name),&global);

	node -> val = Node_read_funcarg(token,&node -> left);
	
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
		if(
			lvar -> tp -> Type_label == TP_POINTER
			&& lvar -> next
			&& lvar -> next -> tp -> Type_label == TP_ARRAY
		)//size of 用の処理
			return new_Node_t(ND_LVAL,NULL,NULL,lvar -> next -> tp -> size,lvar -> offset,lvar -> tp,lvar -> name);
		return new_Node_t(ND_LVAL,NULL,NULL,0,lvar -> offset,lvar -> tp,NULL);
	}
	lvar = find_lvar(ident -> str,ident -> length,&global);
	if(lvar)
	{//グローバル変数
		if(lvar -> tp -> Type_label != TP_ARRAY)//暗黙にpointer 型にキャストする
			return new_Node_t(ND_GLOBVALCALL,NULL,NULL,0,0,lvar -> tp,lvar -> name);
		else
			return new_Node_t(ND_GLOBVALCALL,NULL,NULL,0,0,new_tp(TP_POINTER,lvar -> tp,8),lvar -> name);
	}
	error_at(ident -> str,"不明な識別子");
}

Node_t *new_node_ident(Token_t**token)
{

	if(is_functioncall(token))
	{
		return new_node_funcCall(token);
	}

	return new_node_var(token);
	
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

			Node_t *uppernode = calloc(1,sizeof(Node_t));
			uppernode ->kind = ND_ELSE;
			uppernode -> left = node;
			node -> kind = ND_IFE;

			(*token) = (*token)->next;

			uppernode -> right = calloc(1,sizeof(Node_t));
			uppernode -> right =stmt(token);
			
			
			return uppernode;

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

		if((*token)-> str[0] != ';'){// no conditions
			
			
			expect("(",token);

			Node_t *conditions = calloc(1,sizeof(Node_t));
			conditions -> kind = ND_FORUPDATE;
			
			Node_t *init_condition = calloc(1,sizeof(Node_t));
			init_condition -> kind = ND_FORINITCONDITION;
			if(is_lvardec(token))
			{
				init_condition -> left = Lvardec(token);
				expect(";",token);
			}
			else
			{
				init_condition -> left = assign(token);
				expect(";",token);
			}

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
	node -> kind = ND_STRINGLITERAL;
	node -> tp = new_tp(TP_POINTER,new_tp(TP_CHAR,NULL,1),8);
	
	Lvar *iter = declere_ident(node -> tp,(*token) -> str,(*token) -> length,&string_iter);
	
	node -> name = calloc(iter -> length, sizeof(char));
    memcpy(node -> name,iter -> name,iter -> length);
	node -> offset = iter -> offset;
	
	consume(token);
	expect("\"",token);
	return node;
}

Node_t *new_node_globalident(Token_t**token){

	if(is_functioncall(token))
	{
		return new_node_funcDef(token);
	}

	//グローバル変数の定義
	Node_t *node = new_Node_t(ND_GLOBVALDEF,NULL,NULL,0,0,NULL,NULL);

	node -> tp = read_type(&node -> name,token);
	Lvar *lvar = declere_glIdent(node -> tp,node -> name, strlen(node -> name),&global);

	if (find("=",token))//変数の代入
	{
		node -> val = expect_num(token);
	}
	expect(";",token);
	return node;
	
}

Node_t *new_node_block(Token_t ** token){


	Node_t *node = calloc(1,sizeof(Node_t));
		
	node -> kind = ND_BLOCK;

	Node_t *node_top = node;

	while (!find("}",token))
	{
		node -> kind = ND_BLOCK;
		node -> left = stmt(token);

		Node_t *right = calloc(1,sizeof(Node_t));
		node -> right = right;

		node = right;
	}

	node -> kind = ND_BLOCKEND;
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