#include "cc.h"
//#include<stdio.h>
//#include<stdlib.h>



long int rsp_counter = 0;//rsp/8 の値 function callの時 rspを16の倍数にそろえるため
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
		rsp_counter += 8;
	}
}

void gen_function_call(Node_t *node){


	//引数読み込み
	Node_t *node_arg = node -> left;

	Type *arg_types[node -> val];

	int i = 1;

	while (node_arg ->kind != ND_BLOCKEND){
		
		generate(node_arg -> left);

		arg_types[i - 1] = node_arg -> left -> tp; 

		node_arg = node_arg -> right;

		i++;

	}//読み込み終了
		

		switch(node ->val){
		case 6:

			if(arg_types[5] -> size < 5 && arg_types[5] -> size > 1)
			{
				printf("	mov r9d, DWORD PTR [rsp]\n");
				printf("	add rsp, 4\n");
				rsp_counter -=4;
			}
			else
			{
				printf("	pop r9\n");
				rsp_counter -= 8;
			}

		case 5:

			if(arg_types[4] -> size < 5 && arg_types[4] -> size > 1)
			{
				printf("	mov r8d, DWORD PTR [rsp]\n");
				printf("	add rsp, 4\n");
				rsp_counter -= 4;
			}
			else
			{
				printf("	pop r8\n");
				rsp_counter -= 8;
			}

		case 4:

			if(arg_types[3] -> size < 5 && arg_types[3] -> size > 1)
			{
				printf("	mov ecx, DWORD PTR [rsp]\n");
				printf("	add rsp, 4\n");
				rsp_counter -= 4;
			}
			else
			{
				printf("	pop rcx\n");
				rsp_counter -= 8;
			}

		case 3:

			if(arg_types[2] -> size < 5 && arg_types[2] -> size > 1)
			{
				printf("	mov edx, DWORD PTR [rsp]\n");
				printf("	add rsp, 4\n");
				rsp_counter -= 4;
			}
			else
			{
				printf("	pop rdx\n");
				rsp_counter -= 8;
			}

		case 2:

			if(arg_types[1] -> size < 5 && arg_types[1] -> size > 1)
			{
				printf("	mov esi, DWORD PTR [rsp]\n");
				printf("	add rsp, 4\n");
				rsp_counter -= 4;
			}
			else
			{
				printf("	pop rsi\n");
				rsp_counter -= 8;
			}

		case 1:

			if(arg_types[0] -> size < 5 && arg_types[0] -> size > 1)
			{
				printf("	mov edi, DWORD PTR [rsp]\n");
				printf("	add rsp, 4\n");
				rsp_counter -= 4;
			}
			else
			{
				printf("	pop rdi\n");
				rsp_counter -= 8;
			}
		}

		if(rsp_counter%16 !=0){


			printf("	sub rsp , %ld\n", 16 - rsp_counter % 16);
		}
		printf("	call %s\n",node -> name);
		rsp_counter += 8;
		if(node -> tp -> size < 5 && node -> tp -> size > 1)
		{
			printf("	sub rsp, 4\n");
			printf("	mov DWORD PTR [rsp], eax\n");
			rsp_counter += 4;
		}
		else
		{
			printf("	push rax\n");
			rsp_counter += 8;
		}
		return;
}

void gen_function_def(Node_t *node){


	printf("%s:",node -> name);
	int return_rsp_number = rsp_counter;

	//prologue=======================================
	printf("	push rbp\n");
	rsp_counter+= 8;
	printf("	mov rbp ,rsp\n");

	if(nametable && nametable ->locals){


		printf("	sub rsp, %ld\n",nametable ->locals -> offset);
		rsp_counter += nametable ->locals->offset;
	}//=======================================

	Node_t *arg = node -> left;
	long int size[node -> val];
	int i = 0;
	while (arg -> kind != ND_BLOCKEND)
	{	
		if(i > 0)
		{
			size[i] = arg -> left -> tp -> size + size[i-1];
		}
		else
		{
			size[i] = arg -> left -> tp -> size;
		}
			
		i++;
		arg = arg -> right;
	}
	
	//引数代入
	switch (node->val)
	{
	case 6:
		
		printf("	mov rax, rbp\n");
		printf("	sub rax, %ld\n",size[5] );
		if(size[5] < 5 && size[5] >1)
		{
			printf("	mov DWORD PTR [rax], r9d\n");
		}
		else
		{
			printf("	mov QWORD PTR [rax], r9\n");
		}
	
	case 5:
		
		printf("	mov rax, rbp\n");
		printf("	sub rax, %ld\n",size[4] );
		if(size[4] < 5 && size[4] >1)
		{
			printf("	mov DWORD PTR [rax], r8d\n");
		}
		else
		{
			printf("	mov QWORD PTR [rax], r8\n");
		}


	case 4:

		printf("	mov rax, rbp\n");
		printf("	sub rax, %ld\n" , size[3]);
		if(size[3] < 5 && size[3] > 1)
		{
			printf("	mov DWORD PTR [rax], ecx\n");
		}
		else
		{
			printf("	mov QWORD PTR [rax], rcx\n");
		}

	case 3:

		printf("	mov rax, rbp\n");
		printf("	sub rax, %ld\n",size[2] );
		if(size[2] < 5 && size[2] > 1)
		{
			printf("	mov DWORD PTR [rax], edx\n");
		}
		else
		{
			printf("	mov QWORD PTR [rax], rdx\n");
		}

	case 2:

		printf("	mov rax, rbp\n");
		printf("	sub rax, %ld\n" , size[1]);
		if(size[1] < 5 && size[1] > 1)
		{
			printf("	mov DWORD PTR [rax], esi\n");
		}
		else
		{
			printf("	mov [rax], rsi\n");
		}

	case 1:

		printf("	mov rax, rbp\n");
		printf("	sub rax, %ld\n" , size[0]);
		if(size[0] < 5 && size[0] > 1)
		{
			printf("	mov DWORD PTR [rax], edi\n");
		}
		else
		{
			printf("	mov [rax], rdi\n");	
		}
		
		break;
	}

	set_array_header();
	

	generate(node -> right);//定義本文をコンパイル

	//epilogue return に書く
	if(node -> tp -> size < 5)
	{
		printf("	mov eax, DWORD PTR [rsp]\n");
		rsp_counter -= 4;
	}
	else
	{
		printf("	pop rax\n");
		rsp_counter -= 8;
	}
	printf("	mov rsp, rbp\n");
	printf("	pop rbp\n");
	rsp_counter -= 8;
	printf("	ret\n");

	return;
}



void gen_formula(Node_t *node){

	//サイズ確認
	long size[2] = { node -> left -> tp -> size, node -> right -> tp -> size};
	char *register_name[2];

	// right side of operator
	if(size[1] < 5 && size[1] > 1 )
	{
		printf("	mov edi, DWORD PTR [rsp]\n");
		printf("	add rsp, %ld\n",size[1]);
		register_name[1] = "edi";
		rsp_counter -= 4;
		
	}
	else
	{
		printf("	pop rdi\n");
		register_name[1] = "rdi";
		rsp_counter -= 8;
	}

	// left side of operator
	if(size[0] < 5 && size[0] > 1)
	{
		printf("	mov eax, DWORD PTR[rsp]\n");
		printf("	add rsp, %ld\n",size[0]);
		register_name[0] = "eax";
		rsp_counter -= 4;
	}
	else
	{
		printf("	pop rax\n");
		register_name[0] = "rax";
		rsp_counter -= 8;
	}

	//サイズ合わせ
	if(size[0] < size[1])
	{
		printf("	mov ecx, eax\n");
		printf("	mov eax, ecx\n");
		register_name[0] = "rax";
	}
	else if( size[0] > size[1] )
	{
		printf("	mov ecx, edi\n");
		printf("	mov edi, ecx\n");
		register_name[1] = "rdi";
	}
	
	

	
	rsp_counter-=2;

	switch (node -> kind){
	case ND_EQL:
		
		printf("	cmp %s, %s\n",register_name[0],register_name[1]);
		printf("	sete al\n");
		printf("	movzb %s, al\n",register_name[0]);
		break;
		
	case ND_NEQ:

		printf("	cmp %s, %s\n",register_name[0],register_name[1]);
		printf("	setne al\n");
		printf("	movzb %s, al\n",register_name[0]);
		break;

	case ND_LES:

		printf("	cmp %s, %s\n",register_name[0],register_name[1]);
		printf("	setl al\n");
		printf("	movzb %s, al\n",register_name[0]);
		break;

	case ND_LEQ:

		printf("	cmp %s, %s\n",register_name[0],register_name[1]);
		printf("	setle al\n");
		printf("	movzb %s, al\n",register_name[0]);
		break;

	case ND_ADD:
		
		printf("	add %s, %s\n",register_name[0],register_name[1]);
		break;

	case ND_SUB:
		
		printf("	sub %s, %s\n",register_name[0],register_name[1]);
		break;
	
	case ND_MUL:
		
		printf("	imul %s, %s\n",register_name[0],register_name[1]);
		break;
	
	case ND_DIV:

		if( size[0] < 5 )
		{
			printf("	cdq\n");
		}
		else if( size[0] < 9 )
		{
			printf("	cqo\n");
		}
		printf("	idiv %s\n",register_name[1]);
		break;
	}

	if(size[0] < 5 && size[0] >1 )
	{
		printf("	sub rsp, %ld\n",size[0]);
		printf("	mov DWORD PTR [rsp], eax\n");
		rsp_counter += 4;
		return;
	}
	else
	{
		printf("	push rax\n");
		rsp_counter += 8;
		return;
	}
}




//抽象構文木からアセンブリコードを生成する
void generate(Node_t *node){

	if(!node)
		return;

	
	//ノード末端付近==========================================================
	switch(node -> kind) {
	case ND_NUM:

		printf("	sub rsp , 4\n");
		printf("	mov DWORD PTR [rsp], %d\n",node ->val);
		rsp_counter+= 4;
		return;
	case ND_ASSIGN:
	{
		long int size[2];
		char *register_name[3];

		if( node -> left && node -> left ->  kind != ND_DEREF){

			if(node -> left -> kind == ND_GLVALCALL)
			{//グローバル変数処理
				generate(node -> right);
				printf("	pop rax\n");
				printf("	mov QWORD PTR %s[rip], rax\n",node -> left -> name);
				return;
			}
			gen_lval(node->left);
			size[0] = node -> left -> tp -> size;
		
		}else{
			
			if(node -> left -> left -> kind == ND_ADD)
			{//global 変数処理
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
			size[0] = node -> left -> left -> tp -> pointer_to -> size;
		}	
		
		generate(node -> right);
		size[1] = node -> right -> tp -> size;


		// right side of operator
		if(size[1] < 5 && size[1] > 1 )
		{
			printf("	mov edi, DWORD PTR [rsp]\n");
			printf("	add rsp, %ld\n",size[1]);
			register_name[1] = "edi";
			rsp_counter -= 4;
			
		}
		else
		{
			printf("	pop rdi\n");
			register_name[1] = "rdi";
			rsp_counter -= 8;
		}


		// left side of operator
		printf("	pop rax\n");
		rsp_counter -= 8;
		if(size[0] < 5 && size[0] > 1)
		{
			register_name[0] = "rax";
			register_name[2] = "DWORD PTR";
		}
		else
		{
			register_name[0] = "rax";
			register_name[2] = "QWORD PTR";
		}
		printf("	mov %s [%s], %s\n",register_name[2],register_name[0],register_name[1]);
		printf("	sub rsp, %ld\n",node -> right -> tp -> size );
		printf("	mov %s [rsp], %s\n",register_name[2],register_name[1]);
		rsp_counter++;
		return;
	}
	case ND_ADDR:

		gen_lval(node -> left);
		return;

	case ND_DEREF:
	{

		if(node -> left -> kind == ND_ADD)
		{// global 変数処理

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
		
		char * register_name;
		char * pointer_size;
		if(node -> tp -> size < 5 && node -> tp -> size > 1){
			register_name = "ecx";
			pointer_size = "DWORD PTR";
		}
		else
		{
			register_name = "rcx";
			pointer_size = "QWORD PTR";
		}
		
		printf("	pop rax\n");
		rsp_counter -= 8;
		printf("	mov %s, %s [rax]\n",register_name,pointer_size);
		if(node -> tp -> size < 5 && node -> tp -> size > 1)
		{
			printf("	mov eax, ecx\n");
		}
		else
		{
			printf("	mov rax, rcx\n");
		}
		
		if(node -> tp -> size < 5 && node -> tp -> size > 1)
		{
			printf("	sub rsp, 4\n");
			printf("	mov DWORD PTR [rsp], eax\n");
			rsp_counter += 4;
		}
		else
		{
			printf("	push rax\n");
			rsp_counter += 8;
		}
		return;
	}
	
	case ND_LVAL:
	{

		gen_lval(node);

		char * register_name;
		char * pointer_size;
		if(node -> tp -> size < 5 && node -> tp -> size > 1){
			register_name = "eax";
			pointer_size = "DWORD PTR";
		}
		else
		{
			register_name = "rax";
			pointer_size = "QWORD PTR";
		}
		
		printf("	pop rax\n");
		rsp_counter -= 8;
		printf("	mov %s, %s [rax]\n",register_name,pointer_size);
		
		if(node -> tp -> size < 5 && node -> tp -> size > 1)
		{
			printf("	sub rsp, 4\n");
			printf("	mov DWORD PTR [rsp], eax\n");
			rsp_counter += 4;
		}
		else
		{
			printf("	push rax\n");
			rsp_counter += 8;
		}
		return;
	}

	case ND_GLVALCALL://調整=====================
	{
		if(node -> tp && node -> tp -> size < 5 && node -> tp -> size > 1)
		{
			printf("	mov eax, DWORD PTR %s[rip]\n",node -> name);
			printf("	sub rsp, 4\n");
			printf("	mov DWORD PTR [rsp], eax\n");
			rsp_counter += 4;
		}
		else
		{
			printf("	mov rax, QWORD PTR %s[rip]\n",node -> name);
			printf("	push rax\n");
			rsp_counter += 8;
		}
		return;
	}//調整=====================

	case ND_FUNCTIONCALL://function call abi -> System V AMD64 ABI (Lunix)

		gen_function_call(node);
		return;

	case ND_RETURN:
		
		generate(node -> left);

		if(node -> left -> tp && node -> left -> tp -> size < 5)
		{
			printf("	mov eax, DWORD PTR [rsp]\n");
		}
		else
		{
			printf("	pop rax\n");
			rsp_counter-= 8;
		}

		
		printf("	mov rsp, rbp\n");
		printf("	pop rbp\n");
		rsp_counter -= 8;
		printf("	ret\n");
		return;

	}//ノード末端付近==========================================================

	//ノード先端付近 ===================================================
	switch(node -> kind){
	case ND_GLVALDEF:
	{//調整=====================

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
	}//調整=====================
	
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
	{
		
		int size = node -> left -> tp -> size;		

		generate(node -> left);

		if(size < 5 && size > 1)
		{
			printf("	mov eax, DWORD PTR [rsp]\n");
			printf("	add rsp, 4\n");
			rsp_counter -= 4;
		}
		else
		{
			printf("	pop rax\n");
			rsp_counter -= 8;
		}
		rsp_counter --;
		printf("	cmp rax, 0\n");
		printf("	je  .Lend%d\n",filenumber);
		int endnumber_if = filenumber;

		generate(node -> right);

		printf(".Lend%d:\n",endnumber_if);
		
		filenumber ++;
		return;
	}

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
	{
		generate(node -> left);
		int size = node -> left -> tp -> size;
		if(size < 5 && size > 1)
		{
			printf("	mov eax, DWORD PTR [rsp]\n");
			printf("	add rsp, 4\n");
			rsp_counter -= 4;
		}
		else
		{
			printf("	pop rax\n");
			rsp_counter -= 8;
		}
		printf("	cmp rax, 0\n");
		printf("	je  .Lelse%d\n",filenumber);
		int elsenumber = filenumber;
		filenumber++;

		generate(node -> right);
		
		printf("	jmp .Lend%d\n",filenumber);

		printf(".Lelse%d:\n",elsenumber);
		return;
	}

	case ND_WHILE:
	{
		printf(".Lbegin%d:\n",filenumber);
		int beginnumber_while = filenumber;
		
		generate(node -> left);

		int size_l = node -> left -> tp -> size;
		if(size_l < 5 && size_l > 1)
		{
			printf("	mov eax, DWORD PTR [rsp]\n");
			printf("	add rsp, 4\n");
			rsp_counter -= 4;
		}
		else
		{
			printf("	pop rax\n");
			rsp_counter -= 8;
		}
		printf("	cmp rax, 0\n");
		printf("	je	.Lend%d\n",filenumber);
		int endnumber_while = filenumber;

		generate(node -> right);

		int size_r;
		if( node -> right -> tp)
		{
			size_r = node -> right -> tp -> size;
		}
		else
		{
			size_r = 0;
		}
		if(size_r < 5 && size_r > 1)
		{
			printf("	mov eax, DWORD PTR [rsp]\n");
			printf("	add rsp, 4\n");
			rsp_counter -= 4;
		}
		else
		{
			printf("	pop rax\n");
			rsp_counter -= 8;
		}
		printf("	jmp .Lbegin%d\n",beginnumber_while);
		printf(".Lend%d:\n",endnumber_while);
		filenumber++;
		return;
	}

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

			int size;
			if(init_condition -> right -> tp)
			{
				size = init_condition -> right -> tp -> size;
			}
			else
			{
				size = 0;
			}
			if(size < 5 && size > 1)
			{
				printf("	mov eax, DWORD PTR [rsp]\n");
				printf("	add rsp, 4\n");
				rsp_counter -= 4;
			}
			else
			{
				printf("	pop rax\n");
				rsp_counter -= 8;
			}
			printf("	cmp rax, 0\n");
			printf("	je .Lend%d\n",endnumber_for);

			generate(node -> right);

			generate(update);

			printf("	jmp .Lbegin%d\n",beginnumber_for);
			printf(".Lend%d:\n",endnumber_for);

			filenumber++;

		}
		return;

	}//ノード先端付近 ===================================================
	

	//以下は単項でないoperatorのコンパイル===================
	generate(node -> left);
	generate(node -> right);


	gen_formula(node);
}
