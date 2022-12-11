
#define Min(a, b) a < b? a: b
#define Max(a, b) a < b? b: a

extern void int_to_string(char*, int);
extern void long_to_string(char*, long);

/**
 * @brief vector that contains void*
 * 
 */

typedef struct vector Vector;

struct vector {
	void **container;
	unsigned int length;
	unsigned int allocsize;
};

// Vector.c====================================================
/**
 * @brief make new Vector*\n
 * @param unsigned allocate size
 */
extern Vector* new_Vector(unsigned);

/**
 * @brief make empty vector
 * 
 * @return Vector* 
 */
extern Vector* make_vector();

/*
 * @brief return if received length can be accepted or not 
 * @param Vector* vector
 * @param unsigned _rsvlen
 * @return int
 */
extern int _is_acceptable(Vector *,unsigned);

/**
 * @brief  if vector needs to reallocate memory then do. This function will use in Vector_push.
 * @param Vector** vector
 */
extern void _maybe_realloc(Vector* vector);

/**
 * @brief get vec -> length if vec is available.
 * 
 * @param vec 
 * @return int 
 */
extern int Vector_get_length(Vector* vec);

/**
 * @brief append element to tail of the vector
 * 
 * @param vec 
 * @param x 
 */
extern void Vector_push(Vector *vec, void* x);

/**
 * @brief reduce length
 * 
 */
extern void* Vector_pop(Vector* vec);

/**
 * @brief pop the initial element from vec 
 * 
 */
extern void* Vector_pop_init(Vector* vec);

/**
 * @brief replace element of vec at index by assigned pointer.
 * 
 * @param Vector* vec
 * @param unsigned index
 * @param void* pointer which will be assigned to vector
 * 
 */
extern void Vector_replace(Vector*,unsigned,void*);

/**
 * @brief access element of vec at index if index < vec -> length .
 * 
 * @param vec 
 * @param index 
 * @return void* 
 */
extern void* Vector_at(Vector* vec, unsigned index);

/**
 * @brief get element of vec at tail.
 * 
 * @param vec 
 * @return void* 
 */
extern void* Vector_get_tail(Vector *vec);

// ====================================================
// Map.c====================================================
typedef struct container Container;
struct container {
	char* key;
	void* data;
	Container* next;
	Container* prev;
};


typedef struct {
	unsigned int size;
	unsigned int bodySize;
	Container** body;
}Map;

/*
 * @brief make empty Map
 */
Map* make_Map();
extern void Map_add(Map*, char*, void*);
extern void* Map_at(Map*, char*);
extern Vector* Map_get_all(Map*, char*);
extern void* Map_delete(Map*, char*);
extern int Map_contains(Map*, char*);
// ====================================================

/**
 * @brief  represent storage class
 */
typedef enum {
	SC_AUTO,
	SC_EXTERN,
	SC_STATIC,
}StorageClass;

/**
 * @brief represent type of identifier.
 * 
 * @param Type_label label of identifier which stands for type.
 * @param Type_* pointer_to 
 * @param unsigned size : actual memory size.
 * 
 */
#define VOID_TYPE_VALUE 0
#define POINTER_TYPE_VALUE 10
#define INTEGER_TYPE_START 1
#define INTEGER_TYPE_END 3
typedef struct type Type;

struct type{
	enum{
		TP_VOID = VOID_TYPE_VALUE,
		TP_CHAR = INTEGER_TYPE_START, //char
		TP_INT,//int
		TP_LONG= INTEGER_TYPE_END, // long 
		TP_POINTER = POINTER_TYPE_VALUE,// pointer type 8byte
		TP_ARRAY,// array type
		TP_STRUCT,
		TP_UNION,
		TP_ENUM,
	}Type_label;
	/**
	 * @brief size of type 
	 */
	unsigned int size;
	Type *pointer_to;// this member has meaning when Type_label == TP_POINTER or TP_ARRAY.
	char* name;// tag name

};

typedef enum {
	SIZEOF_VOID = 0,
	SIZEOF_CHAR = 1,
	SIZEOF_INT = 4,
	SIZEOF_LONG = 8,
	SIZEOF_POINTER = 8,
}SIZEOF_BASE_TYPE;

typedef struct scope_info ScopeInfo;
/* scope infomation*/
struct scope_info {
	unsigned number;// if nested > 0, this member recodes block number
	ScopeInfo* parent; // parent of this scope.
};

/**
 * @brief Container which store objects that need to calculate an offset to map stack or data section.
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
	Type *tp;
	int length;
	// this member is initialized as SC_AUTO when new_lvar is called 
	StorageClass storage_class;
	// following members are not inputted by utility function
	/**
	 * @brief local variable : offset from rbp , string : label number of string literal
	 * 
	 */
	unsigned int offset;
	ScopeInfo* scope;
};
extern Lvar *string_literal;
extern Lvar *global;

/* @brief  this use for ordinary name space
*/
typedef struct {
	ScopeInfo* scope;
	Type* tp;// this member is used when tag is function or typedef
	int val;// this member is used when tag is enum constant
	enum {
		TAG_FUNCTION,
		TAG_OBJECT,
		TAG_TYPEDEF,
		TAG_ENUMCONSTANT,
	}tag;
	StorageClass storage;
}NameData;

extern Map *ordinaryNameSpace;
extern Map *tagNameSpace;// tag name space which contains a name of which struct , union and enum.
extern Vector *nameTable;//table of identifier which has block scope.
extern void** rootBlock;// this variable points an current root block.




// =========================Token =========================
typedef enum keyWords {
	KEYWORD_START = 0,// start
	CONTINUE,
	DEFINED,
	TYPEDEF,
	RETURN,
	SIZEOF,
	WHILE,
	BREAK,
	SWITCH,
	DEFAULT,
	CASE,
	ELSE,
	FOR,
	DO,
	IF,
	STATIC,
	EXTERN,
	MACRO_UNDEF,
	MACRO_DEFINE,
	MACRO_IF,
	MACRO_IFDEF,
	MACRO_IFNDEF,
	MACRO_ENDIF,
	MACRO_ELSE,
	MACRO_ELIF,
	MACRO_INCLUDE,
	TYPE_START,// following entries are representing type 
	VOID,
	CHAR,
	INT,
	LONG_INT,
	LONG,
	UNSIGNED_INT,
	UNSIGNED_LONG,
	UNSIGNED,
	STRUCT,
	UNION,
	ENUM,
	KEYWORD_END,// end marker
}keyword;

typedef enum symbols { 
	MULTOPERATOR_START=1000,
	EQUAL, // ==
	NEQ,// !=
	LEQ,// <=
	GEQ,// >=
	INC,// ++
	DEC,// --
	LOG_AND,// &&
	LOG_OR,// ||
	ARROW,// ->
	DOUBLE_SHARP, // ## 
	THREE_DOTS, // ...
	END_OF_MULTI_OPERATOR,
	UNIT_SYMBOL_START = 0,
	LE, // = '<',
	GE,// = '>',
	PLUS,// = '+',
	MINUS,// = '-',
	STER ,//= '*',
	DOT, //= '.'
	BACK_SLASH ,//= '/',
	AND ,//= '&',
	ASSIGN,//= '=',
	EXCLAMATION, //= '!'
	QUESTION, //= '?'
	SHARP, // = '#'
	PERCENTILE, // = '%'
	END_OF_UNIT_OPERATOR,
	PUNCTUATOR_START,
	COMMA,// = ','
	SEMICOLON,// == ';'
	COLON, // == ':'
	BRACE ,//= '{',
	BRACE_CLOSE ,//= '}',
	PARENTHESIS ,//= '(',
	PARENTHESIS_CLOSE ,//= ')',
	BRACKET ,//= '[',
	BRACKET_CLOSE ,//= ']'
	END_OF_SYMBOLS,

}Symbols;

#define TOKEN_FLOW_OPERATION_START 100
#define TOKEN_SIZEOF 200
#define TOKEN_TYPE 300
typedef enum tokenKind{
	// token ====================================================
	TK_IDENT=0, //identifier
	TK_CONST=1, //integer
	TK_OPERATOR=2, // operator
	TK_PUNCTUATOR=3,// punctuator
	TK_STRINGLITERAL,//string
	TK_PLACE_HOLDER, // <-> ...
	// token flow operation keyword =====================================================
	TK_IF= TOKEN_FLOW_OPERATION_START,//flow operation
	TK_ELSE,
	TK_WHILE,
	TK_DO,
	TK_SWITCH,
	TK_CASE,
	TK_DEFAULT,
	TK_FOR,
	TK_RETURN,
	TK_BREAK,
	TK_CONTINUE,
	TK_FLOW_OPERATION_END,// flow operation end 
	// ====================================================================
	TK_SIZEOF = TOKEN_SIZEOF,// this keyword acts like operator.
	TK_DECLARATION_SPECIFIER_START,
	TK_TYPEDEF,
	TK_STATIC,
	TK_EXTERN,
	//type of variable keyword =====================================================
	TK_TYPESTART,
	TK_TypeVOID = TOKEN_TYPE,//this list is sorted as in Type_label
	TK_TypeCHAR,
	TK_TypeINT,
	TK_TypeLONG,
	TK_STRUCT,
	TK_UNION,
	TK_ENUM,
	TK_TYPEEND,
	TK_DECLARATION_SPECIFIER_END,
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


};
extern Token_t* new_Token_t(Token_kind, Token_t*, int val, int length, char* str, Type* tp);
/**
 * @brief make new token
 * @param Token_kind kind
 * @param Token_t_* cur
 * @param char_* str
 * @return Token_t_*
 * */
extern Token_t *new_token(Token_kind kind,Token_t *cur,char *str);

/**
 * @brief 
 * @param Token_kind
 * @param keyword
 * @param Token_t* cur previous token
 * @return Token_t* 
 */
extern Token_t *new_keyword(Token_kind, keyword,Token_t *,char *);


/**
 * @brief Compare if *token -> str and given string are equal.
 * @param int kind
 * @param Token_t_** token 
 * @return bool
 */
extern int find(int ,Token_t **);



/**
 * @brief return given token and pass token to next
 * @param token 
 * @return Token_t* 
 */
extern Token_t *consume(Token_t **token);

/**
 * @brief check function call. this function does not consume token.
 * 
 * @param Token_t**
 * @return int
 * 
 */
extern int is_functioncall(Token_t **);

/**
 * @brief check cast. this function does not consume token.
 * 
 * @return int 
 */
extern int is_cast(Token_t**);

/**
 * @brief get ident string from token if kind == TK_IDENT
 * 
 */
extern char* get_ident_name(Token_t** token);

/**
 * @brief check ident is type alias
 * 
 */
extern int is_type_alias(Token_t**);
/** 
 * @brief copy kind val length str tp not copy next
 */ 
extern Token_t* Token_copy(Token_t* token);
/**
 * @brief copy all members until until.
 * 	if until == NULL, this function copy tokens to TK_EOF.
 * 
 * @param token 
 * @return Token_t* 
 */
extern Token_t* Token_copy_all(Token_t* token, Token_t* until);

// insert first param between second param and third param 
extern void Token_splice(Token_t*, Token_t*, Token_t*);

extern Token_t* Token_tailHead(Token_t* newToken, Token_t* old);

extern Token_t* Token_consume_to_last(Token_t* token);

// this function does not consume any token.
extern int check_ident_appear_until_punctuator(Token_t** token);

/**
 * @brief check whether (*token) is compatible with kind or not
 * @sa error_at
 * @param int kind 
 * @param Token_t token
 * @return void
 * @sa error_at
 */
extern void expect(int kind,Token_t **token);
/**
 * @brief check identifier
 * @sa error_at
 * @param Token_t token
 * @return char 
 */extern 
char *expect_ident(Token_t **);
/**
 * @brief check number literal
 * @param Token_t_** token 
 * @return int 
 */
extern int expect_num(Token_t **token);
//====================================================



//========================= Node =========================

//Mostly used abstract syntax tree
typedef enum nodeKind{
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
	ND_MOD, // <-> %
	ND_ASSIGN, // <-> = 
	ND_LOGNOT, // <-> !
	ND_ADDR, //<-> & reference
	ND_DEREF,// <-> * dereference
	ND_DOT, // <-> . dot operator :: -> is considered as  *(). 
	ND_CONDITIONAL, // <-> ?
	ND_CONDITIONAL_EXPRS, // <-> ... : ... (right after ?)
	ND_NUM, // <-> integer
	ND_STRINGLITERAL,
	//=========================
	ND_GLOBVALDEF,//  a definition of global variable
	ND_GLOBVALCALL,// a left value which represents global variable
	ND_LVAL, // a local variable
	ND_INITLIST, // <-> variable declare = { ... }; 
	ND_FUNCTIONCALL,//A function call
	ND_FUNCTIONDEF,//Definition of a function call
	ND_ARGMENT,// an argument of a function
	ND_LOGAND,// logic and
	ND_LOGOR,// logic or
	// flow operation=========================
	ND_RETURN,
	ND_IF,// if statement which has no else block.
	ND_ELSE,
	ND_IFE, //if ... else
	ND_DO,
	ND_WHILE,
	ND_SWITCH,
	ND_CASE,
	ND_DEFAULT,
	ND_FOR,
	ND_FORINITCONDITION,
	ND_FORUPDATE,
	ND_BREAK,
	ND_CONTINUE,
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
 * @param long_int offset
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
	 * @brief 
	 * 	ND_FUNCTION... -> number of arguments
	 * 
	 * 	ND_NUM -> value
	 */
	int val;
	unsigned int offset;// offset from rbp
	Type *tp;
	char *name;
	// this member is initialized as SC_AUTO when new_Note_t called
	StorageClass storage_class;
	ScopeInfo* scope;// this member can't initiate when new_Node_t, and other new functions
};
/**
 * @brief make new node
 * 
 * @return Node_t* 
 */
extern Node_t *new_Node_t(Node_kind,Node_t *l,Node_t *r,int v,Type* tp,char *name);
// Identify node is lvar or not
extern int is_lval(Node_t* node);

//file.c====================================================
/**
 * @brief return char* of a received file
 * @param char* path
 * @return char*
 */
extern char *file_open(char *);
//====================================================

// error_point.c====================================================

//error assert
extern char* user_input;

//error assert
extern char* filepath;

//error assert
extern char *parsing_here;

extern void error(char* fmt, ...);

/**
 * @brief this function points out the code that has some error in syntax.
 * @param char_* location
 * @param char_* format
 * @param ... 
 */
extern void error_at(char *,char *,...);
//====================================================

//file.c====================================================
/**
 * @brief return a pointer to string literal which is in given file 
 * @param char* path
 * @return char*
 */
extern char *file_open(char *);
//====================================================
//main.c=====================================================

//=====================================================

/*
 * tokenize.c=====================================================
 */

extern int is_space(char );
/**
 * @brief skip withe space characters
 * @param char* p 
 * @return char* 
 */
extern char *skip(char * p);

/**
 * @brief return size of TK_Type... 
 * 
 * @param int kind
 * @return int
 * 
 */
extern int sizeof_token(int);

extern int is_alnum(char c);

extern char* get_symbol(int);
/**
 * 
 * @brief judge if it is symbol or not 
 * @param char*
 * @return int : len of operator
 * */
extern int is_symbol(char *);

extern int is_comment(char *p);

/**
 * @brief skip comment section and set p to next character
 * 
 * @param char** p 
 */
extern void comment_skip(char **p);

extern char* get_keyword(keyword);
extern int get_correspond_token_kind(keyword);

/**
 * @fn 
 * tokenize function
 * @sa new_token
 * 
 * @return Token_t*
 * */
extern Token_t *lexical_analyze(char *p);

// store identifier: macro tokens
extern Map *macros;
extern Token_t* tokenize_macro(char** p, Token_t* cur);
//=====================================================

// preprocess.c
typedef struct {
	char* ident;
	Vector* parameters;
	Token_t* macroBody;
	enum{
		MACRO_OBJECT,
		MACRO_FUNCTION,
	}tag;
}MacroData;
extern MacroData* new_MacroData(char* ident, int tag, Token_t* macroBody, Vector* parameters);
extern void* MacroData_get_parameters(MacroData* d, unsigned int index);
extern int MacroData_contains_param(MacroData* d, char* name);

typedef enum tokenInMacro {
    Constant = 0,
    Add,
    Sub,
    Mul,
    Div,
    LogOr,
    LogAnd,
    Eq,
    Neq,
    Le,// <
    Leq,// <=
    Ge,// >
    Geq, // >=
    Plus,
    Minus,
    LogNot,
}TokenInMacro;

typedef struct expression Expr;
struct expression{
    TokenInMacro kind;
    // if kind == Constant this member has value
    int value;
    // below two members are valid if kind > 0
    Expr* left;
    Expr* right;
};
extern Expr* parse_macro_expr(Token_t** token);
extern int eval_Expr(Expr*);

extern Token_t* preprocess(Token_t* token);
//=====================================================

/*
 * parse.c=====================================================
 */
extern NameData* new_NameData(int tag);
extern NameData* search_from_ordinary_namespace(char*, ScopeInfo*);

ScopeInfo* new_ScopeInfo(unsigned number, ScopeInfo* parent);
ScopeInfo* ScopeInfo_copy(ScopeInfo* info);
int ScopeInfo_equal(ScopeInfo*, ScopeInfo*);
int ScopeInfo_in_right(ScopeInfo*, ScopeInfo*);
int ScopeInfo_inscope(ScopeInfo*);

typedef struct {
	Vector* nestedScopeData;
	unsigned current_number;
	unsigned total_number;
}ScopeController;
ScopeController* ScopeController_init();
void ScopeController_nest_appeared(ScopeController*);
void ScopeController_nest_disappeared(ScopeController*);
/* if you want to copy scopeInfo, wrap this with ScopeInfo_copy*/
ScopeInfo* ScopeController_get_current_scope(ScopeController*);

extern ScopeController* controller;

typedef struct {
	enum {
		TAG_STRUCT,
		TAG_UNION,
		TAG_ENUM,
	}tag;
	unsigned int size;
	ScopeInfo* scope;
	Vector* memberNames;
	Map* memberContainer;
	Type* tp;
}StructData;
StructData* make_StructData();
void StructData_add(StructData* data, Node_t* member);
StructData* search_from_tag_namespece(char* name, ScopeInfo*);
/**
 * @brief initialize parser / make nameTable and ast vector
 * */
Vector* init_parser();
/**
 * @brief 新しい型を作成
 * 
 * @param int Type_label
 * @param Type* pointer_to
 * @param long_int size
 * @return Type* 
 */
Type *new_tp(int,Type*,unsigned int size);


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
Lvar *declare_ident(Type *tp, char *name,int len ,Lvar **table);

/**
 * @brief ファイルスコープの識別子宣言
 * 
 * @param Type* tp
 * @param char* name
 * @param int len
 * @param Lvar** table
 */
Lvar *declare_glIdent(Type *,char*,int,Lvar**);

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
 * @brief  make new node. typecheck left and right node. result node has type. expect l and r has type
 * if lhs or rhs is pointer then other size value is scaling
 * @param Node_kind kind 
 * @param Node_t l : left
 * @param Node_t r : right
 * @param char* parsing here
 * @return Node_t* 
 */
Node_t *new_node_arithmetic( Node_kind kind,Node_t *l,Node_t *r, char*);
Node_t* new_node_function_call(Token_t** token);

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
Node_t* new_node_string_literal(Token_t**);

/**
 * @brief 識別子の末端ノードを作る
 * 
 * @param Token_t** token
 * @return Node_t* 
 */
Node_t *new_node_ident(Token_t **);
/**
 * @brief make a ast node for flow operation 
 * @param Token_kind kind 
 * @param Token_t** token 
 * @return Node_t* 
 */
Node_t *new_node_flow_operation(Token_kind kind,Token_t **token);
/**
 * @brief function def or global variable declaration or type alias
 * @param Token_t**
 */
Node_t *new_node_glob_ident(Token_t **);

/**
 * @brief make block arguments
 * @param Token_t** token
 * @return Node_t*
 * 
 */
Node_t *new_node_block(Token_t **);

/**
 * @brief parse * or &
 * @param Token_t** token
 * @return Node_t*
 */
Node_t *new_node_ref_deref(Token_t **);

Node_t* new_node_set_type_alias(Token_t** token, Node_t* node);

/**
 * @brief check the end of token
 * @param Token_t_** token 
 * @return bool 
 */
int at_eof(Token_t **token);


/**
 * @brief main functions of parser
 * @param Token_t_** token
 * @param Vector* codes
 */
void program(Token_t **,Vector*);
Node_t *func(Token_t**);
int type_parameter_list(Token_t**, Node_t**);
int parameter_list(Token_t**, Node_t**);
Node_t *parameter_declare(Token_t**, int);
Node_t *stmt(Token_t**);
Node_t *declare(Token_t**);
Node_t *init(Token_t**, Node_t*);
Node_t *init_list(Token_t**, Node_t*);
Node_t* ident_specify(Token_t** , Node_t*);
Node_t* declare_specify(Token_t** , Node_t*, int);
Node_t* pointer(Token_t**, Node_t*);
Node_t* type_specify(Token_t** token, Node_t*);
Node_t* struct_union_specify(Token_t**, Node_t*);
Node_t* struct_declare(Token_t**, Node_t*);
Node_t* struct_declare_inside(Token_t**, Node_t*);
Node_t* enum_specify(Token_t**, Node_t*);
Node_t* enum_list(Token_t**, Node_t*);
Node_t* enumerator(Token_t**, Node_t*);
Node_t* expr(Token_t**);
Node_t *assign(Token_t **);
Node_t* conditional(Token_t**);
Node_t *log_or(Token_t **);
Node_t *log_and(Token_t **);
Node_t *equality(Token_t **);
Node_t *relational(Token_t **);
Node_t *add(Token_t **);
Node_t *mul(Token_t **);
Node_t *cast(Token_t **, Node_t*);
Node_t *type_name(Token_t **, Node_t*);
Node_t *postfix(Token_t **);
Node_t *unitary(Token_t **);
Node_t *primary(Token_t **);
//=====================================================
/*
 * codegenerator.c=====================================================
 */

//変数情報をコンパイルする
long gen_lval(Node_t *node);


//関数呼びたしをコンパイル
void gen_function_call(Node_t *node);

//関数の引数をセットする
void argment_set(int,long int,long int);

//関数定義をコンパイル
void gen_function_def(Node_t *node);


/**
 * @brief Get the pointer prefix
 * 
 * @param long_int size 
 * @return char* 
 */
char *get_pointerpref(long int size);

/**
 * @brief set string literal to data section.
 * 
 */
void set_stringiter();
// generate global var declaring code
void gen_glob_declare(Node_t*);
// compile arithmetic formula
void gen_formula(Node_t*);


// generate assembly code from ast.
void generate(Node_t *node, int labelLoopBegin, int labelLoopEnd);
//=====================================================

// globals 

