#include "cc.h"
//#include<stdio.h>
//#include<stdbool.h>
//#include<string.h>

extern void unit_test_Vector();
extern void unit_test_Map();
extern void unit_test_String();

extern unsigned int String_len(char*);
extern int String_conpair(char*, char*,unsigned int);
int main(int argc, char **argv){

	//unit test
	if(String_len(argv[1]) == 2 && String_conpair(argv[1],"-T",2))
	{
		unit_test_Vector();
		unit_test_String();
		unit_test_Map();
		return 0;
	}

	nameTable = make_vector();

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

	//helperfunction input
	char *test_print = "test_print";
	declere_glIdent(new_tp(TP_VOID,NULL,0),test_print,String_len(test_print),&global);

	char *test_print_int = "test_print_int";
	declere_glIdent(new_tp(TP_VOID,NULL,0),test_print_int,String_len(test_print_int),&global);

	char *test_error = "test_error";
	declere_glIdent(new_tp(TP_VOID,NULL,0),test_error,String_len(test_error),&global);
		
	


	// fuction ごとのコード
	Vector *codes = make_vector();
	
	//入力をトークン列に変換
	Token_t *token = tokenize(buffer);


	//token を抽象構文木に変換
	program(&token,codes);

	//関数スコープ識別子テーブルを先頭にセット
	scope = nameTable -> container;
	

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
		if(code -> kind == ND_GLOBVALDEF)
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

			generate(code);
			scope++;
		}
	}
	return 0;
}
