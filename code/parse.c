#include "cc.h"
//#include<stdlib.h>
//#inclide<string.h>




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
	if( ! l -> tp){


		fprintf(stderr,"型が未定義です\n");
		exit(1);
	}else if (! r -> tp){


		fprintf(stderr,"型が未定義です\n");
		exit(1);
	}
	if (l -> tp -> Type_label != r -> tp -> Type_label){

		if( l -> tp -> Type_label == TP_POINTER || r -> tp -> Type_label == TP_POINTER){

		
			if(l -> tp -> Type_label == TP_INT || r -> tp -> Type_label == TP_INT ){


				if( l -> tp -> Type_label == TP_POINTER){
					
					node -> tp = l -> tp;
				
				}else{

					node -> tp = r -> tp;
				}
				return node;
			}

		}else{

			fprintf(stderr,"型が一致しません\n");
			exit(1);
		}
	}
	node -> tp = l -> tp;

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
	node -> tp = calloc(1,sizeof(Type));
	node -> tp -> Type_label = TP_INT;
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
 * primary = num | type? indent  ( "(" ")" )? | "(" assign ")"
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

		if(funclocal == 0){//関数の変数情報更新

			funclocal = calloc(1,sizeof(Funcvar));
			funclocal -> head = funclocal;
		}else{

			funclocal ->next = calloc(1,sizeof(Funcvar));
			funclocal -> next -> head = funclocal -> head;
			funclocal = funclocal -> next;

		}

		code[i] = func(token);
		i++;
	}
	code[i] = NULL;
}

Node_t *func(Token_t **token){// function def


	Node_t *node;

	if ((*token) -> kind != TK_Type){

			fprintf(stderr,"型宣言がありません");
			exit(1);

	}

	
	if( !( (*token) ->next ) && (*token) -> next -> kind != TK_IDENT ){


		fprintf(stderr,"関数名が必要です");
		exit(1);
	
	}else{


		node = calloc(1,sizeof(Node_t));
		node -> kind = ND_FUNCTIONDEF;

		node -> tp = (*token) -> tp;

		(*token) = (*token) ->next;

		//名前読み込み
		node -> name = calloc(((*token)->length),sizeof(char));
		memcpy(node -> name,(*token)->str,(*token)-> length );

		consume_ident(token);

		node -> val = 0; // 引数の個数
		// if(funclocal ->locals){
		// 	node -> offset = locals -> offset;//top offset
		// }else{
		// 	node -> offset = 0;
		// }

		expect("(",token);
		
		Node_t *left_argnames = calloc(1,sizeof(Node_t));
		node -> left = left_argnames;
		while (!find(")",token) && node -> val < 7)
		{
			
			node -> val ++;
			left_argnames -> kind = ND_ARGMENT;
			left_argnames ->left = primary(token);
			Node_t *node_rightend = calloc(1,sizeof(Node_t));
			left_argnames -> right = node_rightend;
			left_argnames = node_rightend;
		}

		if( node -> val > 6)
		{

			fprintf(stderr,"引数の個数が6個より大きいです");
			exit(1);
		}
		left_argnames -> kind = ND_BLOCKEND;
		node -> right = stmt(token);
		return node;
		

	}
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
	if( (*token)-> kind == TK_SIZEOF ){


		(*token) = (*token) -> next;
		node = unitary(token);
		Type *tp = node -> tp;

		if( tp -> Type_label == TP_POINTER && node -> val == 1 ){
			
			
			tp -> Type_label = TP_ARRAY;
		}
		
		if(  tp -> Type_label == TP_INT ){


			node = new_node_num(4);

		}else if( tp -> Type_label == TP_ARRAY ){


			if( tp -> pointer_to -> Type_label == TP_INT ){


				node = new_node_num( 4 * ( tp -> size) );

			}else if(tp -> pointer_to -> Type_label == TP_POINTER ){


				node = new_node_num( 8 * ( tp -> size )  );

			}else{


				fprintf(stderr,"不明な型名");
				exit(1);
			}
		}else if( tp -> Type_label == TP_POINTER ){


			node = new_node_num(8);
		
		}else{


			fprintf(stderr,"不明な型名");
			exit(1);
		}
	}else if( find("+",token) ){


		node = primary(token);
		
	}else if( find("-",token) ){


		node = new_node(ND_SUB,new_node_num(0),primary(token));
		node -> tp = calloc(1,sizeof(Type));
		node -> tp = node -> right -> tp;
		
	}else if( (*token) -> kind == TK_OPERATOR && ( (*token) -> str[0] == '*' || (*token) -> str[0] == '&' )   ){


		
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
			node -> tp = node -> left -> tp -> pointer_to;
			return node;
		
		case '&':

			(*token) = (*token) -> next;

			node = calloc(1,sizeof(Node_t));
			node -> kind = ND_ADDR;
			node -> left = unitary(token);
			node -> tp = calloc(1,sizeof(Type));
			node -> tp -> Type_label = TP_POINTER;
			node -> tp -> pointer_to = node -> left -> tp;
			return node;
		}
		
	
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

	}else if( (*token)-> kind == TK_IDENT || (*token) -> kind == TK_Type  ){

		int flag_def = 0;// 0 : 型宣言なし
		Type *tp;

		if((*token)-> kind == TK_Type){


			tp = (*token) -> tp;
			flag_def = 1;//型宣言あり
			(*token) = (*token)->next; 
		}

		Token_t *ident = consume_ident(token);

		/**
		 * function call ====================================================
		 */
		if( find("(",token) ){


			node = calloc(1,sizeof(Node_t));
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

			Lvar *lvar = find_lvar(&ident,&(funclocal->locals) );

			if(lvar){

				node -> tp = lvar -> tp;
				//配列型かどうか====================================================
				if ( lvar -> tp -> Type_label == TP_ARRAY ){
					node -> tp -> Type_label = TP_POINTER;
					node -> val = 1;// 元は配列でした
				}
				//====================================================

				node -> offset = lvar -> offset;

			}else{// local var def =========================


				if( flag_def == 0 ){


					error_at(ident-> str,"型宣言がありません\n");
				}
				lvar = calloc(1,sizeof(Lvar));
				lvar -> next = funclocal-> locals;
				lvar -> name = ident -> str;
				lvar -> length = ident -> length;
				
				//配列型かどうか====================================================
				if ( find("[",token)){


					lvar -> tp = calloc(1,sizeof(Type));
					lvar -> tp -> Type_label = TP_ARRAY;
					lvar -> tp -> pointer_to = tp;
					lvar -> tp -> size = expect_num(token);
					expect("]",token);

				}else{


					lvar -> tp = tp;
					lvar -> tp -> size =1;
				}
				//====================================================

				//offset の計算
				int type_size = 8;

				if(tp -> Type_label == TP_INT){
					//type_size =4; 8の倍数でoffset は考える
				}

				if(funclocal ->locals){


					lvar -> offset = (funclocal-> locals -> offset) +type_size*(lvar -> tp -> size);

				}else{


					lvar -> offset = type_size*(lvar -> tp->size);
				}
				node -> val = -1;
				node -> offset = lvar -> offset;
				node ->tp = lvar -> tp;
				funclocal -> locals = lvar;
			}//=========================
			return node;
		}
	}else{


		node = new_node_num(expect_num(token));
	}
	return node;
}

