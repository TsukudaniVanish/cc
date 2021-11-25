#include "cc.h"
//#include<stdio.h>
//#include<stdlib.h>








void gen_lval(Node_t *node){


	if( node -> kind != ND_LVAL ){
		
		
		fprintf(stderr,"代入の左辺値が変数ではありません");
		exit(1);
	
	}else{


		printf("	mov rax, rbp\n");
		printf("	sub rax, %d\n", node -> offset);
		printf("	push rax\n");
	}
}


int filenumber =0;//制御構文で使用する

//抽象構文木からアセンブリコードを生成する
void generate(Node_t *node){

	
	
	switch(node -> kind) {
	case ND_NUM:

		printf("	push %d\n",node ->val);
		return;
	case ND_ASSIGN:

		gen_lval(node->left);
		generate(node -> right);

		printf("	pop rdi\n");
		printf("	pop rax\n");
		printf("	mov [rax], rdi\n");
		printf("	push rdi\n");
		return;
	
	case ND_LVAL:

		gen_lval(node);
		printf("	pop rax\n");
		printf("	mov rax, [rax]\n");
		printf("	push rax\n");
		return;

	case ND_RETURN:

		generate(node -> left);
		printf("	pop rax\n");
		printf("	mov rsp, rbp\n");
		printf("	pop rbp\n");
		printf("	ret\n");
		return;

	case ND_IF:
		generate(node -> left);
		printf("	pop rax\n");
		printf("	cmp rax, 0\n");
		printf("	je  .Lend%d\n",filenumber);

		generate(node -> right);

		printf(".Lend%d:\n",filenumber);
		
		filenumber ++;
		return;

	case ND_ELSE:
		
		if(node -> left -> kind = ND_IFE){
			
			
			generate(node -> left);
			generate(node -> right);
			printf(".Lend%d:\n",filenumber);
			filenumber++;
			return;
		
		}else{
			

			fprintf(stderr,"elseはif(...)...の後に続きます");
			exit(1);
		}

	case ND_IFE:

		generate(node -> left);
		
		printf("	pop rax\n");
		printf("	cmp rax, 0\n");
		printf("	je  .Lelse%d\n",filenumber);

		filenumber++;

		generate(node -> right);
		
		printf("	jmp .Lend%d\n",filenumber);
		printf(".Lelse%d:\n",filenumber -1);
		return;

	case ND_WHILE:

		printf(".Lbegin%d:\n",filenumber);

		generate(node -> left);

		printf("	pop rax\n");
		printf("	cmp rax, 0\n");
		printf("	je	.Lend%d\n",filenumber);
		
		generate(node -> right);
		printf("	pop rax\n");
		printf("	jmp .Lbegin%d\n",filenumber);
		printf(".Lend%d:",filenumber);
		return;
	}
	

	generate(node -> left);
	generate(node -> right);
	
	
	printf("	pop rdi\n");
	printf("	pop rax\n");

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
}
