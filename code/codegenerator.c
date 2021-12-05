#include "cc.h"
//#include<stdio.h>
//#include<stdlib.h>



int rsp_counter = 0;//rsp/8 の値 function callの時 rspを16の倍数にそろえるため
int filenumber =0;//制御構文のラベル指定で使用する




void set_array_header(){

	if( nametable == 0 )
		return;

	for( Lvar *var = (nametable -> locals) ; var ; var = var -> next ){


		if (var ->tp -> Type_label == TP_ARRAY){

			printf("	mov rax, rbp\n");
			printf("	sub rax, %ld\n",var ->  offset);
			printf("	mov rdi, rax\n");
			printf("	sub rax, 8\n");
			printf("	mov [rax], rdi\n");
		}
	}
	
	
	return;
}




void gen_lval(Node_t *node){



	if(node -> kind != ND_LVAL){
		
		
		fprintf(stderr,"代入の左辺値が変数ではありません");
		exit(1);

	}
	else
	{

		printf("	mov rax, rbp\n");
		printf("	sub rax, %ld\n",  node -> offset  );
		printf("	push rax\n");
		rsp_counter++;
	}
}

void gen_function_call(Node_t *node){


	//引数読み込み
	Node_t *node_arg = node -> left;

	while (node_arg ->kind != ND_BLOCKEND){
		
		generate(node_arg -> left);
		node_arg = node_arg -> right;

	}//読み込み終了
		

		switch(node ->val){
		case 6:

			printf("	pop r9\n");

		case 5:

			printf("	pop r8\n");

		case 4:

			printf("	pop rcx\n");

		case 3:

			printf("	pop rdx\n");

		case 2:

			printf("	pop rsi\n");

		case 1:

			printf("	pop rdi\n");
		}
		rsp_counter -= node -> val;

		if(rsp_counter%2 ==1){


			printf("	sub rsp , 8\n");
		}
		printf("	call %s\n",node -> name);
		rsp_counter ++;
		printf("	push rax\n");
		rsp_counter++;
		return;
}

void gen_function_def(Node_t *node){


	printf("%s:",node -> name);
	int return_rsp_number = rsp_counter;

	//prologue=======================================
	printf("	push rbp\n");
	rsp_counter++;
	printf("	mov rbp ,rsp\n");

	if(nametable && nametable ->locals){


		printf("	sub rsp, %ld\n",nametable ->locals -> offset);
		rsp_counter += nametable ->locals->offset /8;
	}//=======================================
	//引数代入
	switch (node->val)
	{
	case 6:
		
		printf("	mov rax, rbp\n");
		printf("	sub rax, %d\n",48 );
		printf("	mov [rax], r9\n");
	
	case 5:
		
		printf("	mov rax, rbp\n");
		printf("	sub rax, %d\n",40 );
		printf("	mov [rax], r8\n");

	case 4:

		printf("	mov rax, rbp\n");
		printf("	sub rax, %d\n",32 );
		printf("	mov [rax], rcx\n");

	case 3:

		printf("	mov rax, rbp\n");
		printf("	sub rax, %d\n",24 );
		printf("	mov [rax], rdx\n");

	case 2:

		printf("	mov rax, rbp\n");
		printf("	sub rax, %d\n",16 );
		printf("	mov [rax], rsi\n");

	case 1:

		printf("	mov rax, rbp\n");
		printf("	sub rax, %d\n",8 );
		printf("	mov [rax], rdi\n");	
		break;
	}

	set_array_header();
	

	generate(node -> right);//定義本文をコンパイル

	//epilogue return に書く
	printf("	pop rax\n");
	rsp_counter --;
	printf("	mov rsp, rbp\n");
	printf("	pop rbp\n");
	rsp_counter --;
	printf("	ret\n");

	return;
}




//抽象構文木からアセンブリコードを生成する
void generate(Node_t *node){

	if(!node)
		return;

	
	//ノード末端付近
	switch(node -> kind) {
	case ND_NUM:

		printf("	push %d\n",node ->val);
		rsp_counter++;
		return;
	case ND_ASSIGN:

		if( node -> left && node -> left ->  kind != ND_DEREF){

			if(node -> left -> kind == ND_GLVALCALL)
			{
				generate(node -> right);
				printf("	pop rax\n");
				printf("	mov QWORD PTR %s[rip], rax\n",node -> left -> name);
				return;
			}
			gen_lval(node->left);
		
		}else{
			
			if(node -> left -> left -> kind == ND_ADD)
			{
				Node_t *node_ll = node -> left -> left;
				if(node_ll -> left -> kind == ND_GLVALCALL)
				{
					Type *tp_lll = node_ll -> left -> tp;
					if(tp_lll -> Type_label == TP_POINTER && tp_lll -> pointer_to -> Type_label == TP_ARRAY)
					{
						generate(node -> right);
						printf("	pop rax\n");
						printf("	mov QWORD PTR %s[rip+%d], rax\n",node_ll -> left -> name,node_ll -> right -> val);
						return;
					}
				}
				else if(node_ll -> right -> kind == ND_GLVALCALL)
				{
					Type *tp_llr = node_ll -> right -> tp;
					if(tp_llr -> Type_label == TP_POINTER && tp_llr -> pointer_to -> Type_label == TP_ARRAY)
					{
						generate(node -> right);
						printf("	pop rax\n");
						printf("	mov QWORD PTR %s[rip+%d], rax\n",node_ll -> right -> name,node_ll -> left -> val);
						return;
					}
				}
			}
			generate(node -> left -> left);
		}	
		
		generate(node -> right);

		printf("	pop rdi\n");
		rsp_counter--;
		printf("	pop rax\n");
		rsp_counter--;
		printf("	mov [rax], rdi\n");
		printf("	push rdi\n");
		rsp_counter++;
		return;
	
	case ND_ADDR:

		gen_lval(node -> left);
		return;

	case ND_DEREF:

		if(node -> left -> kind == ND_ADD)
		{

			Node_t *node_l = node -> left;
			if( node_l -> left -> kind == ND_GLVALCALL )
			{

				Type *tp_l = node_l -> left -> tp;

				if(tp_l -> Type_label == TP_POINTER && tp_l -> pointer_to -> Type_label == TP_ARRAY)
				{	

					if(node_l -> right -> val >= 0)
					{

						printf("	mov rax, QWORD PTR %s[rip+%d]\n",node_l -> left -> name,node_l -> right -> val);
						printf("	push rax\n");
						return;
					}
					else
					{

						fprintf(stderr,"添え字が配列範囲外です");
						exit(1);
					}
					
				}
			}
			else if( node_l -> right -> kind == ND_GLVALCALL )
			{

				Type *tp_r = node_l -> right -> tp;
				
				if(tp_r -> Type_label == TP_POINTER && tp_r -> pointer_to -> Type_label == TP_ARRAY)
				{

					if( node_l -> left -> val >= 0)
					{

						printf("	mov rax, QWORD PTR %s[rip+%d]\n",node_l -> right -> name,node_l -> left -> val);
						return;
					}else
					{
						fprintf(stderr,"添え字が配列の範囲外です");
						exit(1);
					}
				}
			}
		}
		generate(node -> left);
			
		
		printf("	pop rax\n");
		printf("	mov rax, [rax]\n");
		printf("	push rax\n");
		return;

	
	case ND_LVAL:

		gen_lval(node);
		printf("	pop rax\n");
		printf("	mov rax, [rax]\n");
		printf("	push rax\n");
		return;

	case ND_GLVALCALL:

		printf("	mov rax, QWORD PTR %s[rip]\n",node -> name);
		printf("	push rax\n");
		return;
		

	case ND_FUNCTIONCALL://function call abi -> System V AMD64 ABI (Lunix)

		gen_function_call(node);
		return;

	case ND_RETURN:
		
		generate(node -> left);
		printf("	pop rax\n");
		rsp_counter--;
		printf("	mov rsp, rbp\n");
		rsp_counter =1;
		printf("	pop rbp\n");
		rsp_counter --;
		printf("	ret\n");
		return;

	}

	//ノード先端付近
	switch(node -> kind){
	case ND_GLVALDEF:

		printf("%s:\n",node -> name);

		
		if(node -> tp -> Type_label == TP_INT)
		{

			if(node -> val == 0){
		
		
				printf("	.zero %ld\n",node -> tp -> size);
			
			}else{
				

				printf("	.long %d\n",node -> val);
			}
		}
		else{

			printf("	.zero %ld\n",node -> tp -> size);
		}
		

		
		return;
	
	case ND_FUNCTIONDEF:

		gen_function_def(node);
		return;

	case ND_BLOCK:

		while (node->right ->kind != ND_BLOCKEND){
			
			
			generate(node -> left);
			node = node ->right;

		}
		generate(node ->left);
		
		return;


	case ND_IF:
	
		generate(node -> left);
		printf("	pop rax\n");
		rsp_counter --;
		printf("	cmp rax, 0\n");
		printf("	je  .Lend%d\n",filenumber);
		int endnumber_if = filenumber;

		generate(node -> right);

		printf(".Lend%d:\n",endnumber_if);
		
		filenumber ++;
		return;

	case ND_ELSE:
		
		if( node -> left && node -> left -> kind == ND_IFE){
			
			
			generate(node -> left);
			int endnumber_else = filenumber;
			
			generate(node -> right);
			
			printf(".Lend%d:\n",endnumber_else);
			filenumber++;
			return;
		
		}else{
			

			fprintf(stderr,"elseはif(...)...の後に続きます");
			exit(1);
		}

	case ND_IFE:

		generate(node -> left);
		
		printf("	pop rax\n");
		rsp_counter--;
		printf("	cmp rax, 0\n");
		printf("	je  .Lelse%d\n",filenumber);
		int elsenumber = filenumber;
		filenumber++;

		generate(node -> right);
		
		printf("	jmp .Lend%d\n",filenumber);

		printf(".Lelse%d:\n",elsenumber);
		return;

	case ND_WHILE:

		printf(".Lbegin%d:\n",filenumber);
		int beginnumber_while = filenumber;
		
		generate(node -> left);

		printf("	pop rax\n");
		rsp_counter--;
		printf("	cmp rax, 0\n");
		printf("	je	.Lend%d\n",filenumber);
		int endnumber_while = filenumber;

		generate(node -> right);
		printf("	pop rax\n");
		rsp_counter--;
		printf("	jmp .Lbegin%d\n",beginnumber_while);
		printf(".Lend%d:\n",endnumber_while);
		filenumber++;
		return;

	case ND_FOR:

		if(!node -> left ){//loop will go on


			fprintf(stderr,"for(;;)は無効です");
			exit(1);
		
		}else{


			Node_t *conditions = node -> left;
			Node_t *init_condition = conditions -> left;
			Node_t *update = conditions -> right;

			generate(init_condition -> left);

			printf(".Lbegin%d:\n",filenumber++);
			int beginnumber_for = filenumber -1;
			int endnumber_for = filenumber;
			
			generate(init_condition -> right);

			printf("	pop rax\n");
			rsp_counter --;
			printf("	cmp rax, 0\n");
			printf("	je .Lend%d\n",endnumber_for);

			generate(node -> right);

			generate(update);

			printf("	jmp .Lbegin%d\n",beginnumber_for);
			printf(".Lend%d:\n",endnumber_for);

			filenumber++;

		}
		return;

	}
	

	//以下は単項でないoperatorのコンパイル===================
	generate(node -> left);
	generate(node -> right);


	printf("	pop rdi\n");// right side of operator
	printf("	pop rax\n");// left side of operator

	
	rsp_counter-=2;

	switch (node -> kind){
	case ND_EQL:
		
		printf("	cmp rax, rdi\n");
		printf("	sete al\n");
		printf("	movzb rax, al\n");
		break;
		
	case ND_NEQ:

		printf("	cmp rax, rdi\n");
		printf("	setne al\n");
		printf("	movzb rax, al\n");
		break;

	case ND_LES:

		printf("	cmp rax, rdi\n");
		printf("	setl al\n");
		printf("	movzb rax, al\n");
		break;

	case ND_LEQ:

		printf("	cmp rax, rdi\n");
		printf("	setle al\n");
		printf("	movzb rax, al\n");
		break;

	case ND_ADD:
		
		printf("	add rax, rdi\n");
		break;

	case ND_SUB:
		
		printf("	sub rax, rdi\n");
		break;
	
	case ND_MUL:
		
		printf("	imul rax, rdi\n");
		break;
	
	case ND_DIV:

		printf("	cqo\n");
		printf("	idiv rdi\n");
		break;
	}
	printf("	push rax\n");
	rsp_counter++;
}
