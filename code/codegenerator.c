#include "cc.h"
//#include<stdio.h>
//#include<stdlib.h>

extern int printf(char* fmt, ...);
extern void exit(int);
static void* NULL = (void*) 0;

extern unsigned int String_len(char*);
extern int String_compare(char* ,char* ,unsigned int);
extern char* String_add(char*, char*);
extern char* i2a(int);
extern char* ui2a(unsigned int);
extern char* l2a(long);
static long int stack_depth;//use for x86 api / value of rsp must be divided by 16 when you use call instruction
static int filenumber = 0; // use for operarion flow

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

char* get_label_file_scope(char* name) {
	return String_add(".L", name);
}

void label(char* label) {
	char* s = String_add(label, ":\n");
	printf("%s", s);
}

void section_text() {
	char* l = "	.text\n";
	printf("%s", l);
}

void section_read_only() {
	char* l = ".section    .rodata\n";
	printf("%s", l);
} 

void section_comm(char* name, unsigned size, unsigned align) {
	printf("	.comm %s, %d, %d\n", name, size, align);
}

void section_global(char* name) {
	char* l = String_add(".global ", name);
	l = String_add(l, "\n");
	printf("%s", l);
}

void section_local(char* name) {
	char* l = String_add(".local ", name);
	l = String_add(l, "\n");
	printf("%s", l);
}

void section_type(char* name, char* at) {
	char* l = String_add("	.type ", name);
	l = String_add(l, ", ");
	l = String_add(l, at);
	l = String_add(l, "\n");
	printf("%s", l);
}

void section_size(char* name, char* size) {
	char* l = String_add("	.size ", name);
	l = String_add(l, ", ");
	l = String_add(l, size);
	l = String_add(l , "\n");
	printf("%s", l);
}

void section_zero(long size) {
	char* l = String_add("	.zero ", l2a(size));
	l = String_add(l, "\n");
	printf("%s", l);
}

void section_long(long size) {
	char* l = String_add("	.long ", l2a(size));
	l = String_add(l, "\n");
	printf("%s", l);
}

void section_quad(char* arg) {
	char* l = String_add("	.quad ", arg);
	l = String_add(l, "\n");
	printf("%s", l);
}

void section_string(char* name) {
	char* string = String_add("    .string \"", name);
	string = String_add(string, "\"\n");
	printf("%s",string);
}

/**
 * @brief set string literal at label .LC(offset)
 * 
 * @param offset 
 * @param name 
 */
void data_string_literal(long offset, char* name) {
	label(get_label_string_literal(offset));

	section_string(name);
	printf("	.align 8\n");
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
		ins = "    cdq\n";
		printf("%s", ins);
		return;
	}
	if(5 <size && size < 9) {
		ins = "    cqo\n";
		printf("%s", ins);
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

void compare_value(char* dst, char* src) {
	char* ins = String_add("	cmp ", dst);
	ins = String_add(ins, ", ");
	ins = String_add(ins, src);
	ins = String_add(ins, "\n");
	printf("%s", ins);
}

/**
 * @brief Set 1 to arg( byte size register) if equal
 *  @param arg
 */
void set_equal(char* arg) {
	char* ins = String_add("	sete ", arg);
	ins = String_add(ins, "\n");
	printf("%s", ins);
}

/**
 * @brief Set 1 to arg( byte size register) if not equal
 *  @param arg
 */
void set_not_equal(char* arg) {
	char* ins = String_add("	setne ", arg);
	ins = String_add(ins, "\n");
	printf("%s", ins);
}

/**
 * @brief Set 1 to arg( byte size register) if > holds.
 *  @param arg
 */
void set_l(char* arg) {
	char* ins = String_add("	setl ", arg);
	ins = String_add(ins, "\n");
	printf("%s", ins);
}

/**
 * @brief Set 1 to arg( byte size register) if >= holds.
 *  @param arg
 */
void set_le(char* arg) {
	char* ins = String_add("	setle ", arg);
	ins = String_add(ins, "\n");
	printf("%s", ins);
}

/**
 * @brief jump to the label
 * 
 * @param label 
 */
void jump(char* label) {
	char* ins = String_add("	jmp ", label);
	ins = String_add(ins, "\n");
	printf("%s", ins);
}

void jump_equal(char* label) {
	char* ins = String_add("	je ", label);
	ins = String_add(ins, "\n");
	printf("%s", ins);
}

void jump_not_equal(char* label) {
	char* ins = String_add("	jne ", label);
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
void push_stack( RegisterName register_name, int long size){

	char *name = get_registername(register_name,SIZEOF_POINTER);
	if(size == 1) {
		move_data_zero_extension(name, get_registername(register_name, size));
	}
	printf("	push %s\n", name);
	stack_depth++;
}

/**
 * @brief load from the place pointed by rsp and add size to rsp
 * 
 * @param long_int size
 * @param char* register name
 * @return void 
 */
void pop_stack(RegisterName register_name, int long size){

	char *name = get_registername(register_name,SIZEOF_POINTER);
	printf("	pop %s\n", name);
	stack_depth--;
}

void call(char* name, StorageClass storage) {
	char* ins = storage == SC_EXTERN? String_add("	call ", String_add(name, "@PLT")):String_add("	call ", name);
	ins = String_add(ins, "\n");
	printf("%s", ins);
}

void leave() {
	printf("	leave \n");
}

void ret() {
	char* ins = "	ret\n";
	printf("%s", ins);
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
	move_data(get_registername(RN_R11, SIZEOF_POINTER), get_registername(RN_RBP, SIZEOF_POINTER));
	substitution(get_registername(RN_R11, SIZEOF_POINTER), l2a(offset));
	move_data(get_pointer(get_pointerpref(size), RN_R11),get_registername(reg,size));
}



/**
 * @brief Set the stringiter object
 * 
 */
void set_stringiter()
{
	section_read_only();

	Lvar *iter = string_literal;
	while (iter)
	{
		data_string_literal(iter -> offset, iter -> name);
		iter = iter -> next;
	}
	
}

long gen_lval(Node_t *node){
	if(node -> kind == ND_GLOBVALCALL)
	{
		char* pref = get_pointerpref(node -> tp -> size);
		if(node -> tp -> Type_label == TP_ARRAY)
		{
			calculate_address(get_registername(RN_RAX, SIZEOF_POINTER), get_data_address_with_rip(pref, node -> name));
			push_stack(RN_RAX, SIZEOF_POINTER);
			return node -> tp -> size;
		}
		calculate_address(get_registername(RN_RAX, SIZEOF_POINTER), get_data_address_with_rip(pref, node -> name));
		push_stack( RN_RAX, SIZEOF_POINTER);
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
		pop_stack( RN_RAX, SIZEOF_POINTER);// get top address of struct

		addition(get_registername(RN_RAX, SIZEOF_POINTER), ui2a(node -> right -> offset));
		push_stack( RN_RAX, SIZEOF_POINTER);
		return node -> tp -> size;
	
	}
	move_data(get_registername(RN_RAX, SIZEOF_POINTER), get_registername(RN_RBP, SIZEOF_POINTER));
	substitution(get_registername(RN_RAX, SIZEOF_POINTER), ui2a(node -> offset));

	push_stack( RN_RAX, SIZEOF_POINTER);
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
		pop_stack(registers[i], arg_type -> size);
	}
	
	return;
}


void gen_function_call(Node_t *node){

	gen_arg_entry(node -> left);

	if(stack_depth%2 == 1)
	{// modify rsp place
		substitution(get_registername(RN_RSP, SIZEOF_POINTER), i2a(8));
	}

	move_data(get_registername(RN_RAX, 4), i2a(0));
	call(node -> name, node -> storage_class);
	
	if(stack_depth%2 == 1)
	{
		addition(get_registername(RN_RSP, SIZEOF_POINTER), l2a(8));
	}


	push_stack(RN_RAX, node -> tp -> size);
	
	return;
}

void argment_set(int arg_index , long int offset , long int size){

	RegisterName registers [7] = {RN_RAX,RN_RDI,RN_RSI,RN_RDX,RN_RCX,RN_R8,RN_R9};
	//use register set 
	RegisterName use = registers[arg_index];
	set_register_to_stack(offset,size,use);
	return;
}

char* function_header(char* name, StorageClass class) {
	
	if(class == SC_STATIC) {
		section_local(name);
	} else {
		// .globl %s\n, name
		section_global(name);
	}
	
	
	// .type %s, @function\n, name
	section_type(name, "@function");

	// .text\n
	section_text();

	// %s:\n, name
	label(name);
}

char* function_footer(char* name) {
	char* end_label = get_label_file_scope(String_add("End_", name));
	
	// .LEnd_%s:\n, name
	label(end_label);

	//     .size %s, .LEnd_$s - %s\n, name, name, name
	char* size_info = String_add(end_label, " - ");
	size_info = String_add(size_info, name);
	section_size(name, size_info);
}

void gen_function_def(Node_t *node){
	stack_depth = 1;
	Lvar* nametable = *rootBlock;

	// function header
	function_header(node -> name, node -> storage_class);
	int return_rsp_number = stack_depth;

	//prologue=======================================
	push_stack( RN_RBP, SIZEOF_POINTER);
	move_data(get_registername(RN_RBP, SIZEOF_POINTER), get_registername(RN_RSP, SIZEOF_POINTER));

	// allocate memory on stack for arguments
	if(nametable != NULL){
		long stack_length = nametable -> offset + (8 - (nametable -> offset % 8));
		substitution(get_registername(RN_RSP, SIZEOF_POINTER), i2a(stack_length));
		stack_depth = stack_length / 8;
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
	pop_stack(RN_RAX, node -> tp -> size);
	leave();
	ret();

	// function footer
	function_footer(node -> name);

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
	case TP_LONG:
	case TP_INT: return 1;
	case TP_ENUM: return 1;
	default:
		return 0;
	}
}
// fix adding enum constant
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
		
		pop_stack(RN_RAX, SIZEOF_POINTER);
		move_data(get_registername(RN_RCX, SIZEOF_POINTER), get_registername(RN_RAX, SIZEOF_POINTER));
		move_data(rax, get_pointer(pref, RN_RAX));

		push_stack( RN_RAX, size);
		
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
		
		pop_stack( RN_RAX, SIZEOF_POINTER);
		move_data(get_registername(RN_RCX, SIZEOF_POINTER), get_registername(RN_RAX, SIZEOF_POINTER));
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

		push_stack( RN_RAX, size);

		move_data( get_pointer(pref, RN_RCX),  rax);
		return;
	}
}

void set_instruction(char* arg, Node_kind kind) {
	switch(kind) {
	case ND_EQL :
		set_equal(arg); 
		return;
	case ND_NEQ : 
		set_not_equal(arg);
		return;
	case ND_LES :
		set_l(arg);
		return;
	case ND_LEQ :
		set_le(arg);
		return;
	default:
		return;
	}
}

void gen_compare(Node_t* node) {
	long size_l = node -> left -> tp -> Type_label == TP_ARRAY? SIZEOF_POINTER: node -> left -> tp -> size;
	long size_r = node -> right -> tp -> Type_label == TP_ARRAY? SIZEOF_POINTER: node -> right -> tp -> size;
	long size = size_r > size_l? size_r: size_l;
	char* rax = get_registername(RN_RAX, size);
	char* rdi = get_registername(RN_RDI, size);

	pop_stack( RN_RDI, size_r);
	pop_stack( RN_RAX, size_l);

	compare_value(rax, rdi);
	set_instruction(get_registername(RN_RAX, 1), node -> kind);
	if(size_l > 1) {
		move_data_zero_extension(get_registername(RN_RAX, size_l), get_registername(RN_RAX, 1));
	}
	else {
		move_data_zero_extension(get_registername(RN_RAX, SIZEOF_POINTER), get_registername(RN_RAX, 1));
	}
	push_stack( RN_RAX, size_l);
	return;
}

void gen_arithmetic_instruction(Node_t *node) {
	long size_l = node -> left -> tp -> Type_label == TP_ARRAY? SIZEOF_POINTER: node -> left -> tp -> size;
	long size_r = node -> right -> tp -> Type_label == TP_ARRAY? SIZEOF_POINTER: node -> right -> tp -> size;
	long size = size_r > size_l? size_r: size_l;
	char* rax = get_registername(RN_RAX,size);
	char* rdi = get_registername(RN_RDI,size);

	pop_stack( RN_RDI, size_r);
	pop_stack( RN_RAX, size_l);

	if(node -> kind == ND_DIV)
	{
		sign_extension(size);
		division(rdi);
		push_stack( RN_RAX, size);
		return;
	}
	if(node -> kind == ND_MOD) {
		sign_extension(size);
		division(rdi);
		push_stack(RN_RDX, size);
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
	push_stack( RN_RAX, size_l);
}

void gen_number(int val) {
	move_data(get_registername(RN_RAX, SIZEOF_INT), i2a(val));
	push_stack( RN_RAX, SIZEOF_INT);
	return;
}

void gen_string_literal(long offset) {
	calculate_address(get_registername(RN_RAX, SIZEOF_POINTER), get_data_address_with_rip(get_label_string_literal(offset), " "));
	push_stack( RN_RAX, SIZEOF_POINTER);
	return;
}

void gen_assign(Node_t* node) {
	long int size[2];
	size[0] = gen_lval(node -> left);// generate left node as left value

	size[1] = node -> right -> tp -> size;
	char *rdi = get_registername(RN_RDI,size[0]);
	char* pref = get_pointerpref(size[0]);
	generate(node -> right, 0, 0);

	pop_stack(RN_RDI, size[1]);// right
	pop_stack( RN_RAX, SIZEOF_POINTER);// left

	move_data( get_pointer(pref, RN_RAX), rdi);//代入
	push_stack( RN_RDI, size[1]);
	return;
}

void gen_deref(Node_t *node) {
		
	generate(node -> left, 0, 0);
	
	char* rax = get_registername(RN_RAX,node -> tp -> size);
	char* rcx = get_registername(RN_RCX, node -> tp -> size); 
	char * pointer_pref = get_pointerpref(node -> tp -> size);
	
	pop_stack(RN_RAX, SIZEOF_POINTER);
	move_data(rcx,get_pointer(pointer_pref,  RN_RAX));	
	push_stack(RN_RCX, node -> tp -> size);
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
	pop_stack( RN_RAX, SIZEOF_POINTER);

	move_data(register_name, get_pointer(pointer_pref,  RN_RAX));
	push_stack(RN_RAX, node -> tp -> size);
	return;
}

void gen_globvar(Node_t* node){
	long size = node -> tp -> size;
	char* rax = get_registername(RN_RAX, size);
	char* pref = get_pointerpref(size);

	gen_lval(node);
	if(node -> tp -> Type_label == TP_ARRAY) return;

	pop_stack( RN_RAX, SIZEOF_POINTER);
	move_data( rax, get_pointer(pref,  RN_RAX));
	push_stack( RN_RAX, size);
	return;
}

void gen_return(Node_t* node) {
	generate(node, 0, 0);
	if(node != NULL)// is "return;"?
		pop_stack( RN_RAX, node -> tp -> size);
	
	leave();
	ret();
	return;
}

void gen_initialize_glob_variable(Node_t* node) {
	if(node -> tp -> Type_label == TP_INT)
	{
		if(node -> val == 0)
		{
			section_zero(node -> tp -> size);
		}
		else
		{
			section_long(node -> val);
		}
	}
	else if(node -> kind == ND_STRINGLITERAL)
	{
		section_quad(get_label_string_literal(node -> offset));
	}
}
void gen_glob_declare(Node_t* node) {
	char* name = node -> kind != ND_INITLIST? node -> name: node -> left -> name;
	Type* type = node -> kind != ND_INITLIST? node -> tp: node -> left -> tp;
	int storage_class = node -> kind != ND_INITLIST? node -> storage_class: node -> left -> storage_class;
	if(node -> kind == ND_INITLIST)
	{
		if(node -> storage_class != SC_STATIC) {
			section_global(name);
			printf("	.align 8\n"); // TODO wrap by a function
		}
		section_type(name, "@object");
		section_size(name, ui2a(type -> size));
		label(name);
		Node_t* init_branch = node -> right;
		while(init_branch -> kind == ND_BLOCK)
		{
			gen_initialize_glob_variable(init_branch -> left);
			init_branch = init_branch -> right;
		}
		return;
	}

	// .comm 
	if(node -> val == 0 || storage_class != SC_AUTO) {
		if(storage_class == SC_STATIC) {
			section_local(name);
		}
		section_comm(name, type -> size, type -> size);
		return ;
	}	

	// initialize variable 
	// register symbol
	if(storage_class == SC_STATIC) {
		section_local(name);
	} 	
	else {
		section_global(name);
		printf("	.align 8\n"); // TODO wrap by a function
	}
	section_type(name, "@object");
	section_size(name,ui2a(type -> size));
	label(name);
	if(type -> size <= 4) {
		section_long(node -> val);
	} else {
		section_quad(l2a(node -> val));
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
/**
 * @brief generate if and if else
 * we assume a parameter node has following tree structure
 * if: ND_IF ---(Left) condition part
 *            |-(Right) body part
 *
 * if else: ND_Else ---(Left) ND_IFE ---(Left) condition part
 *                   |                |-(Right) if body part
 *                   |-(Right) else body part
 * 
 * @param node 
 * @param labelLoopBegin 
 * @param labelLoopEnd 
 */
void gen_if(Node_t* node, int labelLoopBegin, int labelLoopEnd) {

	long size;		
	int end_number_if;
	int end_number_else;
	int else_number;
	char* end_label;
	int depth;
	switch(node -> kind) {
	case ND_IF:
		end_number_if = filenumber++;
		end_label = get_label_file_scope(String_add("end", i2a(end_number_if)));

		size = node -> left -> tp -> size;	
		generate(node -> left, labelLoopBegin, labelLoopEnd); // condition 
		pop_stack( RN_RAX, size);
		compare_value(get_registername(RN_RAX, size), i2a(0));
		jump_equal(end_label);

		depth = stack_depth;
		generate(node -> right, labelLoopBegin, labelLoopEnd); // if body 
		if(stack_depth%2 != depth %2) { // stack management 
			move_data(get_registername(RN_RAX, SIZEOF_POINTER), get_pointer(get_pointerpref(SIZEOF_POINTER), RN_RSP));
			push_stack(RN_RAX, SIZEOF_POINTER);
		}

		label(end_label);
		
		return;
	case ND_ELSE:
		if( node -> left && node -> left -> kind == ND_IFE)
		{
			generate(node -> left, labelLoopBegin, labelLoopEnd); // condition and if body 
			end_number_else = filenumber++;
			end_label = get_label_file_scope(String_add("end", i2a(end_number_else))); // .Lend%d, end_number_else

			depth = stack_depth;
			generate(node -> right, labelLoopBegin, labelLoopEnd); // else body 
			if(stack_depth%2 != depth %2) { // stack management 
				move_data(get_registername(RN_RAX, SIZEOF_POINTER), get_pointer(get_pointerpref(SIZEOF_POINTER), RN_RSP));
				push_stack(RN_RAX, SIZEOF_POINTER);
			}

			label(end_label);
			return;
		}
		
	case ND_IFE:
		size = node -> left -> tp -> size;
		generate(node -> left, labelLoopBegin, labelLoopEnd); // condition
		pop_stack(RN_RAX, size);
		
		else_number = filenumber++;
		char* else_label = get_label_file_scope(String_add("else", i2a(else_number)));
		compare_value(get_registername(RN_RAX, size), i2a(0));
		jump_equal(else_label);
		
		depth = stack_depth;
		generate(node -> right, labelLoopBegin, labelLoopEnd); // if body 
		if(stack_depth%2 != depth %2) { // stack management 
			move_data(get_registername(RN_RAX, SIZEOF_POINTER), get_pointer(get_pointerpref(SIZEOF_POINTER), RN_RSP));
			push_stack(RN_RAX, SIZEOF_POINTER);
		}

		// After this function called then else body part will generate.
		// Same file number is used for end_number_else;
		// so I don't increment filenumber at this time.
		end_number_else = filenumber;
		end_label = get_label_file_scope(String_add("end", i2a(end_number_else)));
		jump(end_label);

		label(else_label);
		return;
	}
	return;
}

void gen_while(Node_t* node) {

	int begin_number_while = filenumber++;
	char* begin_label = get_label_file_scope(String_add("begin", i2a(begin_number_while)));
	int end_number_while = filenumber++;
	char* end_label = get_label_file_scope(String_add("end", i2a(end_number_while)));
	
	label(begin_label);
	generate(node -> left, begin_number_while, end_number_while);
	
	int size_l = node -> left -> tp -> size;
	pop_stack( RN_RAX, size_l);
	
	compare_value(get_registername(RN_RAX, size_l), i2a(0));
	jump_equal(end_label);
	

	generate(node -> right, begin_number_while, end_number_while);
	
	jump(begin_label);
	label(end_label);
	return;
}

void gen_do_while(Node_t* node) {
	int begin_number = filenumber++;
	char* begin_label = get_label_file_scope(String_add("begin", i2a(begin_number)));
	int end_number = filenumber++;
	char* end_label = get_label_file_scope(String_add("end", i2a(end_number)));

	label(begin_label);
	generate(node -> right, begin_number, end_number);
	
	generate(node -> left, begin_number, end_number);
	int size_l = node -> left -> tp -> size;
	pop_stack( RN_RAX, size_l);
	
	compare_value(get_registername(RN_RAX, size_l), i2a(0));
	jump_equal(end_label);

	jump(begin_label);
	label(end_label);
	return;
}

void gen_for(Node_t* node) {

	if(node -> left == 0)
	{//loop will go on
		// don't accept infinite loop
		return;
	}
	else
	{
		int begin_number_for = filenumber++;
		char* begin_label = get_label_file_scope(String_add("begin", i2a(begin_number_for)));
		int end_number_for = filenumber++;
		char* end_label = get_label_file_scope(String_add("end", i2a(end_number_for)));
		int update_number_for = filenumber++;
		char* update_label = get_label_file_scope(String_add("begin", i2a(update_number_for)));

		Node_t *conditions = node -> left;
		Node_t *init_condition = conditions -> left;
		Node_t *update = conditions -> right;

		generate(init_condition -> left, update_number_for, end_number_for);

		label(begin_label);
		
		
		generate(init_condition -> right, update_number_for, end_number_for);

		int size; // memory size of a iterator
		if(init_condition -> right -> tp)
		{
			size = init_condition -> right -> tp -> size;
		}
		else
		{
			size = 0;
		}
		pop_stack( RN_RAX, size);
		
		compare_value(get_registername(RN_RAX, size), i2a(0));
		jump_equal(end_label);

		generate(node -> right, update_number_for, end_number_for);

		// TODO: add update label 
		label(update_label);
		generate(update, update_number_for, end_number_for);

		jump(begin_label);
		label(end_label);
	}
	return;
}

// store value at rcx. to compare case label with value, we copy value to rax
// TODO: allow blank block
void gen_switch(Node_t* node, int begin_number) {
	int end_number_switch = filenumber++;
	unsigned int size = node -> left -> tp -> size;
	char* prefix = get_pointerpref(size);
	char* rcx = get_registername(RN_RCX, size);
	char* rdi = get_registername(RN_RDI, size);
	char* rax = get_registername(RN_RAX, size);
	// get  a value 
	// value is in the head of the current stack
	generate(node -> left, begin_number, end_number_switch);

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
			// get a case label 
			generate(caseBranch ->left, begin_number, end_number_switch);
			pop_stack( RN_RDI, caseBranch -> left -> tp -> size);

			//get a value
			pop_stack( RN_RCX, size);
			move_data( rax,  rcx);

			//compare to case value 
			// For a next loop, 
			// I push the rcx value at the end of evaluation.
			compare_value(rax, rdi);
			jump_equal(get_label_file_scope(String_add("case", i2a(i + end_number_switch))));
			push_stack( RN_RCX, size);

			caseBranch = caseBranch -> right;
			continue;
		}
	}
	// jump to default
	jump(get_label_file_scope(String_add("end", i2a(end_number_switch))));
	
	// body code generation 
	for(int j = 0; j < depth; j++) {
		if(bodyBranch -> kind == ND_DEFAULT) {
			label(get_label_file_scope(String_add("end", i2a(end_number_switch))));

			Node_t* statement = bodyBranch -> left;
			while (statement -> kind != ND_BLOCKEND)
			{
				generate(statement -> left, begin_number, end_number_switch);
				statement = statement -> right;
			}
			break;
		} else {
			label(get_label_file_scope(String_add("case", i2a(j + end_number_switch))));

			Node_t* statement = bodyBranch -> left;
			while (statement -> kind != ND_BLOCKEND)
			{
				generate(statement -> left, begin_number, end_number_switch);
				statement = statement -> right;
			}

			bodyBranch = bodyBranch -> right;
			continue;
		}
	}
	if(bodyBranch -> kind == ND_BLOCKEND)
	{
		label(get_label_file_scope(String_add("end", i2a(end_number_switch))));
	}
	filenumber = filenumber + depth;
}

void gen_log_and_or(Node_t* node) {
	long size_l = node -> left -> tp -> size;
	long size_r = node -> right -> tp -> size;
	char* rax_l = get_registername(RN_RAX, size_l);
	char* rax_r = get_registername(RN_RAX, size_r);
	int end_number = filenumber++;
	char* end_label = get_label_file_scope(String_add("end", i2a(end_number)));

	generate(node -> left, 0, 0);
	pop_stack( RN_RAX, size_l);

	// lazy evaluation 
	compare_value(rax_l, i2a(0));
	if(node -> kind == ND_LOGAND) {
		jump_equal(end_label);
	}
	else {
		jump_not_equal(end_label);
	}
	generate(node -> right, 0, 0);
	pop_stack( RN_RAX, size_r);

	compare_value(rax_r, i2a(0));
	label(end_label);
	set_not_equal(get_registername(RN_RAX, 1));

	move_data_zero_extension(get_registername(RN_RAX, SIZEOF_INT), get_registername(RN_RAX, 1));
	push_stack( RN_RAX, SIZEOF_INT);
	return;

}

StructData* get_struct_union_data(int tag, ScopeInfo* scope, char* name) {
	Vector* data = Map_get_all(tagNameSpace, name);
	for(int i = 0; i < Vector_get_length(data); i++)
	{
		StructData* maybeThis = Vector_at(data, i);
		if(ScopeInfo_in_right(scope, maybeThis -> scope) && maybeThis -> tag == tag)
		{
			return maybeThis;
		}
	}
	return NULL;
}

void gen_list_init(Node_t* node) {
	unsigned offsetTop = node -> left -> offset;
	move_data(get_registername(RN_RAX, SIZEOF_POINTER), get_registername(RN_RBP, SIZEOF_POINTER));
	substitution(get_registername(RN_RAX, SIZEOF_POINTER), ui2a(offsetTop));
	push_stack( RN_RAX, SIZEOF_POINTER);

	Node_t* initBranch = node -> right;
	if(node -> left -> tp -> Type_label == TP_ARRAY)
	{
		unsigned size = node -> left -> tp -> pointer_to -> size;
		char* prefix = get_pointerpref(size);
		while(initBranch -> kind == ND_BLOCK)
		{
			generate(initBranch -> left, 0, 0);
			pop_stack( RN_RDI, initBranch -> left -> tp -> size);

			// assign value
			pop_stack( RN_RAX, SIZEOF_POINTER);
			move_data(get_pointer( prefix, RN_RAX), get_registername(RN_RDI,  size));
			addition(get_registername(RN_RAX, SIZEOF_POINTER), i2a(size));
			push_stack( RN_RAX, SIZEOF_POINTER);
			
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
		return;
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
			pop_stack( RN_RDI, initBranch -> left -> tp -> size);

			pop_stack( RN_RAX, SIZEOF_POINTER);
			move_data( get_pointer(prefix, RN_RAX), get_registername(RN_RDI,  initBranch -> left -> tp -> size));
		
			i++;
			char* memberName = Vector_at(memberNames, i);
			if(memberName != NULL)
			{
				member = Map_at(container, memberName);
				prefix = get_pointerpref(member -> tp -> size);
			}
			// calculate next address
			move_data(get_registername(RN_RAX, SIZEOF_POINTER), get_registername(RN_RBP, SIZEOF_POINTER));
			substitution(get_registername(RN_RAX, SIZEOF_POINTER), ui2a(offsetTop));
			addition(get_registername(RN_RAX, SIZEOF_POINTER), ui2a(member -> offset));
			push_stack( RN_RAX, SIZEOF_POINTER);

			initBranch = initBranch -> right;
	}
}

void gen_dot(Node_t* node) {
	gen_lval(node);
	pop_stack( RN_RAX, SIZEOF_POINTER);
	move_data( get_registername(RN_RAX, node -> tp -> size), get_pointer(get_pointerpref(node -> tp -> size),  RN_RAX));
	push_stack( RN_RAX, node -> tp -> size);
	return;
}

void gen_log_not(Node_t* node) {
	generate(node -> left, 0, 0);
	pop_stack( RN_RAX, node -> left -> tp -> size);

	compare_value(get_registername(RN_RAX, node -> left -> tp -> size), i2a(0));
	set_equal(get_registername(RN_RAX, 1));
	move_data_zero_extension(get_registername(RN_RAX, SIZEOF_INT), get_registername(RN_RAX, 1));
	push_stack( RN_RAX, SIZEOF_INT);
}

void gen_continue(int beginLabel) {
	jump(get_label_file_scope(String_add("begin", i2a(beginLabel))));
}

void gen_break(int endLabel) {
	jump(get_label_file_scope(String_add("end", i2a(endLabel))));
}

void gen_conditional_operator(Node_t* node) {
	long condition_size = node -> left -> tp -> size;
	int first_expr = filenumber++;
	char* begin_label1 = get_label_file_scope(String_add("begin", i2a(first_expr)));
	int second_expr = filenumber++;
	char* begin_label2 = get_label_file_scope(String_add("begin", i2a(second_expr)));
	char* end_label2 = get_label_file_scope(String_add("end", i2a(second_expr)));
	Node_t* r = node -> right;


	generate(node -> left, 0, 0);
	pop_stack( RN_RAX, condition_size);
	
	compare_value(get_registername(RN_RAX, condition_size), i2a(0));
	jump_equal(begin_label2);
	jump(begin_label1);
	
	label(begin_label1);
	generate(r -> left, 0, 0);
	jump(end_label2);

	label(begin_label2);
	generate(r -> right, 0, 0);

	label(end_label2);
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
	case ND_SWITCH: gen_switch(node, labelLoopBegin);
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
	case ND_MOD:
		generate(node -> left, 0, 0);
		generate(node -> right, 0, 0);		
		gen_arithmetic_instruction(node);
		return;
	default:
		return;
	}

}


/**
 * @brief implementation of wrappers of glibc. 
 * 
 */

// assume rdi has char* value 
void gen_printf_h() {
	char* name = "printf_h";
	int return_size = SIZEOF_INT;
	function_header(name, SC_EXTERN);
	push_stack( RN_RBP, SIZEOF_POINTER);
	move_data(get_registername(RN_RBP, SIZEOF_POINTER), get_registername(RN_RSP, SIZEOF_POINTER));

	move_data(get_registername(RN_RAX, return_size), i2a(0)); // eax will have return value 
	call("printf", SC_EXTERN);

	push_stack(RN_RAX, return_size);

	pop_stack( RN_RAX, return_size);
	move_data(get_registername(RN_RSP, SIZEOF_POINTER), get_registername(RN_RBP, SIZEOF_POINTER));
	pop_stack( RN_RBP, SIZEOF_POINTER);
	ret();

	function_footer(name);
}

// assume rdi has char* value and rsi has char* value, ...  
void gen_sprintf_h() {
	char* name = "sprintf_h";
	int return_size = SIZEOF_INT;
	function_header(name, SC_EXTERN);
	push_stack( RN_RBP, SIZEOF_POINTER);
	move_data(get_registername(RN_RBP, SIZEOF_POINTER), get_registername(RN_RSP, SIZEOF_POINTER));

	move_data(get_registername(RN_RAX, return_size), i2a(0)); // eax will have return value 
	call("sprintf", SC_EXTERN);

	push_stack(RN_RAX, return_size);


	pop_stack( RN_RAX, return_size);
	move_data(get_registername(RN_RSP, SIZEOF_POINTER), get_registername(RN_RBP, SIZEOF_POINTER));
	pop_stack( RN_RBP, SIZEOF_POINTER);
	ret();

	function_footer(name);
}

// rdi has number of elements, rsi has size 
void gen_calloc_h() {
	char* name = "calloc_h";
	int return_size = SIZEOF_POINTER;
	function_header(name, SC_EXTERN);
	push_stack( RN_RBP, SIZEOF_POINTER);
	move_data(get_registername(RN_RBP, SIZEOF_POINTER), get_registername(RN_RSP, SIZEOF_POINTER));

	move_data(get_registername(RN_RAX, return_size), i2a(0)); // eax will have return value 
	call("calloc", SC_EXTERN);

	push_stack(RN_RAX, return_size);


	pop_stack( RN_RAX, return_size);
	move_data(get_registername(RN_RSP, SIZEOF_POINTER), get_registername(RN_RBP, SIZEOF_POINTER));
	pop_stack( RN_RBP, SIZEOF_POINTER);
	ret();

	function_footer(name);
}

// assume edi has int value 
void gen_exit_h() {
	char* name = "exit_h";
	int return_size = SIZEOF_INT;
	function_header(name, SC_EXTERN);
	push_stack( RN_RBP, SIZEOF_POINTER);
	move_data(get_registername(RN_RBP, SIZEOF_POINTER), get_registername(RN_RSP, SIZEOF_POINTER));

	move_data(get_registername(RN_RAX, return_size), i2a(0)); // eax will have return value 
	call("exit", SC_EXTERN);

	push_stack(RN_RAX, return_size);

	pop_stack( RN_RAX, return_size);
	move_data(get_registername(RN_RSP, SIZEOF_POINTER), get_registername(RN_RBP, SIZEOF_POINTER));
	pop_stack( RN_RBP, SIZEOF_POINTER);
	ret();

	function_footer(name);
}