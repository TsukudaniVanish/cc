#include<stdbool.h>

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
	ND_VAR, // ローカル変数

}Node_kind;

typedef struct Node Node_t;

//抽象構文木のノード型

struct Node {
	
	Node_kind kind;
	Node_t *left;
	Node_t *right;
	int val;
	char name;

};

typedef enum{

	TK_OPERATOR, //記号
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
	int length;//length of token

};

// //エラーをはく関数
// void error(char *fmt,...){//printf と同じ引数をとる
//user_input を読み込んでエラー個所を示す関数
void error_at(char *loc,char *fmt,...);

//token.kindが'op'か判定してtoken = token.next
// char OPERATOR,Token_t TOKEN -> bool
bool find(char *operator,Token_t **token);

/*
 *expect function
 */

void expect(char *string ,Token_t **token);
char expect_ident(Token_t **);
int expect_num(Token_t **token);
bool at_eof(Token_t **token);

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
 * 				+,-
 *
 * 	演算子は長さの順にtokenizeすること
 *
 */
//tokenize funcion char * -> Token_t
Token_t *tokenize(char *p);

//Node_t を作る関数
Node_t *new_node( Node_kind kind,Node_t *l,Node_t *r);
Node_t *new_node_num(int val);
Node_t *new_node_ident(char alpha);

/*
 * token から構文木を生成 
 */
/*
 * 生成文法
 *
 * program = stmt*
 * stmt = assign";"
 * assign = equality ("=" assign )?
 * equality = relational("==" relational | "!=" relational)*
 * relational = add( "<=" add | "<" add | ">=" add | ">" add  )*
 * add = mul( "+"mul | "-"mul)* 
 * mul = unitary ("*" unitary | "/" unitary )*
 * unitary = ('+' | '-' )? primary
 * primary = num | indent | "(" assign ")"
 *
 * 終端記号:
 * 		num
 * 		indent
 *
 *
 */

void program(Token_t **);
Node_t *stmt(Token_t**);
Node_t *assign(Token_t **);
Node_t *equality(Token_t **);
Node_t *relational(Token_t **);
Node_t *add(Token_t **);
Node_t *mul(Token_t **);
Node_t *unitary(Token_t **);
Node_t *primary(Token_t **);

//トークン列から抽象構文木を生成
Node_t *expr(Token_t **token);

//抽象構文木からアセンブリコードを生成する
void generate(Node_t *node);
