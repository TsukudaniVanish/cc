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
		fprintf(stderr,": %d:型エラー\n",typecheck(node));
		exit(1);
	}
	
	node -> tp = imptypechast(node);
	if(node -> tp)
	{
		return node;
	}
	else
	{
		fprintf(stderr,": %d:型エラー\n",typecheck(node));
		exit(1);
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

Node_t *new_node_ident(Token_t**token)
{

	if(is_functioncall(token))
	{
		return new_node_funcCall(token);
	}

	int flag_def = 0;// 0 : 型宣言なし
	Type *tp;

	if((*token)-> kind > 299){


		tp = (*token) -> tp;
		flag_def = 1;//型宣言あり
		(*token) = (*token)->next; 
	}

	Token_t *ident = consume(token);//識別子読み込み
	Node_t *node;

	if(ident){
		

		node = calloc(1,sizeof(Node_t));
		

		Lvar *lvar = find_lvar( ident -> str,ident -> length,&(nametable->locals) );
		Lvar *glvar = find_lvar(ident -> str,ident -> length,&global);

		if(lvar){
			node -> kind = ND_LVAL;

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
		else if( glvar ){// global var call 

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


		
		}else
		{// local var def なし=========================
			fprintf(stderr,"不明な識別子");
			exit(1);
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
	node -> kind = ND_STRINGITERAL;
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


	Node_t *node = calloc(1,sizeof(Node_t));

	node -> tp = read_type(&node -> name,token);

	if(find("(",token)){//関数定義

		//関数をname table に登録
        declere_glIdent(node -> tp,node -> name,strlen(node -> name),&global);

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
			left_argnames ->left = Lvardec(token);

			Node_t *node_rightend = calloc(1,sizeof(Node_t));
			left_argnames -> right = node_rightend;
			left_argnames = node_rightend;

			find(",",token);
		}
		if( node -> val > 6){

			fprintf(stderr,"引数の個数が6個より大きいです");
			exit(1);
		}
		left_argnames -> kind = ND_BLOCKEND;//引数読み込み終了
		
		node -> right = stmt(token);// 定義本文
		return node;
	
	}else{//グローバル変数の定義

		node -> kind = ND_GLVALDEF;
		Lvar *lvar = declere_glIdent(node -> tp,node -> name, strlen(node -> name),&global);
		
		if(lvar == NULL){// 既に定義されている
            fprintf(stderr,"%s は既に定義されています",node -> name);
            exit(1);
		}

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