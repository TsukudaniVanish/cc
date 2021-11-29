#include "cc.h"
//#include<stdio.h>
//#include<stdlib.h>



int rsp_counter = 0;




void gen_lval(Node_t *node){


	if( node -> kind != ND_LVAL ){
		
		
		fprintf(stderr,"代入の左辺値が変数ではありません");
		exit(1);
	
	}else{


		printf("	mov rax, rbp\n");
		printf("	sub rax, %d\n", node -> offset);
		printf("	push rax\n");
		rsp_counter++;
	}
}


int filenumber =0;//制御構文で使用する

//抽象構文木からアセンブリコードを生成する
void generate(Node_t *node){

	
	
	switch(node -> kind) {
	case ND_NUM:

		printf("	push %d\n",node ->val);
		rsp_counter++;
		return;
	case ND_ASSIGN:

		gen_lval(node->left);
		generate(node -> right);

		printf("	pop rdi\n");
		rsp_counter--;
		printf("	pop rax\n");
		rsp_counter--;
		printf("	mov [rax], rdi\n");
		printf("	push rdi\n");
		rsp_counter++;
		return;
	
	case ND_LVAL:

		gen_lval(node);
		printf("	pop rax\n");
		rsp_counter--;
		printf("	mov rax, [rax]\n");
		printf("	push rax\n");
		rsp_counter++;
		return;

	case ND_FUNCTIONDEF:

		printf("%s:",node -> name);
		int return_rsp_number = rsp_counter;

		//prologue
		printf("	push rbp\n");
		rsp_counter++;
		printf("	mov rbp ,rsp\n");
		if(locals){
			printf("	sub rsp, %d\n",locals -> offset);
			rsp_counter += locals->offset /8;
		}
		//引数代入
		switch (node->val)
		{
		case 6:
			
			printf("	mov rax, rbp\n");
			printf("	sub rax, %d\n",node->offset + 48);
			printf("	mov [rax], r9\n");
		
		case 5:
			
			printf("	mov rax, rbp\n");
			printf("	sub rax, %d\n",node->offset + 40);
			printf("	mov [rax], r8\n");

		case 4:

			printf("	mov rax, rbp\n");
			printf("	sub rax, %d\n",node->offset + 32);
			printf("	mov [rax], rcx\n");

		case 3:

			printf("	mov rax, rbp\n");
			printf("	sub rax, %d\n",node->offset + 24);
			printf("	mov [rax], rdx\n");

		case 2:

			printf("	mov rax, rbp\n");
			printf("	sub rax, %d\n",node->offset + 16);
			printf("	mov [rax], rsi\n");

		case 1:

			printf("	mov rax, rbp\n");
			printf("	sub rax, %d\n",node->offset + 8);
			printf("	mov [rax], rdi\n");	
			break;
		}
		

		generate(node -> right);

		//epilogue
		printf("	pop rax\n");
		printf("	mov rsp, rbp\n");
		printf("	pop rbp\n");
		printf("	ret\n");

		return;
	
	case ND_FUNCTIONCALL://function call abi -> System V AMD64 ABI (Lunix)

		{//引数読み込み
			Node_t *node_arg = node -> left;
			while (node_arg ->kind != ND_BLOCKEND)
			{
				
				generate(node_arg -> left);
				node_arg = node_arg -> right;

			}
		}

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

		
		if(rsp_counter%2 ==1)
			printf("	sub rsp , 8");
		printf("	call foo\n");
		rsp_counter ++;
		printf("	push rax\n");
		rsp_counter++;
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
		
		if(node -> left -> kind = ND_IFE){
			
			
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

		if(node -> left == NULL){//loop will go on


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

	case ND_BLOCK:

		while (node->right ->kind != ND_BLOCKEND)
		{
			
			generate(node -> left);
			node = node ->right;

		}
		generate(node ->left);
		
		return;
	}
	

	generate(node -> left);
	generate(node -> right);
	
	
	printf("	pop rdi\n");
	printf("	pop rax\n");
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
