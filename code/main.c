#include "cc.h"

extern int unit_test();
extern unsigned int String_len(char*);
extern int String_compare(char*, char*,unsigned int);

extern int printf(char*, ...);
extern void exit(int);
static void* NULL = (void*) 0;

extern void gen_printf_h();
extern void gen_sprintf_h();
extern void gen_calloc_h();
extern void gen_exit_h();

int main(int argc, char **argv){
	
	controller = NULL;
	ordinaryNameSpace = NULL;
	macros = NULL;

	//unit test
	if(String_len(argv[1]) == 2 && String_compare(argv[1],"-T",2))
	{
		unit_test();
		return 0;
	}

	
	char *buffer;
	if(String_len(argv[1]) == 2 && String_compare(argv[1],"-f",2))
	{
		filepath = argv[2];
		buffer = file_open(argv[2]);
		user_input = buffer;
	}
	else
	{
		//グローバル変数に代入　エラー出力用
		user_input = argv[1];
		buffer = argv[1];
	}
	controller = ScopeController_init();
	ordinaryNameSpace = make_Map();

	// helper functions
	char* _printf_h = "printf_h";
	NameData* __printf_h = new_NameData(TAG_FUNCTION);
	__printf_h -> tp = new_tp(TP_VOID, NULL, 0);
	Map_add(ordinaryNameSpace, _printf_h, __printf_h);

	char* _sprintf_h = "sprintf_h";
	NameData* __sprintf_h = new_NameData(TAG_FUNCTION);
	__sprintf_h -> tp = new_tp(TP_INT, NULL, SIZEOF_POINTER);
	Map_add(ordinaryNameSpace, _sprintf_h, __sprintf_h);

	char* _calloc_h = "calloc_h";
	NameData* __calloc_h = new_NameData(TAG_FUNCTION);
	__calloc_h -> tp = new_tp(TP_POINTER, new_tp(TP_VOID, NULL, 0), SIZEOF_POINTER);
	Map_add(ordinaryNameSpace, _calloc_h, __calloc_h);

	char* _exit_h = "exit_h";
	NameData* __exit_h = new_NameData(TAG_FUNCTION);
	__exit_h -> tp  = new_tp(TP_VOID, NULL, 0);
	Map_add(ordinaryNameSpace, _exit_h, __exit_h);
	
	
	//convert an input to a token list
	macros = make_Map();
	Token_t *token = lexical_analyze(buffer);
	token = preprocess(token);
	
	Vector* codes = init_parser();
	program(&token, codes);
	rootBlock = nameTable -> container;

// print heder of assembly 
	printf(".intel_syntax noprefix\n");
	if(string_iter)
		set_stringiter();
	printf("	.data\n");
	// generate glob var declaring code
	int len = Vector_get_length(codes);
	for(int i = 0; i < len; i++)
	{
		Node_t *code = Vector_at(codes, i);
		if(code -> kind == ND_GLOBVALDEF || code -> kind == ND_INITLIST)
		{
			gen_glob_declare(code);
		}
	}

	// add pre-implemented functions 
	gen_printf_h();
	gen_sprintf_h();
	gen_calloc_h();
	gen_exit_h();

	// recursive code generation 
	for(int i = 0;i < len;i++){
		Node_t* code = Vector_at(codes, i);
		if(code -> kind == ND_FUNCTIONDEF )
		{
			printf(".text\n");

			generate(code, 0, 0);
		}
		rootBlock++; 
	}
	return 0;
}
