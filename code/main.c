#include "cc.h"
//#include<stdio.h>
//#include<stdbool.h>
//#include<string.h>

extern int unit_test();
extern unsigned int String_len(char*);
extern int String_conpair(char*, char*,unsigned int);


int main(int argc, char **argv){
	
	controller = NULL;
	ordinaryNameSpace = NULL;
	macros = NULL;

	//unit test
	if(String_len(argv[1]) == 2 && String_conpair(argv[1],"-T",2))
	{
		unit_test();
		return 0;
	}

	
	char *buffer;
	if(String_len(argv[1]) == 2 && String_conpair(argv[1],"-f",2))
	{
		filepah = argv[2];
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
	//helperfunction input
	char *test_print = "test_print";
	NameData* _test_print = new_NameData(TAG_FUNCTION);
	_test_print -> tp = new_tp(TP_VOID, NULL, 0);
	Map_add(ordinaryNameSpace, test_print, _test_print);

	char *test_print_int = "test_print_int";
	NameData* _test_print_int = new_NameData(TAG_FUNCTION);
	_test_print_int -> tp = new_tp(TP_VOID, NULL, 0);
	Map_add(ordinaryNameSpace, test_print_int, _test_print_int);

	char *test_error = "test_error";
	NameData* _test_error = new_NameData(TAG_FUNCTION);
	_test_error -> tp = new_tp(TP_VOID, NULL, 0);
	Map_add(ordinaryNameSpace, test_error, _test_error);
	
	
	//conbert an input to a token list
	macros = make_Map();
	Token_t *token = lexical_analyze(buffer);
	token = preprocess(token);
	
	Vector* codes = init_parser();
	program(&token, codes);
	rootBlock = nameTable -> container;

//アセンブリ前半を出力
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
			gen_glob_declar(code);
		}
	}

	//先頭の式からコード生成
	//抽象構文木を降りてコード生成
	//スタックトップには式の結果が入っている
	for(int i = 0;i < len;i++){
		Node_t* code = Vector_at(codes, i);
		if(code -> kind == ND_FUNCTIONDEF )
		{
			printf("	.text\n");
			printf("	.global main\n");

			generate(code, 0, 0);
		}
		rootBlock++; 
	}
	return 0;
}
