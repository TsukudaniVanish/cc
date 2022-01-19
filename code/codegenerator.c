#include "cc.h"
//#include<stdio.h>
//#include<stdlib.h>

extern unsigned int String_len(char* );
extern int String_conpair(char* ,char* ,unsigned int);
long int rsp_counter = 0;//use for x86 api / value of rsp must be divided by 16 when you use call instruction
int filenumber = 0; // use for operarion flow




char * get_registername(char *register_name,long int size)
{
	//register names
	char *register_rax[] = {"al","ax","eax","rax"};
	char *register_rdi[] = {"dil","di","edi","rdi"};
	char *register_rsi[] = {"sil","si","esi","rsi"};
	char *register_rdx[] = {"dl","dx","edx","rdx"};
	char *register_rcx[] = {"cl","cx","ecx","rcx"};
	char *register_r8[] = {"r8b","r8w","r8d","r8"};
	char *register_r9[] = {"r9b","r9w","r9d","r9"};
	char* register_r11[] = {"r11b","r11w","r11d","r11"};
	char **arg_register[] = {register_rax,register_rdi , register_rsi , register_rdx , register_rcx , register_r8 , register_r9,register_r11,NULL};
	char *registers [] = {"rax","rdi","rsi","rdx","rcx","r8","r9","r11",NULL};

	//name search
	for(char ** name = registers; *name ; name++)
	{
		if(String_len(*name) <= String_len(register_name) && String_conpair(register_name,*name,String_len(*name)))
		{
			int i = name - registers;
			if( size < 5 && size > 1)
			{
				return arg_register[i][2];
			}
			else if(0 < size && size < 2)
			{
				return arg_register[i][0];
			}
			else
			{
				return arg_register[i][3];
			}
		}
	}
	return NULL;
}




char *get_pointerpref(long int size)
{
	if(size < 5 && 1 < size)
	{
		return "DWORD PTR";
	}
	else if(size == 1)
	{
		return "BYTE PTR";
	}
	else
	{
		return "QWORD PTR";
	}
}




void push_stack(int long size, char * register_name){

	char *name = get_registername(register_name,size);

	if(size < 5 && size > 1)
	{
		printf("	sub rsp, %ld\n",size);
		printf("	mov DWORD PTR [rsp], %s\n",name);
		rsp_counter += 4;
	}
	else if(0 < size && size < 2)
	{
		printf("	sub rsp, %ld\n",size);
		printf("	mov BYTE PTR [rsp], %s\n",name);
		rsp_counter += 1;
	}
	else
	{
		printf("	push %s\n",name);
		rsp_counter += 8;
	}
}

void pop_stack(int long size,char *register_name){

	char *name = get_registername(register_name,size);

	if(size < 5 && size > 1)
	{
		printf("	mov %s, DWORD PTR [rsp]\n",name);
		printf("	add rsp, 4\n");
		rsp_counter -= 4;
	}
	else if(0 < size && size < 2)
	{
		printf("	movsx %s, BYTE PTR [rsp]\n",get_registername(register_name,4));
		printf("	add rsp, 1\n");
		rsp_counter -= 1;
	}
	else
	{
		printf("	pop %s\n",name);
		rsp_counter -= 8;
	}
}

//set value of register to stack
void set_register_to_stack(long int offset,long int size,char* reg)
{
	printf("	mov r11,rbp\n");
	printf("	sub r11, %ld\n",offset);
	printf("	mov %s[r11], %s\n",get_pointerpref(size),get_registername(reg,size));

}




void set_stringiter()
{
	printf("	.section	.rodata\n");
	Lvar *iter = string_iter;
	while (iter)
	{
		printf(".LC%ld:\n",iter -> offset);
		printf("	.string \"%s\"\n",iter -> name);
		iter = iter -> next;
	}
	
}


void set_array_header(){

	Lvar* nametable = *scope;

	if(nametable == NULL)
		return;

	for( Lvar *var = nametable ; var ; var = var -> next ){


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



	if(!is_lval(node)){
		

		fprintf(stderr, "error at code generating\n");
		fprintf(stderr,"	this is not variables\n");
		fprintf(stderr, "generating node kind : %d\n", node -> kind);
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

void gen_arg_entry(Node_t* node){

	Vector *arg_types = make_vector();

	for(int i = 0; node -> kind != ND_BLOCKEND; i++ , node = node -> right){
		generate(node -> left);
		Vector_push(arg_types,node -> left -> tp);
	}
	int len = Vector_get_length(arg_types);

	Type* arg_type;
	char *registers [] = {"rax","rdi","rsi","rdx","rcx","r8","r9",NULL};
	for (int i = len; i >0 ; i--)
	{
		arg_type = Vector_pop(arg_types);
		pop_stack(arg_type -> size,registers[i]);
	}
	
	return;
}



void gen_function_call(Node_t *node){

	gen_arg_entry(node -> left);

	if(rsp_counter%16 !=0)
	{//rsp を調整

		printf("	sub rsp , %ld\n", 16 - rsp_counter % 16);
	}

	printf("	call %s\n",node -> name);
	
	if(rsp_counter%16 !=0)
	{
		printf("	add rsp, %ld\n", 16 - rsp_counter %16);
	}


	push_stack(node -> tp -> size,"rax");
	
	return;
}

void argment_set(int arg_index , long int offset , long int size){

	char *registers [] = {"rax","rdi","rsi","rdx","rcx","r8","r9",NULL};
	//use register set 
	char *use = registers[arg_index];
	set_register_to_stack(offset,size,use);
	return;
}

void gen_function_def(Node_t *node){

	Lvar* nametable = *scope;

	printf("%s:",node -> name);
	int return_rsp_number = rsp_counter;

	//prologue=======================================
	printf("	push rbp\n");
	rsp_counter+= 8;
	printf("	mov rbp ,rsp\n");

	if(nametable){


		printf("	sub rsp, %ld\n",nametable -> offset);
		rsp_counter += nametable ->offset;
	}//=======================================

	Node_t *arg = node -> left;
	long int size[node -> val];
	long int offset[node -> val];
	for(int i = 0 ;arg -> kind != ND_BLOCKEND; i++,arg = arg -> right)
	{	
		size[i] = arg -> left -> tp -> size;
		if(i > 0)
		{
			offset[i] = size[i] + offset[i-1];
			
		}
		else
		{
			offset[i] = size[i];
		}
	}
	//引数代入
	for (size_t i = node -> val; i > 0; i--)
	{
		argment_set(i,offset[i-1],size[i-1]);
	}
	set_array_header();
	

	generate(node -> right);//定義本文をコンパイル

	//epilogue return に書く
	pop_stack(node -> tp -> size,"rax");
	printf("	mov rsp, rbp\n");
	printf("	pop rbp\n");
	rsp_counter -= 8;
	printf("	ret\n");

	return;
}

void gen_global_store(char *name,char *register_name,long int size)
{
	char *regi_name = get_registername(register_name,size);
	if( size < 5 &&  size > 1)
	{
		printf("	mov DWORD PTR %s[rip], %s\n",name,regi_name);
	}
	else if(0 <  size && size < 2)
	{
		printf("	mov BYTE PTR %s[rip], %s\n", name,regi_name);
	}
	else
	{
		printf("	mov QWORD PTR %s[rip], %s\n", name,regi_name);
	}
}




void gen_global_store_arr(char *name,char *register_name,long int size,long int index)
{
	char *regi_name = get_registername(register_name,size);
	if( size < 5 &&  size > 1)
	{
		printf("	mov DWORD PTR %s[rip+%ld], %s\n",name,index,regi_name);
	}
	else if(0 <  size && size < 2)
	{
		printf("	mov BYTE PTR %s[rip+%ld], %s\n",name,index,regi_name);
	}
	else
	{
		printf("	mov QWORD PTR %s[rip+%ld], %s\n",name,index,regi_name);
	}
}

void gen_incement(Node_t* node) {
	if(node -> left == NULL && node -> right != NULL)
	{// postfix
		char *rax = get_registername("rax",node -> right -> tp -> size);
		char *rdi = get_registername("rdi", node -> right -> tp -> size);
		char *pref = get_pointerpref(node -> right -> tp -> size);
		if(node -> right -> kind == ND_DEREF)
			generate(node -> right -> left);
		else
			gen_lval(node -> right);// result of node -> right is stored in rax
		pop_stack(8,"rax");
		printf("	mov rcx, rax\n");
		printf("	mov %s, %s[rax]\n", rax,pref);
		push_stack(node -> right -> tp -> size, "rax");
		printf("	mov %s, %s[rcx]\n", rdi, pref);
		// increment
		int add;
		switch(node -> right -> tp -> Type_label){
		case TP_POINTER: 
			add = node -> right -> tp -> pointer_to -> size;
			break;
		case TP_CHAR:
			add = 1;
			break;
		case TP_INT:
			add = 1;
			break;
		default:
			add = 0;
		}
		printf("	add %s, %d\n", rdi, add);
		printf("	mov %s[rcx], %s\n", pref, rdi);
	}
}

void gen_decrement(Node_t* node) {
	if(node -> left == NULL && node -> right != NULL)
	{// postfix
		char *rax = get_registername("rax",node -> right -> tp -> size);
		char *rdi = get_registername("rdi", node -> right -> tp -> size);
		char *pref = get_pointerpref(node -> right -> tp -> size);
		gen_lval(node -> right);// result of node -> right is stored in rax
		pop_stack(8,"rax");
		printf("	mov rcx, rax\n");
		printf("	mov %s, %s[rax]\n", rax,pref);
		push_stack(node -> right -> tp -> size, "rax");
		printf("	mov %s, %s[rcx]\n", rdi, pref);
		// increment
		int sub;
		switch(node -> right -> tp -> Type_label){
		case TP_POINTER: 
			sub = node -> right -> tp -> pointer_to -> size;
			break;
		case TP_CHAR:
			sub = 1;
			break;
		case TP_INT:
			sub = 1;
			break;
		default:
			sub = 0;
		}
		printf("	sub %s, %d\n", rdi, sub);
		printf("	mov %s[rcx], %s\n", pref, rdi);
	}	// some thing will be written there.
}

char* get_cmpInstruction(Node_kind kind) {
	switch(kind) {
	case ND_EQL : return "sete";
	case ND_NEQ : return "setne";
	case ND_LES : return "setl";
	case ND_LEQ : return "setle";
	default:
		return NULL;
	}
}

void gen_compair(Node_t* node) {
	long size = node -> left -> tp -> size;
	char* rax = get_registername("rax",node -> left -> tp -> size);
	char* rdi = get_registername("rdi", node -> right -> tp -> size);
	char* set = get_cmpInstruction(node -> kind);

	printf("	cmp %s, %s\n",rax,rdi);
	printf("	%s al\n", set);
	if(size > 1)
		printf("	movzb %s, al\n",rax);
	else
		printf("	movzb rax, al\n");
	return;
}

void gen_formula(Node_t *node){

	//サイズ確認
	long size[2] = { node -> left -> tp -> size, node -> right -> tp -> size};
	char *register_name[2];

	// right side of operator
	pop_stack(size[1],"rdi");
	register_name[1] = get_registername("rdi",size[1]);

	// left side of operator
	pop_stack(size[0],"rax");
	register_name[0] = get_registername("rax",size[0]);

	if(size[0] < size[1])
	{
		register_name[0] = get_registername("rax",size[1]);
	}
	else
	{
		register_name[1] = get_registername("rdi",size[0]);
	}
	
	

	
	rsp_counter-=2;

	switch (node -> kind){
	case ND_EQL:
		gen_compair(node);
		break;
		
	case ND_NEQ:

		gen_compair(node);
		break;

	case ND_LES:

		gen_compair(node);
		break;
	case ND_LEQ:

		gen_compair(node);
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
	default:
		return;
	}

	push_stack(size[0],"rax");
}




//抽象構文木からアセンブリコードを生成する
void generate(Node_t *node){

	if(!node)
	{
		return;
	}
	else if(node -> kind == ND_BLOCKEND)
		return;

	
	//ノード末端付近==========================================================
	switch(node -> kind) {
	case ND_NUM:
	
		printf("	sub rsp , 4\n");
		printf("	mov DWORD PTR [rsp], %d\n",node ->val);
		rsp_counter+= 4;
		return;

	case ND_STRINGLITERAL:

		//printf("	sub rsp, 8\n");
		printf("	lea rax, .LC%ld[rip]\n",node -> offset);
		printf("	push rax\n");
		rsp_counter += 8;
		return;

	case ND_ASSIGN:
	{
		long int size[2];
		char *register_name[3];

		
		if( node -> left && node -> left ->  kind != ND_DEREF){

			if(node -> left -> kind == ND_GLOBVALCALL)
			{//グローバル変数処理
				generate(node -> right);
				pop_stack(node -> tp -> size,"rax");
				gen_global_store(node -> left -> name,"rax",node -> tp -> size);
				return;
			}
			gen_lval(node->left);
			size[0] = node -> left -> tp -> size;
		
		}else{
			
			if(node -> left -> left -> kind == ND_ADD)
			{//global 変数処理
				Node_t *node_ll = node -> left -> left;
				if(node_ll -> left -> kind == ND_GLOBVALCALL)
				{
					Type *tp_lll = node_ll -> left -> tp;
					if(tp_lll -> Type_label == TP_POINTER && tp_lll -> pointer_to -> Type_label == TP_ARRAY)
					{
						generate(node -> right);
						pop_stack(node -> tp -> size,"rax");
						gen_global_store_arr(node_ll -> left -> name,"rax",node -> tp -> size,node_ll -> right -> val);
						return;
					}
				}
				else if(node_ll -> right -> kind == ND_GLOBVALCALL)
				{
					Type *tp_llr = node_ll -> right -> tp;
					if(tp_llr -> Type_label == TP_POINTER && tp_llr -> pointer_to -> Type_label == TP_ARRAY)
					{
						generate(node -> right);
						pop_stack(node -> tp -> size,"rax");
						gen_global_store_arr(node_ll -> right -> name,"rax",node -> tp -> size,node_ll -> val);
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
		pop_stack(size[1],"rdi");
		register_name[1] = get_registername("rdi",size[0]);

		// left side of operator
		printf("	pop rax\n");
		rsp_counter -= 8;
		register_name[2] = get_pointerpref(size[0]);
		

		//代入
		printf("	mov %s [rax], %s\n",register_name[2],register_name[1]);
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
		
		generate(node -> left);
		
		char* rax = get_registername("rax",node -> tp -> size);
		char* rcx = get_registername("rcx", node -> tp -> size); 
		char * pointer_pref = get_pointerpref(node -> tp -> size);
		
		pop_stack(node -> left -> tp -> size,"rax");
		printf("	mov %s, %s [rax]\n",rcx,pointer_pref);	
		push_stack(node -> tp -> size,"rcx");
		return;
	}
	case ND_INC: 
		gen_incement(node);
		return;
	case ND_DEC: 
		gen_decrement(node); 
		return;
	case ND_LVAL:
	{

		gen_lval(node);

		char * register_name = get_registername("rax",node -> tp -> size);
		char * pointer_pref = get_pointerpref(node -> tp -> size);
		
		
		printf("	pop rax\n");
		rsp_counter -= 8;
		printf("	mov %s, %s [rax]\n",register_name,pointer_pref);
		
		push_stack(node -> tp -> size,"rax");
		return;
	}

	case ND_GLOBVALCALL://調整=====================
	{
		if(node -> tp && node -> tp -> size < 5 && node -> tp -> size > 1)
		{
			printf("	mov eax, DWORD PTR %s[rip]\n",node -> name);
			printf("	sub rsp, 4\n");
			printf("	mov DWORD PTR [rsp], eax\n");
			rsp_counter += 4;
		}
		else if(node -> tp -> Type_label == TP_POINTER && node -> tp -> pointer_to -> Type_label == TP_ARRAY)
		{
			printf("	lea rax, QWARD PTR %s[rip]\n", node -> name);
			printf("	push rax\n");
			rsp_counter += 8;
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
	case ND_GLOBVALDEF:
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

		while (node ->kind != ND_BLOCKEND)
		{
					
			generate(node -> left);
			node = node ->right;

		}
		
		return;


	case ND_IF:
	{
		
		int size = node -> left -> tp -> size;		

		generate(node -> left);
		pop_stack(size, "rax");
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
		pop_stack(size,"rax");
		
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
		pop_stack(size_l, "rax");
		
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
		pop_stack(size_r, "rax");
		
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
			pop_stack(size, "rax");
			
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
