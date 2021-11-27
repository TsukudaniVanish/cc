#include "cc.h"
//#include<stdlib.h>




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
	return node;
}

Node_t *new_node_ident(char alpha){


	Node_t *node = calloc(1,sizeof( Node_t ));
	node -> kind = ND_LVAL;
	node -> offset = ( alpha - 'a' +1 )*8;
	return node;
}


Node_t *new_node_num(int val){


	Node_t *node=calloc(1,sizeof( Node_t ));
	node -> kind = ND_NUM;
	node -> val = val;
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

/*
 * token から構文木を生成 
 */
/*
 * 生成文法
 *
 * program = stmt*
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
 * unitary = ('+' | '-' )? primary
 * primary = num | indent | "(" assign ")"
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


		code[i] = stmt(token);
		i++;
	}
	code[i] = NULL;
}

Node_t *stmt(Token_t **token){


	Node_t *node;

	if( (*token) -> kind == TK_IF ){

		
		node = new_node_keyword(TK_IF,token);

	}else if( (*token)->kind == TK_WHILE ){
	

		node = new_node_keyword(TK_WHILE,token);
	
	
	}else if( (*token) -> kind == TK_FOR ) {

		node = new_node_keyword(TK_FOR,token);
	
	}else if( (*token)->kind == TK_RETURN ){

		
		node =new_node_keyword(TK_RETURN,token);

	}else if( find("{",token) ){
		
		node = calloc(1,sizeof(Node_t));
		
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
	if( find("+",token) ){


		node = primary(token);
		
	}else if( find("-",token) ){


		node = new_node(ND_SUB,new_node_num(0),primary(token));
		
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
		return node;

	}else if( (*token)-> kind == TK_IDENT  ){


		Token_t *ident = consume_ident(token);
		
		if(ident){
			

			node = calloc(1,sizeof(Node_t));
			node -> kind = ND_LVAL;

			Lvar *lvar = find_lvar(&ident,&locals);

			if(lvar){


				node -> offset = lvar -> offset;
			}else{


				lvar = calloc(1,sizeof(Lvar));
				lvar -> next = locals;
				lvar -> name = ident -> str;
				lvar -> length = ident -> length;
				if(locals){
					lvar -> offset = locals -> offset +8;
				}else{
					lvar -> offset = 8;
				}
				node -> offset = lvar -> offset;
				locals = lvar;
			}
			return node;
		}

	}else{


		node = new_node_num(expect_num(token));
	}

	return node;
}

