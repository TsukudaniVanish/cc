// new cc.h

typedef enum {
    TK_IDENT=0, // identifier
	TK_CONST=1, // number/string literal 
	TK_OPERATOR=2, // operator
	TK_PUNCTUATOR=3,// punctuator
	TK_STRINGLITERAL,//string
    TK_KEYWORD,// preserved keyword 
}token_kind;

typedef struct token token;
struct token {
    token_kind kind;
    token* prev;
    token* next;
    char* str;
}

typedef struct Type Type;
struct Type {
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
    }kind;
    Type* ptr_to;
    char* tag; // tag name for struct or union 
    long align; // alignment size 

}

typedef struct scope scope;
struct scope{
    long index;
    long depth;
    scope* parent;
}scope;

// info of object 
typedef struct object object;
struct object {
    char* name;
    scope* scope;
    int is_func;
    int is_typedef;
    int is_tag; 
    int is_literal;
    
    // literal or variable case
    Type* tp;
    int value; // number literal or 0
    char* string; // string literal or NULL 

    // function case 
    int param_num;
    object** params;
    Type* ret_tp;

    // tag object case 
    enum {
        TAG_ENUM,
        TAG_STRUCT,
        TAG_UNION,
    }tag_kind;
    Map* field_offset; // field_name: char* -> offset: int
    Vector* filed_names: // Vector<char*>  
    int enum_val; // enum value


    // typedef case 
    Type* rep_for;
}