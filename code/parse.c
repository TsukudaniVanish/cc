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

/*
 * token から構文木を生成 
 */
/*
 * 生成文法
 *
 * program = stmt*
 * stmt = assign";" | "return" assign ";"
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

	if( (*token)->kind == TK_RETURN ){

		*token = (*token)->next;

		node = calloc(1,sizeof(Node_t));
		node -> kind = ND_RETURN;
		node -> left = assign(token);
	
	}else{


		node = assign(token);
	}

	expect(";",token);
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




//トークン列から抽象構文木を生成
Node_t *expr(Token_t **token){


	return equality(token);
}



