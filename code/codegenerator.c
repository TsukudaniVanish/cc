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

long gen_lval(Node_t *node){

	if(!is_lval(node)){
		fprintf(stderr, "error at code generating\n");
		fprintf(stderr,"	this is not variables\n");
		fprintf(stderr, "generating node kind : %d\n", node -> kind);
		exit(1);
	}
	if(node -> kind == ND_GLOBVALCALL)
	{
		char* pref = get_pointerpref(node -> tp -> size);
		if(node -> tp -> Type_label == TP_ARRAY)
		{
			printf("	lea rax, %s %s[rip]\n", pref, node -> name);
			push_stack(8,"rax");
			return node -> tp -> size;
		}
		printf("	lea rax, %s %s[rip]\n", pref, node -> name);
		printf("	push rax\n");
		rsp_counter += 8;
		return node -> tp -> size;
	}
	if(node -> kind == ND_DEREF)
	{
		generate(node -> left);
		return node -> tp -> size;
	}
	
	printf("	mov rax, rbp\n");
	printf("	sub rax, %ld\n",  node -> offset);
	printf("	push rax\n");
	rsp_counter += 8;
	return node -> tp -> size;
	
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
	

	generate(node -> right);//定義本文をコンパイル

	//epilogue return に書く
	pop_stack(node -> tp -> size,"rax");
	printf("	mov rsp, rbp\n");
	printf("	pop rbp\n");
	rsp_counter -= 8;
	printf("	ret\n");

	return;
}

int get_inc_dec_registers(Node_t *node,long *size, char** register1, char** register2, char* name_register1, char* name_register2, char** pref) {
	*size = node -> tp -> size;
	*register1 = get_registername(name_register1, *size);
	*register2 = get_registername(name_register2, *size);
	*pref = get_pointerpref(*size);
	int operand;
	switch(node -> tp -> Type_label) {
	case TP_POINTER:
		return node -> tp -> pointer_to -> size;
	case TP_CHAR: return 1;
	case TP_INT: return 1;
	default:
		return 0;
	}
}
void gen_inc_dec(Node_t *node) {
	char* instruction;
	switch(node -> kind) {
	case ND_INC: 
		instruction = "add";
		break;
	case ND_DEC: 
		instruction = "sub";
		break;
	default:
		fprintf(stderr,"conpile error\n	unexpected node at increment, decrement");
		exit(1);
	}
	long size;
	char* rax;
	char* rdi;
	char* pref;
	if(node -> left == NULL && node -> right != NULL)
	{// postfix
		
		int operand = get_inc_dec_registers(node -> right, &size, &rax, &rdi, "rax", "rdi", &pref);
		
		gen_lval(node -> right);// result of node -> right is stored in rax
		
		pop_stack(8,"rax");
		printf("	mov rcx, rax\n");
		printf("	mov %s, %s[rax]\n", rax,pref);

		push_stack(size, "rax");
		
		printf("	mov %s, %s[rcx]\n", rdi, pref);
		printf("	%s %s, %d\n",instruction, rdi, operand);

		printf("	mov %s[rcx], %s\n", pref, rdi);
		return;
	}
	if(node -> right == NULL && node -> left != NULL)
	{// prefix
		int operand = get_inc_dec_registers(node -> left, &size, &rax, &rdi, "rax", "rdi", &pref);	
		gen_lval(node -> left);
		
		pop_stack(8, "rax");
		printf("	mov rcx, rax\n");
		printf("	mov %s, %s[rax]\n", rax, pref);

		printf("	%s %s, %d\n", instruction, rax, operand);
		
		push_stack(size, "rax");

		printf("	mov %s[rcx], %s\n", pref, rax);
		return;
	}
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
	long size_l = node -> left -> tp -> Type_label == TP_ARRAY? 8: node -> left -> tp -> size;
	long size_r = node -> right -> tp -> Type_label == TP_ARRAY? 8: node -> right -> tp -> size;
	long size = size_r > size_l? size_r: size_l;
	char* rax = get_registername("rax", size);
	char* rdi = get_registername("rdi", size);
	char* set = get_cmpInstruction(node -> kind);

	pop_stack(size_r, "rdi");
	pop_stack(size_l, "rax");

	printf("	cmp %s, %s\n",rax,rdi);
	printf("	%s al\n", set);
	if(size_l > 1)
		printf("	movzb %s, al\n",get_registername("rax", size_l));
	else
		printf("	movzb rax, al\n");
	push_stack(size_l, "rax");
	return;
}

char* get_arithmetic_instruction(int kind) {
	switch(kind)
	{
		case ND_ADD : return "add";
		case ND_SUB : return "sub";
		case ND_MUL : return "imul";
		case ND_DIV : return "idiv";
	}
}

//sign extension rax and store to rdx:rax
char* get_sign_extension(long size) {
	if(0 < size && size < 5)
		return "cdq";
	if(5 <size && size < 9)
		return "cqo";
	fprintf(stderr, "invailed size at signExtension\n");
	exit(1);
}

void gen_arithmetic_instruction(Node_t *node) {
	long size_l = node -> left -> tp -> Type_label == TP_ARRAY? 8: node -> left -> tp -> size;
	long size_r = node -> right -> tp -> Type_label == TP_ARRAY? 8: node -> right -> tp -> size;
	long size = size_r > size_l? size_r: size_l;
	char* rax = get_registername("rax",size);
	char* rdi = get_registername("rdi",size);
	char* arithmetic_instruction = get_arithmetic_instruction(node -> kind);

	pop_stack(size_r, "rdi");
	pop_stack(size_l, "rax");

	if(node -> kind == ND_DIV)
	{
		char* singExtension = get_sign_extension(size);
		printf("	%s\n", singExtension);
		printf("	%s %s\n", arithmetic_instruction, rdi);
		push_stack(size, "rax");
		return;
	}

	printf("	%s %s, %s\n",arithmetic_instruction, rax, rdi);
	push_stack(size_l, "rax");
}

void gen_number(int val) {
	printf("	mov eax, %d\n", val);
	push_stack(4, "rax");
	return;
}

void gen_string_literal(long offset) {
	printf("	lea rax, .LC%ld [rip]\n", offset);
	push_stack(8, "rax");
	return;
}

void gen_assign(Node_t* node) {
	long int size[2];
	
	size[0] = gen_lval(node -> left);// generate left node as left value

	size[1] = node -> right -> tp -> size;
	char *rdi = get_registername("rdi",size[0]);
	char* pref = get_pointerpref(size[0]);
	generate(node -> right);

	pop_stack(size[1],"rdi");// right
	pop_stack(8, "rax");// left

	printf("	mov %s [rax], %s\n", pref,rdi);//代入
	push_stack(size[1], "rdi");
	return;
}

void gen_deref(Node_t *node) {
		
	generate(node -> left);
	
	char* rax = get_registername("rax",node -> tp -> size);
	char* rcx = get_registername("rcx", node -> tp -> size); 
	char * pointer_pref = get_pointerpref(node -> tp -> size);
	
	pop_stack(8,"rax");
	printf("	mov %s, %s [rax]\n",rcx,pointer_pref);	
	push_stack(node -> tp -> size,"rcx");
	return;
}

void gen_right_lval(Node_t* node) {
	
	gen_lval(node);
	char * register_name = get_registername("rax",node -> tp -> size);
	char * pointer_pref = get_pointerpref(node -> tp -> size);
	
	
	if(node -> tp -> Type_label == TP_ARRAY) return;
	pop_stack(8, "rax");

	printf("	mov %s, %s [rax]\n",register_name,pointer_pref);
	push_stack(node -> tp -> size,"rax");
	return;
}

void gen_globvar(Node_t* node){
	long size = node -> tp -> size;
	char* rax = get_registername("rax", size);
	char* pref = get_pointerpref(size);

	gen_lval(node);
	if(node -> tp -> Type_label == TP_ARRAY) return;

	pop_stack(8, "rax");
	printf("	mov %s, %s [rax]\n", rax, pref);
	push_stack(size, "rax");
	return;
}

void gen_return(Node_t* node) {
	generate(node);
	if(node != NULL)// is "return;"?
		pop_stack(node -> tp -> size, "rax");
	
	printf("	mov rsp, rbp\n");
	printf("	pop rbp\n");
	rsp_counter -= 8;
	printf("	ret\n");
	return;
}

void gen_glob_declar(Node_t* node) {
	printf("%s:\n",node -> name);
	if(node -> tp -> Type_label == TP_INT)
	{
		if(node -> val == 0)
		{
			printf("	.zero %ld\n",node -> tp -> size);
		
		}
		else
		{
			printf("	.long %d\n",node -> val);
		}
	}
	else
	{
		printf("	.zero %ld\n",node -> tp -> size);
	}
	return;
}

void gen_block(Node_t* node) {
	while (node ->kind != ND_BLOCKEND)
	{
		generate(node -> left);
		node = node ->right;
	}
	return;
}

void gen_if(Node_t* node) {

	long size;		
	int endNumberIf;
	int endNumberElse;
	int elseNumber;
	switch(node -> kind) {
	case ND_IF:
		size = node -> left -> tp -> size;	
		generate(node -> left);
		pop_stack(size, "rax");
		printf("	cmp %s, 0\n", get_registername("rax", size));
		printf("	je  .Lend%d\n",filenumber);
		endNumberIf = filenumber++;
		generate(node -> right);
	
		printf(".Lend%d:\n",endNumberIf);
		
		return;
	case ND_ELSE:
		if( node -> left && node -> left -> kind == ND_IFE)
		{
			generate(node -> left);
			endNumberElse = filenumber++;
			
			generate(node -> right);
			
			printf(".Lend%d:\n",endNumberElse);
			return;
		}
		else
		{
			fprintf(stderr,"elseはif(...)...の後に続きます");
			exit(1);
		}
	case ND_IFE:
		size = node -> left -> tp -> size;
		generate(node -> left);
		pop_stack(size,"rax");
		
		printf("	cmp %s, 0\n", get_registername("rax", size));
		printf("	je  .Lelse%d\n",filenumber);
		elseNumber = filenumber;
		filenumber++;

		generate(node -> right);
		
		printf("	jmp .Lend%d\n",filenumber);

		printf(".Lelse%d:\n",elseNumber);
		return;
	}
	return;
}

void gen_while(Node_t* node) {
	printf(".Lbegin%d:\n",filenumber);
	int beginnumber_while = filenumber++;
	
	generate(node -> left);
	
	int size_l = node -> left -> tp -> size;
	pop_stack(size_l, "rax");
	
	printf("	cmp %s, 0\n", get_registername("rax", size_l));
	printf("	je	.Lend%d\n",filenumber);
	int endnumber_while = filenumber++;

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
	return;
}

void gen_for(Node_t* node) {

	if(!node -> left )
	{//loop will go on
		fprintf(stderr,"for(;;)は無効です");
		exit(1);
	}
	else
	{
		Node_t *conditions = node -> left;
		Node_t *init_condition = conditions -> left;
		Node_t *update = conditions -> right;

		generate(init_condition -> left);

		printf(".Lbegin%d:\n",filenumber);
		int beginnumber_for = filenumber++;
		int endnumber_for = filenumber++;
		
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
		
		printf("	cmp %s, 0\n", get_registername("rax", size));
		printf("	je .Lend%d\n",endnumber_for);

		generate(node -> right);

		generate(update);

		printf("	jmp .Lbegin%d\n",beginnumber_for);
		printf(".Lend%d:\n",endnumber_for);

		filenumber++;

	}
	return;
}

void gen_log_and_or(Node_t* node) {
	long size_l = node -> left -> tp -> size;
	long size_r = node -> right -> tp -> size;
	char* rax_l = get_registername("rax", size_l);
	char* rax_r = get_registername("rax", size_r);

	generate(node -> left);
	pop_stack(size_l, "rax");

	printf("	cmp %s, 0\n", rax_l);
	if(node -> kind == ND_LOGAND)
		printf("	je .Lend%d\n", filenumber);
	else
		printf("	jne .Lend%d\n", filenumber);
	generate(node -> right);
	pop_stack(size_r, "rax");

	printf("	cmp %s, 0\n", rax_r);
	printf(".Lend%d:", filenumber++);
	printf("	setne al\n");
	printf("	movzb %s, al\n", get_registername("rax", 4));
	push_stack(4, "rax");
	return;

}

//抽象構文木からアセンブリコードを生成する
void generate(Node_t *node){

	if(!node) return;

	if(node -> kind == ND_BLOCKEND) return;

	//around an end of the ast tree ==========================================================
	switch(node -> kind) {
	case ND_NUM: gen_number(node -> val);
		return;

	case ND_STRINGLITERAL: gen_string_literal(node -> offset);
		return;

	case ND_ASSIGN: gen_assign(node);
		return;

	case ND_ADDR: gen_lval(node -> left);
		return;

	case ND_DEREF: gen_deref(node);
		return;

	case ND_INC: gen_inc_dec(node);
		return;

	case ND_DEC: gen_inc_dec(node);
		return;
	case ND_LOGAND:
	case ND_LOGOR: gen_log_and_or(node);
		return;

	case ND_LVAL: gen_right_lval(node);
		return;

	case ND_GLOBVALCALL: gen_globvar(node);
		return;
	case ND_FUNCTIONCALL: gen_function_call(node);//function call abi -> System V AMD64 ABI (Linux) 
		return;

	case ND_RETURN: gen_return(node -> left);
		return;

	//around an end of the ast tree ==========================================================

	//around the top of the ast tree ===================================================
	case ND_GLOBVALDEF: 
		return;
	
	case ND_FUNCTIONDEF: gen_function_def(node);
		return;

	case ND_BLOCK: gen_block(node);
		return;
	case ND_IF: 
	case ND_ELSE:
	case ND_IFE: gen_if(node);
		return;
	case ND_WHILE: gen_while(node);
		return;
	case ND_FOR: gen_for(node);
		return;
	//around the top of the ast tree ===================================================
	//以下は単項でないoperatorのコンパイル===================
	case ND_EQL:
	case ND_NEQ:
	case ND_LES:
	case ND_LEQ:
		generate(node -> left);
		generate(node -> right);
		gen_compair(node);
		return;
	case ND_ADD:
	case ND_SUB:
	case ND_MUL:
	case ND_DIV:
		generate(node -> left);
		generate(node -> right);		
		gen_arithmetic_instruction(node);
		return;
	default:
		return;
	}

}
