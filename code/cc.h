#define VOID_TYPE_VALUE 0
#define POINTER_TYPE_VALUE 10
#define VEC_MIN_SIZE 8

#include<stdbool.h>
#include<stdlib.h>
//#include<ctype.h>
#include<stdarg.h>
#include<stdio.h>
//#include<string.h>

/**
 * @brief vector that contains void*
 * 
 */

typedef struct vector Vector;

struct vector{
	void **container;
	unsigned long length;
	unsigned int allocsize;
};

// Vector.c====================================================
/**
 * @brief make new Vector*\n
 * @param size_t allocate size
 */
Vector *new_Vector(size_t);

/**
 * @brief make empty vector
 * 
 * @return Vector* 
 */
Vector* make_vector();

/**
 * @brief return if received length can be accepted or not 
 * @param Vector* vector
 * @param size_t _rsvlen
 * @return int
 */
int _is_acceptable(Vector *,size_t);

/**
 * @brief  if vector needs to reallocate memory then do. This function will use in Vector_push.
 * @param Vector** vector
 */
void _maybe_realloc(Vector* vector);

/**
 * @brief get vec -> length if vec is aviable.
 * 
 * @param vec 
 * @return int 
 */
int Vector_get_length(Vector* vec);

/**
 * @brief append element to tail of the vector
 * 
 * @param vec 
 * @param x 
 */
void Vector_push(Vector *vec, void* x);

/**
 * @brief reduce length
 * 
 */
void* Vector_pop(Vector* vec);

/**
 * @brief replace element of vec at index by assigned pointer.
 * 
 * @param Vector* vec
 * @param size_t index
 * @param void* pointer which will be assigned to vector
 * 
 */
void Vector_replace(Vector*,size_t,void*);

/**
 * @brief access element of vec at index if index < vec -> length .
 * 
 * @param vec 
 * @param index 
 * @return void* 
 */
void* Vector_at(Vector* vec, size_t index);

/**
 * @brief get element of vec at tail.
 * 
 * @param vec 
 * @return void* 
 */
void* Vector_get_tail(Vector *vec);

// ====================================================
// Map.c====================================================
typedef struct conteiner Container;
struct conteiner {
	char* key;
	void* data;
	Container* next;
	Container* prev;
};


typedef struct {
	unsigned long size;
	unsigned long bodySize;
	Container** body;
}Map;

/*
 * @brief make empty Map
 */
Map* make_Map();
void Map_add(Map*, char*, void*);
void* Map_at(Map*, char*);
void* Map_delete(Map*, char*);
// ====================================================

/**
 * @brief represent type of identifier.
 * 
 * @param Type_label label of identifier which stands for type.
 * @param Type_* pointer_to 
 * @param size_t size : actall memory size.
 * 
 */
typedef struct type Type;

struct type{
	enum{
		TP_VOID = VOID_TYPE_VALUE,
		TP_INT ,//int type 8byte
		TP_CHAR,//char type
		TP_POINTER = POINTER_TYPE_VALUE,// pointer type 8byte
		TP_ARRAY,// array type

	}Type_label;

	Type *pointer_to;// this member has meaning when Type_label == TP_POINTER or TP_ARRAY.

	/**
	 * @brief size of type 
	 */
	unsigned long size;

};




/**
 * @brief Container which store data needed for dealing with identifier.
 * 
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
	/**
	 * @brief local variable : offset from rbp , string : label number of string literal
	 * 
	 */
	long int offset;
	Type *tp;
};
Lvar *string_iter;
Lvar *global;

//table of identifier which has block scope.
Vector *nameTable;
void** scope;


// =========================Token =========================
enum { 
	MULTOPERATOR_START=1000,
	EQUAL, // ==
	NEQ,// !=
	LEQ,// <=
	GEQ,// >=
	INC,// ++
	DEC,// --
};
/**
 * @brief this is list of Type of tokens
 * 
 *
 * tokens:
 * 		operator: 
 * 		number 	1001 1002 1003 1004
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
 * 		punctuator:
 * 					"{","}","[","]",";",","
 * 		identifier
 * 		constant
 * 		string-literal
 */
typedef enum{
	// token ====================================================
	TK_IDENT=0, //identifier
	TK_CONST=1, //integer
	TK_OPERATOR=2, // operator
	TK_PUNCTUATOR=3,// punctuator
	TK_STRINGLITERAL,//string
	//key words=====================================================
	TK_IF=100,//flow operation
	TK_ELSE,
	TK_WHILE,
	TK_FOR,
	TK_RETURN,
	TK_SIZEOF=200,// this keyword acts like operator.
	//type of variable =====================================================
	TK_TypeVOID=300,//this list is sorted as in Type_label
	TK_TypeINT,
	TK_TypeCHAR,
	//=====================================================
	TK_EOF=-1, //Symbol which represents end of a list of tokens

}Token_kind;

typedef struct token Token_t;

struct token {

	Token_kind kind;
	Token_t *next;
	int val;//if kind == TK_DIGIT　-> val = the number
	int length;//length of operator or length of local variable name
	char *str;//string of token
	Type *tp;


};//====================================================


//========================= Node =========================

//Mostly used abstract syntax tree
typedef enum{
	//operator =========================
	ND_EQL,// <-> ==
	ND_NEQ,// <-> !=
	ND_LES,// <-> <
	ND_LEQ,// <-> <=
	ND_INC,// <-> ++
	ND_DEC,// <-> --
	ND_ADD, // <-> +
	ND_SUB, // <-> -
	ND_MUL, // <-> *
	ND_DIV, // <-> /
	ND_ASSIGN, // <-> = 
	ND_ADDR, //<-> & reference
	ND_DEREF,// <-> * dereference
	ND_NUM, // <-> integer
	ND_STRINGLITERAL,
	//型=========================
	ND_GLOBVALDEF,//  a definition of global variable
	ND_GLOBVALCALL,// a left value which represents global variable
	ND_LVAL, // a local variable
	ND_FUNCTIONCALL,//A function call
	ND_FUNCTIONDEF,//Definition of a function call
	ND_ARGMENT,// an argument of a function
	// key word=========================
	ND_RETURN,
	ND_IF,// if statement which has no else block.
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
 * @brief represent  an ast 
 * @param Node_kind kind
 * @param Node_t_* left
 * @param Node_t_* right
 * @param int val : When kind is ND_FUNCTION..,ND_IDENT,ND_LVAL this member has a role
 * @param long_int offsett
 * @param Type*   when kind is ND_LVAL this member has a role
 * @param char_* name : if kind is  ND_LVAL  this has a role
 */
typedef struct node Node_t;


struct node {
	
	Node_kind kind;
	Node_t *left;
	Node_t *right;
	/**
	 * 
	 * @brief of member variable : val 
	 * ND_FUNCTION... -> number of arguments
	 * ND_IDENT -> value
	 * ND_Lval && node -> tp -> Type_label == TP_POINTER and pointer_to == TP_ARRAY -> size of array
	 *
	 * 
	 */
	int val;
	long int offset;// offset from rbp
	Type *tp;
	char *name;

};

//file.c====================================================
/**
 * @brief return char* of a received file
 * @param char* path
 * @return char*
 */
char *file_open(char *);
//====================================================

// error_point.c====================================================

//error assert
char* user_input;

//error assert
char* filepah;

//error assert
char *parsing_here;

/**
 * @brief this function points out the code that has some error in syntax.
 * @param char_* location
 * @param char_* format
 * @param ... 
 */
void error_at(char *,char *,...);
//====================================================

//Token.c ======================================================
/**
 * @brief make new token
 * @param Token_kind kind
 * @param Token_t_* cur
 * @param char_* str
 * @return Token_t_*
 * */
Token_t *new_token(Token_kind kind,Token_t *cur,char *str);

/**
 * @brief 
 * @param Token_kind
 * @param 
 * @return Token_t* 
 */
Token_t *new_keyword(Token_kind,Token_t *,char *);

/**
 * @brief Compair if *token -> str and given string are equal.
 * @param int kind
 * @param Token_t_** token 
 * @return bool
 */
int find(int ,Token_t **);

/**
 * @brief 
 * token -> strとstring が一致するか確認 一致しない場合はエラーをはく
 * @sa error_at
 * @param int kind 
 * @param Token_t token
 * @return void
 * @sa error_at
 */
void expect(int kind,Token_t **token);
/**
 * @brief 
 * 識別子か確認する　識別子ではない場合はエラーを吐く
 * @sa error_at
 * @param Token_t token
 * @return char 
 */
char *expect_ident(Token_t **);
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


/**
 * @brief 
 * token をさすポインタを返して token を次に送る
 * @param token 
 * @return Token_t* 
 */
Token_t *consume(Token_t **token);

/**
 * @brief token を先読みして関数宣言かどうか判定する
 * 
 * @param Token_t**
 * @return int
 * 
 */
int is_functioncall(Token_t **);

// ======================================================

//Node.c ======================================================
/**
 * @brief 新しいノードを作る
 * 
 * @return Node_t* 
 */
Node_t *new_Node_t(Node_kind,Node_t *l,Node_t *r,int v,long int off,Type* tp,char *name);
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
 * @brief 文字列リテラルをパースする
 * 
 * @param Token_t** token
 * 
 */
Node_t* new_node_stringiter(Token_t**);

/**
 * @brief 関数呼び出しをパース
 * 
 * @param Token_t** 
 * @return Node_t* 
 */
Node_t *new_node_funcCall(Token_t **token);

/**
 * @brief 関数の引数を読み込む
 * 
 * @param Tokent_t** token
 * @param Node_t** vector which holds arg types
 * @return int : a number of argment 
 */
int Node_readarg(Token_t **, Node_t**);

/**
 * @brief 関数定義をパース
 * 
 * @param Token_t **
 * @return Node_t*
 */
Node_t *new_node_funcDef(Token_t **token);

/**
 * @brief 変数を表すノードを作る
 * 
 * @param token 
 * @return Node_t* 
 */
Node_t *new_node_var(Token_t **token);

/**
 * @brief 識別子の末端ノードを作る
 * 
 * @param Token_t** token
 * @return Node_t* 
 */
Node_t *new_node_ident(Token_t **);
/**
 * @brief keyword の末端ノードを作る
 * 
 * @param Token_kind kind 
 * @param Token_t** token 
 * @return Node_t* 
 */
Node_t *new_node_keyword(Token_kind kind,Token_t **token);
/**
 * @brief 関数定義, グローバル変数の構文木を作成
 * @param Token_t**
 */
Node_t *new_node_globalident(Token_t **);

/**
 * @brief block の構文木を作成
 * @param Token_t** token
 * @return Node_t*
 * 
 */
Node_t *new_node_block(Token_t **);

/**
 * @brief '*' または '&'をパースする
 * @param Token_t** token
 * @return Node_t*
 */
Node_t *new_node_ref_deref(Token_t **);

//======================================================
//file.c====================================================
/**
 * @brief 指定されたファイルの内容を返す
 * @param char* path
 * @return char*
 */
char *file_open(char *);
//====================================================


//main.c=====================================================


//=====================================================




/*
 * tokenize.c=====================================================
 */

/**
 * @brief 空白文字を判定する
 * @param char p
 * @return int 
 */
int is_space(char );

/**
 * @brief 空白をスキップする
 * 
 * @param char* p 
 * @return char* 
 */
char *skip(char * p);

/**
 * @brief TK_Type... のメモリサイズを返す
 * 
 * @param int kind
 * @return int
 * 
 */
int sizeof_token(int);


/**
 * @brief 
 * 英数字かどうか判定する
 * @param char  
 * @return bool 
 */
int is_alnum(char c);


/**
 * 
 * @brief judge if it is simbol or not 
 * @param char*
 * @return int : len of operator
 * */
int is_simbol(char *);

/**
 * @brief コメントかどうか判定する
 * 
 * @param char* p 
 * @return int 
 */
int is_comment(char *p);

/**
 * @brief コメント部分を送って次のトークンにポインタを合わせる
 * 
 * @param char** p 
 */
void comment_skip(char **p);




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
 * @brief 新しい型を作成
 * 
 * @param int Type_label
 * @param Type* pointer_to
 * @param long_int size
 * @return Type* 
 */
Type *new_tp(int,Type*,long int size);

/**
 * @brief token を読み込んで型を判定する
 * 
 * @param char** 識別子名を代入する
 * @param Token_t** token
 * @return Type*
 */
Type *read_type(char ** name,Token_t **token);

/**
 * @brief 関数名を代入して関数の型を検索して返す
 * @param char** name
 * @param Token_t** token
 * @return Type*
 */
Type *Type_function_return(char **,Token_t**);

/**
 * @b
 * table から 識別子を検索する
 * @param char* name
 * @param int length 
 * @param Lvar** locals 
 * @return Lvar* 
 */
Lvar *find_lvar(char *,int,Lvar **locals);

/**
 * @brief 新しい変数を作成する
 * 
 * @param Type* tp 
 * @param char* name 
 * @param int length 
 * @param Lvar* next
 * @return Lvar* 
 */
Lvar *new_lvar(Type *tp,char *name, int length,Lvar *);


/**
 * @brief 型チェックをする関数 \n
 * 型がないときは0 型が一致するときは 1 両辺が違う型の時は2を返す
 * @param Node_t node
 * @return int 
 */
int typecheck(Node_t *node);
/**
 * @brief 暗黙の型変換をする
 * 
 * @param Node_t* node
 * @return Type* 
 */
Type *imptypecast(Node_t*);

/**
 * @brief 変数宣言か判定する
 * 
 * @param token 
 * @return int 
 */
int is_lvardec(Token_t **token);

/**
 * @brief 変数宣言を処理する
 * 
 * @param Type* tp 
 * @param char* name 
 * @param int len 
 * @param Lvar** table
 */
Lvar *declere_ident(Type *tp, char *name,int len ,Lvar **table);

/**
 * @brief ファイルスコープの識別子宣言
 * 
 * @param Type* tp
 * @param char* name
 * @param int len
 * @param Lvar** table
 */
Lvar *declere_glIdent(Type *,char*,int,Lvar**);

/**
 * @brief トークン列が配列要素へのアクセスかどうか判定する
 * 与えられたtoken へのポインタを先頭として
 *  "[" expression "]" 
 * の形の構文かどうかを判定する
 * @param token 
 * @return int 
 */
int is_arrmemaccess(Token_t **token);

/**
 * @brief 配列要素アクセスを構文木に変換する
 * 
 * @param Token_t** token
 * @param Node_t** prev
 * @return Node_t* 
 */
Node_t* arrmemaccess(Token_t **token , Node_t**);


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
Node_t *Lvardec(Token_t**);
Node_t *assign(Token_t **);
Node_t *equality(Token_t **);
Node_t *relational(Token_t **);
Node_t *add(Token_t **);
Node_t *mul(Token_t **);
Node_t *postfix(Token_t **);
Node_t *unitary(Token_t **);
Node_t *primary(Token_t **);
//=====================================================



/*
 * codegenerator.c=====================================================
 */

//変数情報をコンパイルする
void gen_lval(Node_t *node);


//関数呼びたしをコンパイル
void gen_function_call(Node_t *node);

//関数の引数をセットする
void argment_set(int,long int,long int);

//関数定義をコンパイル
void gen_function_def(Node_t *node);

//配列自身の先頭をさすポインタをセット
void set_array_header();

/**
 * @brief Get the register name
 * 
 * @param int 
 * @return char* 
 */
char *get_registername(char *,long int);

/**
 * @brief Get the pointer prefix
 * 
 * @param long_int size 
 * @return char* 
 */
char *get_pointerpref(long int size);

/**
 * @brief rsp の値を引いてrsp の示すアドレスにストア
 * 
 * @param long_int size
 * @param char* register name
 * @return void
 */
void push_stack(int long size,char *);

/**
 * @brief rsp の値を足してrspの示すアドレスからロード
 * 
 * @param long_int size
 * @param char* register name
 * @return void 
 */
void pop_stack(long int size,char *);
/**
 * @brief 文字列イテラルをセットする
 * 
 */
void set_stringiter();
/**
 * @brief register の値をストレージにストア
 * 
 * @param  char* name : 変数名
 * @param char* register name
 * @param long_int size
 */
void gen_global_store(char*,char*,long int);

/**
 * @brief registerの値の値をストレージにストア
 * 
 * @param  char* name : 変数名
 * @param char* register name
 * @param long_int size
 * @param long_int index
 */
void gen_global_store_arr(char*,char*,long int,long int);
//式をコンパイル
void gen_formula(Node_t*);


//抽象構文木からアセンブリコードを生成する
void generate(Node_t *node);
//=====================================================
