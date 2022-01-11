#include "cc.h"
//#include<stdio.h>
//#include<stdbool.h>
//#include<string.h>

extern void unit_test_Vector();

int main(int argc, char **argv){

	//unit test
	if(strlen(argv[1]) == 2 && strncmp(argv[1],"-T",2) == 0)
	{
		unit_test_Vector();
		return 0;
	}

	nameTable = make_vector();

	char *buffer;
	if(strlen(argv[1]) == 2 && strncmp(argv[1],"-f",2) == 0)
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
	declere_glIdent(new_tp(TP_VOID,NULL,0),test_print,strlen(test_print),&global);

	char *test_print_int = "test_print_int";
	declere_glIdent(new_tp(TP_VOID,NULL,0),test_print_int,strlen(test_print_int),&global);

	char *test_error = "test_error";
	declere_glIdent(new_tp(TP_VOID,NULL,0),test_error,strlen(test_error),&global);
		
	


	// fuction ごとのコード
	Node_t *code[100];
	
	//入力をトークン列に変換
	Token_t *token = tokenize(buffer);


	//token を抽象構文木に変換
	program(&token,code);

	//関数スコープ識別子テーブルを先頭にセット
	scope = nameTable -> container;
	

//アセンブリ前半を出力
	printf(".intel_syntax noprefix\n");
	if(string_iter)
		set_stringiter();
	printf("	.data\n");

	int funcflag = 1;
	//先頭の式からコード生成
	//抽象構文木を降りてコード生成
	//スタックトップには式の結果が入っている
	for(int i =0;code[i];i++){
		if( funcflag && code[i] -> kind == ND_FUNCTIONDEF )
		{
			printf("	.text\n");
			printf("	.global main\n");

			funcflag = 0;
		}
		generate(code[i]);
		if(funcflag == 0)
			scope++;

	}

	return 0;
}
