#include "cc.h"
//#include<stdio.h>
//#include<stdlib.h>

extern unsigned int String_len(char*);
extern int String_compare(char* ,char* ,unsigned int);
extern char* String_add(char*, char*);
extern char* i2a(int);
extern char* ui2a(unsigned int);
extern char* l2a(long);
long int rsp_counter = 0;//use for x86 api / value of rsp must be divided by 16 when you use call instruction
int filenumber = 0; // use for operarion flow

extern char* get_label_string_literal(long offset);

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
	case RN_RSP:
		return "rsp";
	case RN_RBP:
		return "rbp";
	case RN_RIP:
		return "rip";
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
		return "DWORD PTR ";
	}
	else if(size == 1)
	{
		return "BYTE PTR ";
	}
	else
	{
		return "QWORD PTR ";
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

char* get_label_string_literal(long offset) {
	return String_add(".LC", l2a(offset)); // LC%ld, offset
}

/**
 * @brief Get the data address with rip
 * 
 * @param pref 
 * @param name 
 * @return char* 
 */
char* get_data_address_with_rip(char* pref, char* name) {
	return String_add(pref, String_add(name, get_pointer("", RN_RIP))); // %s %s[rip], pref, name
}

void sign_extension(long size) {
	char* ins;
	if(0 < size && size < 5) {
		ins = "    cdq";
		printf("%s\n", ins);
		return;
	}
	if(5 <size && size < 9) {
		ins = "    cqo";
		printf("%s\n", ins);
		return;
	}
}

/**
 * @brief move value of src to dst
 * 
 * @param dst 
 * @param src 
 */
void move_data(char* dst, char* src) {
	char* ins = String_add("	mov ", dst);
	ins = String_add(ins, ", ");
	ins = String_add(ins, src);
	ins = String_add(ins, "\n");
	printf("%s", ins);
}

/**
 * @brief move src to dst filling dst with 0.
 * 
 * @param dst 
 * @param src 
 */
void move_data_zero_extension(char* dst, char* src) {
	char* ins = String_add("	movzb ", dst);
	ins = String_add(ins, ", ");
	ins = String_add(ins, src);
	ins = String_add(ins, "\n");
	printf("%s", ins);
}

/**
 * @brief calculate address of src and store it to dst
 * 
 * @param src 
 * @param dst 
 */
void calculate_address(char* dst, char* src) {
	char* operands = String_add(src, "\n");
	operands = String_add(", ", operands);
	operands = String_add(dst, operands);
	char* ins = String_add("	lea ", operands);
	// ins = "	lea %s, %s\n", dst, src
	printf("%s", ins);
	return;
}

void addition(char* dst, char* src) {
	char* ins = String_add("	add ", dst);
	ins = String_add(ins, ", ");
	ins = String_add(ins, src);
	ins = String_add(ins, "\n");
	printf("%s", ins); 
}

void substitution(char* dst, char* src) {
	char* ins = String_add("	sub ", dst);
	ins = String_add(ins, ", ");
	ins = String_add(ins, src);
	ins = String_add(ins, "\n");
	printf("%s", ins);
}

void multiplication(char* dst, char* src) {
	char* ins = String_add("	imul ", dst);
	ins = String_add(ins, ", ");
	ins = String_add(ins, src);
	ins = String_add(ins, "\n");
	printf("%s", ins);
}


/**
 * @brief divide register value with arg. see x86 manual(MASM)
 * 
 * @param arg 
 */
void division(char* arg) {
	char* ins = String_add("	idiv ", arg);
	ins = String_add(ins, "\n");
	printf("%s", ins);
}


/**
 * @brief substitute from rsp and write value to the place pointed by rsp.
 * 
 * @param long_int size
 * @param char* register name
 * @return void
 */
void push_stack(int long size, RegisterName register_name){

	char *name = get_registername(register_name,size);
	if(size < 5 && size > 1)
	{
		substitution(get_registername(RN_RSP, 8), l2a(4));
		move_data(get_pointer(get_pointerpref(4), RN_RSP),name);
		rsp_counter += 4;
	}
	else if(0 < size && size < 2)
	{
		substitution(get_registername(RN_RSP, 8), l2a(size));
		move_data(get_pointer(get_pointerpref(size), RN_RSP),name);
		rsp_counter += 1;
	}
	else
	{
		substitution(get_registername(RN_RSP, 8), l2a(8));
		move_data(get_pointer(get_pointerpref(8), RN_RSP),name);
		rsp_counter += 8;
	}
}

/**
 * @brief load from the place pointed by rsp and add size to rsp
 * 
 * @param long_int size
 * @param char* register name
 * @return void 
 */
void pop_stack(int long size,RegisterName register_name){

	char *name = get_registername(register_name,size);

	if(size < 5 && size > 1)
	{
		move_data(name, get_pointer(get_pointerpref(4), RN_RSP));
		addition(get_registername(RN_RSP, 8), l2a(4));
		rsp_counter -= 4;
	}
	else if(0 < size && size < 2)
	{
		move_data(name, get_pointer(get_pointerpref(1), RN_RSP));
		addition(get_registername(RN_RSP, 8), l2a(1));
		rsp_counter -= 1;
	}
	else
	{
		move_data(name, get_pointer(get_pointerpref(8), RN_RSP));
		addition(get_registername(RN_RSP, 8), l2a(8));
		rsp_counter -= 8;
	}
}

/**
 * @brief Set the register value to stack
 * 
 * @param offset 
 * @param size 
 * @param reg 
 */

void set_register_to_stack(long int offset,long int size,RegisterName reg)
{
	move_data(get_registername(RN_R11, 8), get_registername(RN_RBP, 8));
	substitution(get_registername(RN_R11, 8), l2a(offset));
	move_data(get_pointer(get_pointerpref(size), RN_R11),get_registername(reg,size));
}



/**
 * @brief Set the stringiter object
 * 
 */
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
	if(node -> kind == ND_GLOBVALCALL)
	{
		char* pref = get_pointerpref(node -> tp -> size);
		if(node -> tp -> Type_label == TP_ARRAY)
		{
			calculate_address(get_registername(RN_RAX, 8), get_data_address_with_rip(pref, node -> name));
			push_stack(8,RN_RAX);
			return node -> tp -> size;
		}
		calculate_address(get_registername(RN_RAX, 8), get_data_address_with_rip(pref, node -> name));
		push_stack(8, RN_RAX);
		rsp_counter = rsp_counter + 8;
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

		addition(get_registername(RN_RAX, 8), ui2a(node -> right -> offset));

		push_stack(8, RN_RAX);
		return node -> tp -> size;
	
	}
	move_data(get_registername(RN_RAX, 8), get_registername(RN_RBP, 8));
	substitution(get_registername(RN_RAX, 8), ui2a(node -> offset));

	push_stack(8, RN_RAX);
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
	{// modify rsp place
		substitution(get_registername(RN_RSP, 8), i2a(16 - rsp_counter % 16));
	}

	printf("	call %s\n",node -> name);
	
	if(rsp_counter%16 !=0)
	{
		addition(get_registername(RN_RSP, 8), l2a(16 - rsp_counter %16));
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
	push_stack(8, RN_RBP);
	move_data(get_registername(RN_RBP, 8), get_registername(RN_RSP, 8));

	// allocate memory on stack for arguments
	if(nametable){
		substitution(get_registername(RN_RSP, 8), i2a(nametable -> offset));
		rsp_counter += nametable ->offset;
	}//=======================================

	Node_t *arg = node -> left;
	long int size[7];
	long int offset[7];
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
	// set arguments
	for (long i = node -> val; i > 0; i--)
	{
		argment_set(i,offset[i-1],size[i-1]);
	}
	

	generate(node -> right, 0, 0);

	//epilogue return
	pop_stack(node -> tp -> size,RN_RAX);
	move_data(get_registername(RN_RSP, 8), get_registername(RN_RBP, 8));
	pop_stack(8, RN_RBP);
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
	int instruction = 0;
	switch(node -> kind) {
	case ND_INC: 
		instruction = ND_INC;
		break;
	case ND_DEC: 
		instruction = ND_DEC;
		break;
	default:
		return;
	}
	long size;
	char* rax;
	char* rdi;
	char* pref;
	if(node -> left == NULL && node -> right != NULL)
	{// postfix
		
		int operand = get_inc_dec_registers(node -> right, &size, &rax, &rdi, RN_RAX, RN_RDI, &pref);
		
		gen_lval(node -> right);// result of node -> right is stored in the head of current stack
		
		pop_stack(8,RN_RAX);
		move_data(get_registername(RN_RCX, 8), get_registername(RN_RAX, 8));
		move_data(rax, get_pointer(pref, RN_RAX));

		push_stack(size, RN_RAX);
		
		move_data(rdi, get_pointer(pref, RN_RCX));

		switch (instruction)
		{
		case ND_INC:
			addition(rdi, i2a(operand));
			break;
		case ND_DEC:
			substitution(rdi, i2a(operand));
			break;
		}

		move_data(get_pointer(pref, RN_RCX), rdi);
		return;
	}
	if(node -> right == NULL && node -> left != NULL)
	{// prefix
		int operand = get_inc_dec_registers(node -> left, &size, &rax, &rdi, RN_RAX, RN_RDI, &pref);	
		gen_lval(node -> left);
		
		pop_stack(8, RN_RAX);
		move_data(get_registername(RN_RCX, 8), get_registername(RN_RAX, 8));
		move_data( rax, get_pointer(pref,  RN_RAX));

		switch (instruction)
		{
		case ND_INC:
			addition(rax, i2a(operand));
			break;
		case ND_DEC:
			substitution(rax, i2a(operand));
			break;
		}

		push_stack(size, RN_RAX);

		move_data( get_pointer(pref, RN_RCX),  rax);
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
		move_data_zero_extension(get_registername(RN_RAX, size_l), get_registername(RN_RAX, 1));
	else
		move_data_zero_extension(get_registername(RN_RAX, 8), get_registername(RN_RAX, 1));
	push_stack(size_l, RN_RAX);
	return;
}

void gen_arithmetic_instruction(Node_t *node) {
	long size_l = node -> left -> tp -> Type_label == TP_ARRAY? 8: node -> left -> tp -> size;
	long size_r = node -> right -> tp -> Type_label == TP_ARRAY? 8: node -> right -> tp -> size;
	long size = size_r > size_l? size_r: size_l;
	char* rax = get_registername(RN_RAX,size);
	char* rdi = get_registername(RN_RDI,size);

	pop_stack(size_r, RN_RDI);
	pop_stack(size_l, RN_RAX);

	if(node -> kind == ND_DIV)
	{
		sign_extension(size);
		division(rdi);
		push_stack(size, RN_RAX);
		return;
	}

	switch (node -> kind)
	{
	case ND_ADD:
		addition(rax, rdi);
		break;
	case ND_SUB:
		substitution(rax, rdi);
		break;
	case ND_MUL:
		multiplication(rax, rdi);
		break;
	default:
		break;
	}
	push_stack(size_l, RN_RAX);
}

void gen_number(int val) {
	move_data(get_registername(RN_RAX, 4), i2a(val));
	push_stack(4, RN_RAX);
	return;
}

void gen_string_literal(long offset) {
	calculate_address(get_registername(RN_RAX, 8), get_data_address_with_rip(get_label_string_literal(offset), " "));
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

	move_data( get_pointer(pref, RN_RAX), rdi);//代入
	push_stack(size[1], RN_RDI);
	return;
}

void gen_deref(Node_t *node) {
		
	generate(node -> left, 0, 0);
	
	char* rax = get_registername(RN_RAX,node -> tp -> size);
	char* rcx = get_registername(RN_RCX, node -> tp -> size); 
	char * pointer_pref = get_pointerpref(node -> tp -> size);
	
	pop_stack(8,RN_RAX);
	move_data(rcx,get_pointer(pointer_pref,  RN_RAX));	
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

	move_data(register_name, get_pointer(pointer_pref,  RN_RAX));
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
	move_data( rax, get_pointer(pref,  RN_RAX));
	push_stack(size, RN_RAX);
	return;
}

void gen_return(Node_t* node) {
	generate(node, 0, 0);
	if(node != NULL)// is "return;"?
		pop_stack(node -> tp -> size, RN_RAX);
	
	move_data(get_registername(RN_RSP, 8), get_registername(RN_RBP, 8));
	pop_stack(8, RN_RBP);
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
		// don't accept infinite loop
		return;
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
			move_data( rax,  rcx);

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
	move_data_zero_extension(get_registername(RN_RAX, 4), get_registername(RN_RAX, 1));
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
	move_data(get_registername(RN_RAX, 8), get_registername(RN_RBP, 8));
	substitution(get_registername(RN_RAX, 8), ui2a(offsetTop));
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

			// assign value
			pop_stack(8, RN_RAX);
			move_data(get_pointer( prefix, RN_RAX), get_registername(RN_RDI,  size));
			addition(get_registername(RN_RAX, 8), i2a(size));
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
			move_data( get_pointer(prefix, RN_RAX), get_registername(RN_RDI,  initBranch -> left -> tp -> size));
		
			i++;
			char* memberName = Vector_at(memberNames, i);
			if(memberName != NULL)
			{
				member = Map_at(container, memberName);
				prefix = get_pointerpref(member -> tp -> size);
			}
			// calculate next address
			move_data(get_registername(RN_RAX, 8), get_registername(RN_RBP, 8));
			substitution(get_registername(RN_RAX, 8), ui2a(offsetTop));
			addition(get_registername(RN_RAX, 8), ui2a(member -> offset));
			push_stack(8, RN_RAX);

			initBranch = initBranch -> right;
	}
}

void gen_dot(Node_t* node) {
	gen_lval(node);
	pop_stack(8, RN_RAX);
	move_data( get_registername(RN_RAX, node -> tp -> size), get_pointer(get_pointerpref(node -> tp -> size),  RN_RAX));
	push_stack(node -> tp -> size, RN_RAX);
	return;
}

void gen_log_not(Node_t* node) {
	generate(node -> left, 0, 0);
	pop_stack(node -> left -> tp -> size, RN_RAX);
	printf("	cmp %s, 0\n", get_registername(RN_RAX, node -> left -> tp -> size));
	printf("	sete al\n");
	move_data_zero_extension(get_registername(RN_RAX, 4), get_registername(RN_RAX, 1));
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
