#include "cc.h"
#include<stdio.h>

//抽象構文木からアセンブリコードを生成する
void generate(Node_t *node){


	if(node -> kind == ND_NUM){


		printf("	push %d\n",node -> val);
	}else{

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
}
