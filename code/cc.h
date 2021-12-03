#include<stdbool.h>
#include<stdlib.h>
//#include<ctype.h>
//#include<stdarg.h>
#include<stdio.h>
#include<string.h>




/**
 * @brief 
 * 変数型
 * @param Type_label 変数の型ラベル
 * @param Type_* pointer_to : ポインタの示す先
 * @param size_t size : メモリでのサイズ
 * 
 */
typedef struct type Type;

struct type{
	enum{

		TP_INT,//int 型 8bite
		TP_POINTER,// pointer 型 8bite
		TP_ARRAY,// 配列型

	}Type_label;

	Type *pointer_to;// 配列型or pinter 型の時に意味があるメンバ変数

	/**
	 * @b
	 * 変数のメモリサイズ
	 * 		
	 * @param int 8
	 * @param pointer 8
	 **/
	size_t size;

};




/**
 * @brief 
 * local varの実装
 * @param Lvar_* next
 * @param char_* name
 * @param int length : length of name
 * @param long_int offset
 * @param Type_* tp : Type of local variable
 */
typedef struct lvar Lvar;

struct lvar{
	
	Lvar *next;
	char *name;
	int length;
	long int offset;
	Type *tp;
};


/**
 * @brief 
 * function scope で管理する識別子テーブル
 * @param Tables_* head
 * @param Tables_* next
 * @param Lvar_* locals
 *
 * */
typedef struct NameSpace Tables;

struct NameSpace{

	Tables *head;
	Tables *next;
	Lvar *locals;
};

//関数ごとのlocal 変数
Tables *nametable;


//========================= Node =========================

//抽象構文木のノードの識別に使用する
typedef enum{
	//operator =========================
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
	ND_ADDR, //<-> * dereference
	ND_DEREF,// <-> & reference
	//型=========================
	ND_LVAL, // ローカル変数
	ND_FUNCTIONCALL,//関数呼び出し
	ND_FUNCTIONDEF,//関数定義
	ND_ARGMENT,//関数の引数
	// key word=========================
	ND_RETURN,
	ND_IF,// else なしのif
	ND_ELSE,
	ND_IFE, //if ... else
	ND_WHILE,
	ND_FOR,
	ND_FORINITCONDITION,
	ND_FORUPDATE,
	ND_BLOCK, // <-> { ... }
	ND_BLOCKEND,// { ... } end marker
	//=========================

}Node_kind;

/**
 * @brief 
 * 抽象構文木の実装に使用する
 * @param Node_kind kind
 * @param Node_t_* left
 * @param Node_t_* right
 * @param int val : kind が ND_FUNCTION..,ND_IDENT,ND_LVALの時に使用する
 * @param long_int offset
 * @param Type tp : kind がND_LVALの時に使用する
 * @param char_* name : kind がND_LVAL の時に使用する
 */
typedef struct Node Node_t;


struct Node {
	
	Node_kind kind;
	Node_t *left;
	Node_t *right;
	/**
	 * 
	 * bref of member variavle : val 
	 * ND_FUNCTION... -> 引数の個数
	 * ND_IDENT -> value
	 * ND_Lval && node -> tp -> Type_lable == TP_TOINTER -> 配列が暗黙にキャストされたなら 1 他は-1
	 * 
	 */
	int val;
	long int offset;// offset from rbp
	Type *tp;
	char *name;

};



// =========================Token =========================
/**
 * @b
 * Tokeen の種類を列挙する
 * 
 *
 * tokens:
 * 		operator:
 * 				"==","!=","<=",">=",
 * 				"<",">","+","-","*","/","&","=",
 * 		key word:
 * 				return ...
 * 				if(...)...
 * 				if(...)...else...
 * 				while(...)...
 * 				for(...)...
 * 				sizeof ...
 * 				int
 * 		punctutator:
 * 					"{","}","[","]",";",","
 * 		identifier
 * 		constant
 * 		string-literal
 */
typedef enum{
	// token ====================================================
	TK_IDENT=0, //識別子
	TK_CONST=1, //整数
	TK_OPERATOR=2, // 演算子
	TK_PUNCTUATOR=3,// 区切り文字
	//key words=====================================================
	TK_RETURN=100,
	TK_IF,
	TK_ELSE,
	TK_WHILE,
	TK_FOR,
	TK_SIZEOF,
	//type of variable =====================================================
	TK_Type=200,//変数の型名
	//=====================================================
	TK_EOF=999, //終了記号

}Token_kind;

typedef struct Token Token_t;

struct Token {

	Token_kind kind;
	Token_t *next;
	int val;//if kind == TK_DIGIT　-> val = the number
	char *str;//string of token
	int length;//length of operator or length of local variable name
	Type *tp;


};




/*
 * assert find <- main.c=====================================================
 */

/**
 * @b
 * table から 識別子を検索する
 * @param Token_t_** token 
 * @param Lvar_** locals 
 * @return Lvar* 
 */
Lvar *find_lvar(Token_t **token,Lvar **locals);


/**
 * @b
 * エラーをはく関数 printfと同じ引数をとる
 * @param char_* location
 * @param char_* format
 * @param ... 
 */
void error_at(char *,char *,...);


/**
 * @b
 * token -> strとstring が一致するか見る
 * @param char_* string
 * @param Token_t_** token 
 * @return bool
 */
bool find(char *,Token_t **);

/**
 * @fn
 * @brief 
 * token -> strとstring が一致するか確認 一致しない場合はエラーをはく
 * @sa error_at
 * @param char_* string 
 * @param Token_t token
 * @return void
 * @sa error_at
 */
void expect(char *string ,Token_t **token);
/**
 * @brief 
 * 識別子か確認する　識別子ではない場合はエラーを吐く
 * @sa error_at
 * @param Token_t token
 * @return char 
 */
char expect_ident(Token_t **);
/**
 * @brief 
 * 数字があるか確認する 数字でない場合はエラーをはく
 * @param Token_t_** token 
 * @return int 
 */
int expect_num(Token_t **token);
/**
 * @brief 
 * token が末端か判定する
 * @param Token_t_** token 
 * @return bool 
 */
bool at_eof(Token_t **token);
//=====================================================




/*
 * tokenize.c=====================================================
 */

/**
 * @b
 * 新しいtokenを作り　cur->nextに代入する 代入後 cur を次のtoken に送る
 * @param Token_kind kind
 * @param Token_t_* cur
 * @param char_* str
 * @return Token_t_*
 * */
Token_t *new_token(Token_kind kind,Token_t *cur,char *str);


/**
 * @brief 
 * 英数字かどうか判定する
 * @param char  
 * @return bool 
 */
bool is_alnum(char c);


/**
 * 
 * @b
 * operator か punctuatorのどちらかであるか判定する
 * @param char_*
 * @return int : len of operator
 * */
int is_ope_or_pun(char *);


/**
 * @b
 * key word と一致するか見る 一致したらその種類　一致しないならTK_EOFを出す
 * @param char_*
 * @param Token_t**
 * @return bool
 * @sa  new_token
 * */
Token_kind is_keyword(char *);
/**
 * @brief 
 * key word を受け取ってトークンを生成する
 * @param Token_kind
 * @param 
 * @return Token_t* 
 */
Token_t *new_keyword(Token_kind,Token_t *,char *);

/**
 * @fn 
 * tokenize function
 * 演算子は長さの順にtokenizeすること
 * @sa new_token
 * 
 * @return Token_t*
 * */
Token_t *tokenize(char *p);
//=====================================================





/*
 * parse.c=====================================================
 */

/**
 * @brief 
 * 識別子を読み込んで token を次に送る
 * @param token 
 * @return Token_t* 
 */
Token_t *consume_ident(Token_t **token);

/**
 * @brief 
 * 新しいノードを作る
 * @param Node_kind kind 
 * @param Node_t l : left
 * @param Node_t r : right
 * @return Node_t* 
 */
Node_t *new_node( Node_kind kind,Node_t *l,Node_t *r);
/**
 * @brief 
 * 定数の末端ノードを作る
 * @param int val 
 * @return Node_t* 
 */
Node_t *new_node_num(int val);
/**
 * @brief 識別子の末端ノードを作る
 * 
 * @param char alpha 
 * @return Node_t* 
 */
Node_t *new_node_ident(char alpha);
/**
 * @brief keyword の末端ノードを作る
 * 
 * @param Token_kind kind 
 * @param Token_t** token 
 * @return Node_t* 
 */
Node_t *new_node_keyword(Token_kind kind,Token_t **token);
/**
 * @brief 関数定義の構文木を作成
 * @param Token_t**
 */
Node_t *new_function(Token_t **);

/**
 * @brief パーサ本体
 * @param Token_t_** token
 * @param Node_t_** code
 */
void program(Token_t **,Node_t **);
/**
 * @brief 関数をパース
 * 
 * @return Node_t* 
 */
Node_t *func(Token_t**);
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

//変数情報をコンパイルする
void gen_lval(Node_t *node);


//抽象構文木からアセンブリコードを生成する
void generate(Node_t *node);
//=====================================================
