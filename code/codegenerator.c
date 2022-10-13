#include "cc.h"
//#include<stdio.h>
//#include<stdlib.h>

extern unsigned int String_len(char*);
extern int String_compare(char* ,char* ,unsigned int);
extern char* new_String(unsigned int);
extern char* String_add(char*, char*);
long int rsp_counter = 0;//use for x86 api / value of rsp must be divided by 16 when you use call instruction
int filenumber = 0; // use for operarion flow


typedef enum {
	RN_RAX,
	RN_RDI,
	RN_RSI,
	RN_RDX,
	RN_RCX,
	RN_R8,
	RN_R9,
	RN_R11,
	RN_RSP, // this register points the head of current stack. 
	RN_RBP, // this register points the base of current stack.
	RN_RIP, // this register points data section.
}RegisterName;


/**
 * @brief Get the register name
 * 
 * @param int 
 * @return char* 
 */
char * get_registername(RegisterName name,long int size)
{
	switch (name)
	{
	case RN_RAX:
	case RN_RDI:
	case RN_RSI:
	case RN_RDX:
	case RN_RCX:
	case RN_R8:
	case RN_R9:
	case RN_R11:
		break;
	default:
		return NULL;
	}
	//register names
	char *register_rax[4] = {"al","ax","eax","rax"};
	char *register_rdi[4] = {"dil","di","edi","rdi"};
	char *register_rsi[4] = {"sil","si","esi","rsi"};
	char *register_rdx[4] = {"dl","dx","edx","rdx"};
	char *register_rcx[4] = {"cl","cx","ecx","rcx"};
	char *register_r8[4] = {"r8b","r8w","r8d","r8"};
	char *register_r9[4] = {"r9b","r9w","r9d","r9"};
	char* register_r11[4] = {"r11b","r11w","r11d","r11"};
	char **arg_register[9] = {register_rax,register_rdi , register_rsi , register_rdx , register_rcx , register_r8 , register_r9,register_r11,NULL};

	if( size < 5 && size > 1)
	{
		return arg_register[name][2];
	}
	else if(0 < size && size < 2)
	{
		return arg_register[name][0];
	}
	else
	{
		return arg_register[name][3];
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

char* get_pointer(char* pref, RegisterName name) {
	char* ptr;
	switch (name)
	{
	case RN_RAX:
		ptr = "[rax]";
		break;
	case RN_RDI:
		ptr = "[rdi]";
		break;
	case RN_RSI:
		ptr = "[rsi]";
		break;
	case RN_RDX:
		ptr = "[rdx]";
		break;
	case RN_RCX:
		ptr = "[rcx]";
		break;
	case RN_R8:
		ptr = "[r8]";
		break;
	case RN_R9:
		ptr = "[r9]";
		break;
	case RN_R11:
		ptr = "[r11]";
		break;
	case RN_RSP:
		ptr = "[rsp]";
		break;
	case RN_RBP:
		ptr = "[rbp]";
		break;
	case RN_RIP:
		ptr = "[rip]";
		break;
	default:
		return NULL;
	}
	return String_add(pref, ptr);
} 



/**
 * @brief rsp の値を引いてrsp の示すアドレスにストア
 * 
 * @param long_int size
 * @param char* register name
 * @return void
 */
void push_stack(int long size, RegisterName register_name){

	char *name = get_registername(register_name,size);

	if(size < 5 && size > 1)
	{
		printf("	sub rsp, %ld\n",size);
		printf("	mov %s, %s\n", get_pointer(get_pointerpref(size), RN_RSP),name);
		rsp_counter += 4;
	}
	else if(0 < size && size < 2)
	{
		printf("	sub rsp, %ld\n",size);
		printf("	mov %s, %s\n", get_pointer(get_pointerpref(size), RN_RSP),name);
		rsp_counter += 1;
	}
	else
	{
		printf("	push %s\n",name);
		rsp_counter += 8;
	}
}

/**
 * @brief rsp の値を足してrspの示すアドレスからロード
 * 
 * @param long_int size
 * @param char* register name
 * @return void 
 */
void pop_stack(int long size,RegisterName register_name){

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
void set_register_to_stack(long int offset,long int size,RegisterName reg)
{
	printf("	mov r11,rbp\n");
	printf("	sub r11, %ld\n",offset);
	printf("	mov %s [r11], %s\n",get_pointerpref(size),get_registername(reg,size));

}




void set_stringiter()
{
	printf("	.section	.rodata\n");
	Lvar *iter = string_iter;
	while (iter)
	{
		printf(".LC%d:\n",iter -> offset);
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
			push_stack(8,RN_RAX);
			return node -> tp -> size;
		}
		printf("	lea rax, %s %s[rip]\n", pref, node -> name);
		printf("	push rax\n");
		rsp_counter += 8;
		return node -> tp -> size;
	}
	if(node -> kind == ND_DEREF)
	{
		generate(node -> left, 0, 0);
		return node -> tp -> size;
	}
	if(node -> kind == ND_DOT)
	{
		gen_lval(node -> left);
		pop_stack(8, RN_RAX);// get top address of struct

		printf("	add rax, %d\n", node -> right -> offset);

		push_stack(8, RN_RAX);
		return node -> tp -> size;
	
	}
	printf("	mov rax, rbp\n");
	printf("	sub rax, %d\n",  node -> offset);
	printf("	push rax\n");
	rsp_counter += 8;
	return node -> tp -> size;
	
}

void gen_arg_entry(Node_t* node){

	Vector *arg_types = make_vector();

	for(int i = 0; node -> kind != ND_BLOCKEND; i++ , node = node -> right){
		generate(node -> left, 0, 0);
		Vector_push(arg_types,node -> left -> tp);
	}
	int len = Vector_get_length(arg_types);

	Type* arg_type;
	RegisterName registers [7] = {RN_RAX,RN_RDI,RN_RSI,RN_RDX,RN_RCX,RN_R8,RN_R9};
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


	push_stack(node -> tp -> size,RN_RAX);
	
	return;
}

void argment_set(int arg_index , long int offset , long int size){

	RegisterName registers [7] = {RN_RAX,RN_RDI,RN_RSI,RN_RDX,RN_RCX,RN_R8,RN_R9};
	//use register set 
	RegisterName use = registers[arg_index];
	set_register_to_stack(offset,size,use);
	return;
}

void gen_function_def(Node_t *node){

	Lvar* nametable = *rootBlock;

	// function header
	printf(".globl %s\n", node -> name);
	printf("	.type %s, @function\n", node -> name);
	printf("	.text\n");

	printf("%s:\n",node -> name);
	int return_rsp_number = rsp_counter;

	//prologue=======================================
	printf("	push rbp\n");
	rsp_counter+= 8;
	printf("	mov rbp ,rsp\n");

	if(nametable){


		printf("	sub rsp, %d\n",nametable -> offset);
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
	

	generate(node -> right, 0, 0);//定義本文をコンパイル

	//epilogue return に書く
	pop_stack(node -> tp -> size,RN_RAX);
	printf("	mov rsp, rbp\n");
	printf("	pop rbp\n");
	rsp_counter -= 8;
	printf("	ret\n");

	// function footer
	printf(".LEnd_%s:\n", node -> name);
	printf("	.size %s, .LEnd_%s - %s\n", node -> name, node -> name, node -> name);

	return;
}

int get_inc_dec_registers(Node_t *node,long *size, char** register1, char** register2, RegisterName name_register1, RegisterName name_register2, char** pref) {
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
		fprintf(stderr,"compile error\n	unexpected node at increment, decrement");
		exit(1);
	}
	long size;
	char* rax;
	char* rdi;
	char* pref;
	if(node -> left == NULL && node -> right != NULL)
	{// postfix
		
		int operand = get_inc_dec_registers(node -> right, &size, &rax, &rdi, RN_RAX, RN_RDI, &pref);
		
		gen_lval(node -> right);// result of node -> right is stored in rax
		
		pop_stack(8,RN_RAX);
		printf("	mov rcx, rax\n");
		printf("	mov %s, %s[rax]\n", rax,pref);

		push_stack(size, RN_RAX);
		
		printf("	mov %s, %s[rcx]\n", rdi, pref);
		printf("	%s %s, %d\n",instruction, rdi, operand);

		printf("	mov %s[rcx], %s\n", pref, rdi);
		return;
	}
	if(node -> right == NULL && node -> left != NULL)
	{// prefix
		int operand = get_inc_dec_registers(node -> left, &size, &rax, &rdi, RN_RAX, RN_RDI, &pref);	
		gen_lval(node -> left);
		
		pop_stack(8, RN_RAX);
		printf("	mov rcx, rax\n");
		printf("	mov %s, %s[rax]\n", rax, pref);

		printf("	%s %s, %d\n", instruction, rax, operand);
		
		push_stack(size, RN_RAX);

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

void gen_compare(Node_t* node) {
	long size_l = node -> left -> tp -> Type_label == TP_ARRAY? 8: node -> left -> tp -> size;
	long size_r = node -> right -> tp -> Type_label == TP_ARRAY? 8: node -> right -> tp -> size;
	long size = size_r > size_l? size_r: size_l;
	char* rax = get_registername(RN_RAX, size);
	char* rdi = get_registername(RN_RDI, size);
	char* set = get_cmpInstruction(node -> kind);

	pop_stack(size_r, RN_RDI);
	pop_stack(size_l, RN_RAX);

	printf("	cmp %s, %s\n",rax,rdi);
	printf("	%s al\n", set);
	if(size_l > 1)
		printf("	movzb %s, al\n",get_registername(RN_RAX, size_l));
	else
		printf("	movzb rax, al\n");
	push_stack(size_l, RN_RAX);
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
	return NULL;
}

void gen_arithmetic_instruction(Node_t *node) {
	long size_l = node -> left -> tp -> Type_label == TP_ARRAY? 8: node -> left -> tp -> size;
	long size_r = node -> right -> tp -> Type_label == TP_ARRAY? 8: node -> right -> tp -> size;
	long size = size_r > size_l? size_r: size_l;
	char* rax = get_registername(RN_RAX,size);
	char* rdi = get_registername(RN_RDI,size);
	char* arithmetic_instruction = get_arithmetic_instruction(node -> kind);

	pop_stack(size_r, RN_RDI);
	pop_stack(size_l, RN_RAX);

	if(node -> kind == ND_DIV)
	{
		char* singExtension = get_sign_extension(size);
		printf("	%s\n", singExtension);
		printf("	%s %s\n", arithmetic_instruction, rdi);
		push_stack(size, RN_RAX);
		return;
	}

	printf("	%s %s, %s\n",arithmetic_instruction, rax, rdi);
	push_stack(size_l, RN_RAX);
}

void gen_number(int val) {
	printf("	mov eax, %d\n", val);
	push_stack(4, RN_RAX);
	return;
}

void gen_string_literal(long offset) {
	printf("	lea rax, .LC%ld [rip]\n", offset);
	push_stack(8, RN_RAX);
	return;
}

void gen_assign(Node_t* node) {
	long int size[2];
	size[0] = gen_lval(node -> left);// generate left node as left value

	size[1] = node -> right -> tp -> size;
	char *rdi = get_registername(RN_RDI,size[0]);
	char* pref = get_pointerpref(size[0]);
	generate(node -> right, 0, 0);

	pop_stack(size[1],RN_RDI);// right
	pop_stack(8, RN_RAX);// left

	printf("	mov %s [rax], %s\n", pref,rdi);//代入
	push_stack(size[1], RN_RDI);
	return;
}

void gen_deref(Node_t *node) {
		
	generate(node -> left, 0, 0);
	
	char* rax = get_registername(RN_RAX,node -> tp -> size);
	char* rcx = get_registername(RN_RCX, node -> tp -> size); 
	char * pointer_pref = get_pointerpref(node -> tp -> size);
	
	pop_stack(8,RN_RAX);
	printf("	mov %s, %s [rax]\n",rcx,pointer_pref);	
	push_stack(node -> tp -> size,RN_RCX);
	return;
}

void gen_right_lval(Node_t* node) {
	if(node -> tp -> Type_label == TP_STRUCT && node -> name == NULL)
	{
		return;
	}
	
	gen_lval(node);
	char * register_name = get_registername(RN_RAX,node -> tp -> size);
	char * pointer_pref = get_pointerpref(node -> tp -> size);
	
	
	if(node -> tp -> Type_label == TP_ARRAY) return;
	pop_stack(8, RN_RAX);

	printf("	mov %s, %s [rax]\n",register_name,pointer_pref);
	push_stack(node -> tp -> size,RN_RAX);
	return;
}

void gen_globvar(Node_t* node){
	long size = node -> tp -> size;
	char* rax = get_registername(RN_RAX, size);
	char* pref = get_pointerpref(size);

	gen_lval(node);
	if(node -> tp -> Type_label == TP_ARRAY) return;

	pop_stack(8, RN_RAX);
	printf("	mov %s, %s [rax]\n", rax, pref);
	push_stack(size, RN_RAX);
	return;
}

void gen_return(Node_t* node) {
	generate(node, 0, 0);
	if(node != NULL)// is "return;"?
		pop_stack(node -> tp -> size, RN_RAX);
	
	printf("	mov rsp, rbp\n");
	printf("	pop rbp\n");
	rsp_counter -= 8;
	printf("	ret\n");
	return;
}

void gen_initialize_glob_variable(Node_t* node) {
	if(node -> tp -> Type_label == TP_INT)
	{
		if(node -> val == 0)
		{
			printf("	.zero %d\n",node -> tp -> size);
		
		}
		else
		{
			printf("	.long %d\n",node -> val);
		}
	}
	else if(node -> kind == ND_STRINGLITERAL)
	{
		printf("	.quad .LC%d\n", node -> offset);
	}
	
}
void gen_glob_declare(Node_t* node) {
	char* name = node -> kind != ND_INITLIST? node -> name: node -> left -> name;
	Type* type = node -> kind != ND_INITLIST? node -> tp: node -> left -> tp;
	int storage_class = node -> kind != ND_INITLIST? node -> storage_class: node -> left -> storage_class;
	// register symbol
	if(storage_class != SC_STATIC)
		printf(".global %s\n", name);
	else
		printf(".local %s\n", name);
	printf("	.type %s, @object\n", name);
	printf("	.size %s, %d\n", name, type -> size);
	printf("%s:\n", name);
	if(node -> kind == ND_INITLIST)
	{
		Node_t* init_branch = node -> right;
		while(init_branch -> kind == ND_BLOCK)
		{
			gen_initialize_glob_variable(init_branch -> left);
			init_branch = init_branch -> right;
		}
		return;
	}
	if(node -> tp -> Type_label == TP_INT)
	{
		if(node -> val == 0)
		{
			printf("	.zero %d\n",node -> tp -> size);
		
		}
		else
		{
			printf("	.long %d\n",node -> val);
		}
	}
	else if(node -> tp -> Type_label == TP_STRUCT && node -> name == NULL)
	{
		return;
	}
	else
	{
		printf("	.zero %d\n",node -> tp -> size);
	}
	return;
}

void gen_block(Node_t* node, int beginLabel, int endLabel) {
	while (node ->kind != ND_BLOCKEND)
	{
		generate(node -> left, beginLabel, endLabel);
		node = node ->right;
	}
	return;
}

void gen_if(Node_t* node, int labelLoopBegin, int labelLoopEnd) {

	long size;		
	int endNumberIf;
	int endNumberElse;
	int elseNumber;
	switch(node -> kind) {
	case ND_IF:
		endNumberIf = filenumber++;

		size = node -> left -> tp -> size;	
		generate(node -> left, labelLoopBegin, labelLoopEnd); // condition 
		pop_stack(size, RN_RAX);
		printf("	cmp %s, 0\n", get_registername(RN_RAX, size));
		printf("	je  .Lend%d\n",endNumberIf);
		generate(node -> right, labelLoopBegin, labelLoopEnd); // if body 
	
		printf(".Lend%d:\n",endNumberIf);
		
		return;
	case ND_ELSE:
		if( node -> left && node -> left -> kind == ND_IFE)
		{
			generate(node -> left, labelLoopBegin, labelLoopEnd); // condition and true part 
			endNumberElse = filenumber++;
			
			generate(node -> right, labelLoopBegin, labelLoopEnd); // false part 
			
			printf(".Lend%d:\n",endNumberElse);
			return;
		}
		
	case ND_IFE:
		size = node -> left -> tp -> size;
		generate(node -> left, labelLoopBegin, labelLoopEnd);
		pop_stack(size,RN_RAX);
		
		printf("	cmp %s, 0\n", get_registername(RN_RAX, size));
		printf("	je  .Lelse%d\n",filenumber);
		elseNumber = filenumber;
		filenumber++;

		generate(node -> right, labelLoopBegin, labelLoopEnd);
		
		printf("	jmp .Lend%d\n",filenumber);

		printf(".Lelse%d:\n",elseNumber);
		return;
	}
	return;
}

void gen_while(Node_t* node) {

	int beginNumberWhile = filenumber++;
	int endNumberWhile = filenumber++;
	
	printf(".Lbegin%d:\n",beginNumberWhile);
	generate(node -> left, beginNumberWhile, endNumberWhile);
	
	int size_l = node -> left -> tp -> size;
	pop_stack(size_l, RN_RAX);
	
	printf("	cmp %s, 0\n", get_registername(RN_RAX, size_l));
	printf("	je	.Lend%d\n",endNumberWhile);
	

	generate(node -> right, beginNumberWhile, endNumberWhile);
	
	printf("	jmp .Lbegin%d\n", beginNumberWhile);
	printf(".Lend%d:\n", endNumberWhile);
	return;
}

void gen_do_while(Node_t* node) {
	int beginNumber = filenumber++;
	int endNumber = filenumber++;

	printf(".Lbegin%d:\n", beginNumber);
	generate(node -> right, beginNumber, endNumber);
	
	generate(node -> left, beginNumber, endNumber);
	int size_l = node -> left -> tp -> size;
	pop_stack(size_l, RN_RAX);
	
	printf("	cmp %s, 0\n", get_registername(RN_RAX, size_l));
	printf("	je	.Lend%d\n", endNumber);
	printf("	jmp .Lbegin%d\n", beginNumber);
	printf(".Lend%d:\n", endNumber);
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
		int beginNumberFor = filenumber++;
		int endNumberFor = filenumber++;

		Node_t *conditions = node -> left;
		Node_t *initCondition = conditions -> left;
		Node_t *update = conditions -> right;

		generate(initCondition -> left, beginNumberFor, endNumberFor);

		printf(".Lbegin%d:\n",beginNumberFor);
		
		
		generate(initCondition -> right, beginNumberFor, endNumberFor);

		int size;
		if(initCondition -> right -> tp)
		{
			size = initCondition -> right -> tp -> size;
		}
		else
		{
			size = 0;
		}
		pop_stack(size, RN_RAX);
		
		printf("	cmp %s, 0\n", get_registername(RN_RAX, size));
		printf("	je .Lend%d\n", endNumberFor);

		generate(node -> right, beginNumberFor, endNumberFor);

		generate(update, beginNumberFor, endNumberFor);

		printf("	jmp .Lbegin%d\n", beginNumberFor);
		printf(".Lend%d:\n", endNumberFor);

		filenumber++;

	}
	return;
}

// store value at rcx. to compare case label with value, we copy value to rax
void gen_switch(Node_t* node) {
	int endLable = filenumber++;
	unsigned int size = node -> left -> tp -> size;
	char* prefix = get_pointerpref(size);
	char* rcx = get_registername(RN_RCX, size);
	char* rdi = get_registername(RN_RDI, size);
	char* rax = get_registername(RN_RAX, size);
	// get value 
	generate(node -> left, 0, endLable);

	int depth = node -> val;
	Node_t* branch = node -> right;
	Node_t* caseBranch = branch -> left;
	Node_t* bodyBranch = branch -> right;
	// case code generation 
	for(int i = 0; i< depth; i++) {
		if(caseBranch -> kind == ND_BLOCKEND || caseBranch -> kind == ND_DEFAULT)
		{
			break;
		} else {
			// get case label 
			generate(caseBranch ->left, 0, endLable);
			pop_stack(caseBranch -> left -> tp -> size, RN_RDI);

			//get value
			pop_stack(size, RN_RCX);
			printf("	mov %s, %s\n", rax, rcx);

			//compare to case value 
			printf("	cmp %s, %s\n", rax, rdi);
			printf("	je .Lcase%d\n", i + endLable);
			push_stack(size, RN_RCX);

			caseBranch = caseBranch -> right;
			continue;
		}
	}
	printf("	jmp .Lend%d\n", endLable);
	// body code generation 
	for(int j = 0; j < depth; j++) {
		if(bodyBranch -> kind == ND_DEFAULT) {
			printf(".Lend%d:\n", endLable);

			Node_t* statement = bodyBranch -> left;
			while (statement -> kind != ND_BLOCKEND)
			{
				generate(statement -> left, 0, endLable);
				statement = statement -> right;
			}
			break;
		} else {
			printf(".Lcase%d:\n", j + endLable);

			Node_t* statement = bodyBranch -> left;
			while (statement -> kind != ND_BLOCKEND)
			{
				generate(statement -> left, 0, endLable);
				statement = statement -> right;
			}

			bodyBranch = bodyBranch -> right;
			continue;
		}
	}
	if(bodyBranch -> kind == ND_BLOCKEND)
	{
		printf(".Lend%d:\n", endLable);
	}
	filenumber += depth;
}

void gen_log_and_or(Node_t* node) {
	long size_l = node -> left -> tp -> size;
	long size_r = node -> right -> tp -> size;
	char* rax_l = get_registername(RN_RAX, size_l);
	char* rax_r = get_registername(RN_RAX, size_r);

	generate(node -> left, 0, 0);
	pop_stack(size_l, RN_RAX);

	printf("	cmp %s, 0\n", rax_l);
	if(node -> kind == ND_LOGAND)
		printf("	je .Lend%d\n", filenumber);
	else
		printf("	jne .Lend%d\n", filenumber);
	generate(node -> right, 0, 0);
	pop_stack(size_r, RN_RAX);

	printf("	cmp %s, 0\n", rax_r);
	printf(".Lend%d:", filenumber++);
	printf("	setne al\n");
	printf("	movzb %s, al\n", get_registername(RN_RAX, 4));
	push_stack(4, RN_RAX);
	return;

}

StructData* get_struct_union_data(int tag, ScopeInfo* scope, char* name) {
	Vector* data = Map_get_all(tagNameSpace, name);
	for(int i = 0; i < Vector_get_length(data); i++)
	{
		StructData* maybeThis = Vector_at(data, i);
		if(ScopeInfo_in_right(maybeThis -> scope, scope) && maybeThis -> tag == tag)
		{
			return maybeThis;
		}
	}
	return NULL;
}

void gen_list_init(Node_t* node) {
	unsigned offsetTop = node -> left -> offset;
	printf("	mov rax, rbp\n");
	printf("	sub rax, %d\n", offsetTop);
	push_stack(8, RN_RAX);

	Node_t* initBranch = node -> right;
	if(node -> left -> tp -> Type_label == TP_ARRAY)
	{
		unsigned size = node -> left -> tp -> pointer_to -> size;
		char* prefix = get_pointerpref(size);
		while(initBranch -> kind == ND_BLOCK)
		{
			generate(initBranch -> left, 0, 0);
			pop_stack(initBranch -> left -> tp -> size, RN_RDI);

			pop_stack(8, RN_RAX);
			printf("	mov %s[rax], %s\n", prefix, get_registername(RN_RDI, initBranch -> left -> tp -> size));
			printf("	add rax, %d\n", size);
			push_stack(8, RN_RAX);
			
			initBranch = initBranch -> right;
		}
		return;
	}
	int tag = node -> left -> tp -> Type_label == TP_STRUCT? TAG_STRUCT: TAG_UNION;
	ScopeInfo* scope = node -> scope;
	StructData *data = get_struct_union_data(tag, scope, node -> left -> tp -> name);
	if(data == NULL)
	{
		error_at(user_input, "fail to find struct or union data");
	}
	Vector* memberNames = data -> memberNames;
	Map* container = data -> memberContainer;
	int i = 0;
	char* memberName = Vector_at(memberNames, i);
	Node_t* member = Map_at(container, memberName);

	char* prefix = get_pointerpref(member -> tp -> size);
	while(initBranch -> kind == ND_BLOCK)
	{
			generate(initBranch -> left, 0, 0);
			pop_stack(initBranch -> left -> tp -> size, RN_RDI);

			pop_stack(8, RN_RAX);
			printf("	mov %s [rax], %s\n", prefix, get_registername(RN_RDI, initBranch -> left -> tp -> size));
		
			i++;
			char* memberName = Vector_at(memberNames, i);
			if(memberName != NULL)
			{
				member = Map_at(container, memberName);
				prefix = get_pointerpref(member -> tp -> size);
			}
			// calculate next address
			printf("	mov rax, rbp\n");
			printf("	sub rax, %d\n", offsetTop);
			printf("	add rax, %d\n", member -> offset);
			push_stack(8, RN_RAX);

			initBranch = initBranch -> right;
	}
}

void gen_dot(Node_t* node) {
	gen_lval(node);
	pop_stack(8, RN_RAX);
	printf("	mov %s, %s [rax]\n", get_registername(RN_RAX, node -> tp -> size),get_pointerpref(node -> tp -> size));
	push_stack(node -> tp -> size, RN_RAX);
	return;
}

void gen_log_not(Node_t* node) {
	generate(node -> left, 0, 0);
	pop_stack(node -> left -> tp -> size, RN_RAX);
	printf("	cmp %s, 0\n", get_registername(RN_RAX, node -> left -> tp -> size));
	printf("	sete al\n");
	printf("	movzb eax, al\n");
	push_stack(4, RN_RAX);
}

void gen_continue(int beginLabel) {
	
	printf("	jmp .Lbegin%d\n", beginLabel);
	
}

void gen_break(int endLabel) {
	
	printf("	jmp .Lend%d\n", endLabel);

}

void gen_conditional_operator(Node_t* node) {
	long condition_size = node -> left -> tp -> size;
	int first_expr = filenumber++;
	int second_expr = filenumber++;

	generate(node -> left, 0, 0);
	pop_stack(condition_size, RN_RAX);
	printf("	cmp %s, 0\n", get_registername(RN_RAX, condition_size));
	printf("	je .Lbegin%d\n", second_expr);
	printf("	jmp .Lbegin%d\n", first_expr);

	
	Node_t* r = node -> right;
	printf(".Lbegin%d:\n", first_expr);
	generate(r -> left, 0, 0);
	printf("	jmp .Lend%d\n", second_expr);
	printf(".Lbegin%d:\n", second_expr);
	generate(r -> right, 0, 0);
	printf(".Lend%d:\n", second_expr);
	return;
}

//抽象構文木からアセンブリコードを生成する
void generate(Node_t *node, int labelLoopBegin, int labelLoopEnd){

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
	case ND_DOT: gen_dot(node);
		return;
	case ND_INITLIST: gen_list_init(node);
		return;
	case ND_LOGAND:
	case ND_LOGOR: gen_log_and_or(node);
		return;
	case ND_LOGNOT: gen_log_not(node);
		return;
	case ND_LVAL: gen_right_lval(node);
		return;
	case ND_CONDITIONAL: gen_conditional_operator(node);
		return;
	case ND_GLOBVALCALL: gen_globvar(node);
		return;
	case ND_FUNCTIONCALL: gen_function_call(node);//function call abi -> System V AMD64 ABI (Linux) 
		return;

	case ND_RETURN: gen_return(node -> left);
		return;
	case ND_BREAK: gen_break(labelLoopEnd);
		return;
	case ND_CONTINUE: gen_continue(labelLoopBegin);
		return;
	//around an end of the ast tree ==========================================================

	//around the top of the ast tree ===================================================
	case ND_GLOBVALDEF: 
		return;
	
	case ND_FUNCTIONDEF: gen_function_def(node);
		return;

	case ND_BLOCK: gen_block(node, labelLoopBegin, labelLoopEnd);
		return;
	case ND_IF: 
	case ND_ELSE:
	case ND_IFE: gen_if(node, labelLoopBegin, labelLoopEnd);
		return;
	case ND_WHILE: gen_while(node);
		return;
	case ND_DO: gen_do_while(node);
		return;
	case ND_FOR: gen_for(node);
		return;
	case ND_SWITCH: gen_switch(node);
		return;
	//around the top of the ast tree ===================================================
	// operators ===================
	case ND_EQL:
	case ND_NEQ:
	case ND_LES:
	case ND_LEQ:
		generate(node -> left, 0, 0);
		generate(node -> right, 0, 0);
		gen_compare(node);
		return;
	case ND_ADD:
	case ND_SUB:
	case ND_MUL:
	case ND_DIV:
		generate(node -> left, 0, 0);
		generate(node -> right, 0, 0);		
		gen_arithmetic_instruction(node);
		return;
	default:
		return;
	}

}
