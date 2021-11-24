#include<stdbool.h>
#include<stdlib.h>
//#include<ctype.h>
//#include<stdarg.h>
#include<stdio.h>
#include<string.h>




//operators ={"==","!=","<=",">=","<",">","+","-","*","/","=",";","(",")",NULL}




//local変数の実装
typedef struct lvar Lvar;

struct lvar{
	
	Lvar *next;
	char *name;
	int length;
	int offset;
};

//local変数
Lvar *locals;


//抽象構文木のノード型

typedef enum{
	
	ND_EQL,// <-> ==
	ND_NEQ,// <-> !=
	ND_LES,// <-> <
	ND_LEQ,// <-> <=
	ND_ADD, // <-> +
	ND_SUB, // <-> -
	ND_MUL, // <-> *
	ND_DIV, // <-> /
	ND_NUM, // <-> integer
	ND_ASSIGN, // <-> = 
	ND_LVAL, // ローカル変数
	// key word=========================
	ND_RETURN,
	ND_IF,// else なしのif
	ND_ELSE,
	ND_IFE, //if ... else
	ND_WHILE,
	ND_FOR,
	//=========================

}Node_kind;

typedef struct Node Node_t;


struct Node {
	
	Node_kind kind;
	Node_t *left;
	Node_t *right;
	int val;
	int offset;

};




typedef enum{

	TK_OPERATOR, //記号
	//key words=====================================================
	TK_RETURN,
	TK_IF,
	TK_ELSE,
	TK_WHILE,
	TK_FOR,
	//=====================================================
	TK_IDENT, //識別子
	TK_DIGIT, //整数
	TK_EOF, //終了記号

}Token_kind;

typedef struct Token Token_t;

struct Token {

	Token_kind kind;
	Token_t *next;
	int val;//if kind == TK_DIGIT　-> val = the number
	char *str;//string of token
	int length;//length of operator or length of local variable name 

};




/*
 * assert find <- main.c=====================================================
 */

//変数を名前で検索する
Lvar *find_lvar(Token_t **token,Lvar **locals);


// //エラーをはく関数
//void error(char *fmt,...){//printf と同じ引数をとる
//user_input を読み込んでエラー個所を示す関数
void error_at(char *loc,char *fmt,...);

//token.kindが'op'か判定してtoken = token.next
// char OPERATOR,Token_t TOKEN -> bool
bool find(char *operator,Token_t **token);

void expect(char *string ,Token_t **token);
char expect_ident(Token_t **);
int expect_num(Token_t **token);
bool at_eof(Token_t **token);
//=====================================================




/*
 * tokenize.c=====================================================
 */

//新しいtokenを作り　cur->nextに代入するtoken
//Token_kind KIND_OF_Token_t , Token_t *CURRENT_TOKEN,char *STRING -> Token_t 
Token_t *new_token(Token_kind kind,Token_t *cur,char *str);

/*
 * tokenize function
 * 演算子:
 * 		算術演算子:
 * 				+,-,*,/
 * 		比較演算子:
 * 				==,!=,<=,>=,<,>
 * 		単項演算子:
 * 				(,),+,-,=,;
 *
 * 	演算子は長さの順にtokenizeすること
 *
 */




//文字が英数字か_か判定する
bool is_alnum(char c);

//string が上記のoperatorに一致するか見る.
/* 
 * isoperator(q) =
 * !( strncmp(q,"!=",2) && strncmp(q,"!=",2) && 
 *  strncmp(q,"<=",2) && strncmp(q,">=",2) && 
 *  *q != '+' && *q != '-' && *q != '*' && 
 *  *q != '/' && *q != '(' && *q != ')'&& 
 *  *q != '<' && *q != '>' && *q != '=' && 
 *  *q != ';'   )   
 */
bool isoperator(char *);

//tokenize funcion char * -> Token_t
Token_t *tokenize(char *p);
//=====================================================





/*
 * parse.c=====================================================
 */

Token_t *consume_ident(Token_t **token);

//Node_t を作る関数
Node_t *new_node( Node_kind kind,Node_t *l,Node_t *r);
Node_t *new_node_num(int val);
Node_t *new_node_ident(char alpha);
Node_t *new_node_keyword(Token_kind kind,Token_t **token);

//parser 本体
void program(Token_t **,Node_t **);
Node_t *stmt(Token_t**);
Node_t *assign(Token_t **);
Node_t *equality(Token_t **);
Node_t *relational(Token_t **);
Node_t *add(Token_t **);
Node_t *mul(Token_t **);
Node_t *unitary(Token_t **);
Node_t *primary(Token_t **);
//=====================================================



/*
 * codegenerator.c=====================================================
 */

void gen_lval(Node_t *node);


//抽象構文木からアセンブリコードを生成する
void generate(Node_t *node);
//=====================================================
