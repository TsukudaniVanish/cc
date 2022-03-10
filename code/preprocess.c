#include "cc.h"
extern unsigned int String_len(char*);
extern int String_conpair(char* , char* , unsigned );

MacroData* new_MacroData(char* ident, int tag, Token_t* macroBody, Vector* param) {
    MacroData* d = calloc(1, sizeof(MacroData));
    d -> ident = ident;
    d -> tag = tag;
    d -> parameters = param;
    d -> macroBody = macroBody;
    return d;
}

void* MacroData_get_parameters(MacroData* d, unsigned int index) {
    if(d -> tag != MACRO_FUNCTION)
        return NULL;
    return Vector_at(d -> parameters, index);
}

int MacroData_contains_parameters(MacroData* d, char* name) {
    if(d -> tag != MACRO_FUNCTION)
        return -1;
    Vector* v = d -> parameters;
    unsigned len = Vector_get_length(v);
    for(unsigned i = 0; i < len; i++)
    {
        char* p = Vector_at(v, i);
        if(String_conpair(p , name, String_len(p)))
            return i;
    }
    return -1;
}

// macro expression parser
// token -> ast node -> get value

Expr* new_Expr(int kind, int val, Expr* l, Expr* r) {
    Expr* exp = calloc(1, sizeof(Expr));
    exp -> kind = kind;
    exp -> value = val;
    exp -> left = l;
    exp -> right = r;
    return exp;
}
Expr* make_Expr(int kind) {
    return new_Expr(kind, 0, NULL, NULL);
}
/**
 * 
 * expression = logicOr
 * logicOr = logicAnd ( || logicOr)?
 * logicAnd = logicEq (&& logicAnd)?
 * logicEq = logicRelation ((== | !=) logicEq)?
 * logicRelation = add ((> | >= | < | <=) logicRelation)?
 * add = mul ((+ | -) add)?
 * mul = unit ((* | /) mul)?
 * unit = (+ | - | !)? primary
 * primary = CONST | IdentInMacro | (expression)
 */
static Expr* exprMacro(Token_t**);
static Expr* logOrMacro(Token_t**);
static Expr* logAndMacro(Token_t**);
static Expr* logEqMacro(Token_t**);
static Expr* logReMacro(Token_t**);
static Expr* addMacro(Token_t**);
static Expr* mulMacro(Token_t**);
static Expr* unitMacro(Token_t**);
static Expr* primaryMacro(Token_t**);

int eval_Expr(Expr*);

Expr* parse_macro_expr(Token_t** token) {
    return exprMacro(token);
}

static Expr* exprMacro(Token_t** token) {
    return logOrMacro(token);
}

static Expr* logOrMacro(Token_t** token) {
    Expr* exp = logAndMacro(token);
    if(find(LOG_OR, token))
    {
        return new_Expr(LogOr, 0, exp, logOrMacro(token));
    }
    return exp;
}

static Expr* logAndMacro(Token_t** token) {
    Expr* exp = logEqMacro(token);
    
    if(find(LOG_AND, token))
        return new_Expr(LogAnd, 0, exp, logAndMacro(token));
    return exp;
}
static Expr* logEqMacro(Token_t** token) {
    Expr* exp = logReMacro(token);
    
    if(find(EQUAL, token))
        return new_Expr(Eq, 0, exp, logEqMacro(token));
    
    if(find(NEQ, token))
        return new_Expr(Neq, 0, exp, logEqMacro(token));
    return exp;
}

#define ReLen 4
#ifdef ReLen
    #define getLEQ(j) j == 0? LEQ: Leq
    #define getGEQ(j) j == 0? GEQ: Geq
    #define getLE(j) j == 0? LE: Le
    #define getGE(j) j == 0? GE: Ge
    #define relationals(i, j) i < 3? (i == 0? getLEQ(j): getGEQ(j)):(i == 3? getLE(j): getGE(j))
#endif
static Expr* logReMacro(Token_t** token) {
    Expr* exp = addMacro(token);
    if(*token && (*token) -> kind == TK_OPERATOR)
        for(int i = 0; i< ReLen; i++)
        {
            if(find(relationals(i, 0), token))
            {
                return new_Expr(relationals(i, 1), 0, exp, logReMacro(token));
            }
        }
    return exp;
}

static Expr* addMacro(Token_t** token) {
    Expr* exp = mulMacro(token);
    if(find('+', token))
        return new_Expr(Add, 0, exp, addMacro(token));
    if(find('-', token))
        return new_Expr(Sub, 0, exp, addMacro(token));
    return exp;
}

static Expr* mulMacro(Token_t** token) {
    Expr* exp = unitMacro(token);
    if(find('*', token))
        return new_Expr(Mul, 0, exp, mulMacro(token));
    if(find('/', token))
        return new_Expr(Div, 0, exp, mulMacro(token));
    return exp;
}
char unitOps[] = {'+', '-', '!'};
#define OpsLen 3
#define getOpsKind(i) i == 0? Plus: i == 1? Minus: LogNot
static Expr* unitMacro(Token_t** token) {
    if(*token && (*token) -> kind == TK_OPERATOR)
        for(int i = 0; i < OpsLen; i++)
        {
            if(find(unitOps[i], token))
                return new_Expr(getOpsKind(i), 0, primaryMacro(token), NULL);
        }
    return primaryMacro(token);
}

static Expr* evalIdentInMacroExpr(char* name) {
    if(Map_contains(macros, name))
    {
        MacroData* macroData = Map_at(macros, name);
        if(macroData ->tag != TAG_OBJECT)
            return new_Expr(Constant, 0, NULL, NULL);
        Token_t* token = macroData ->macroBody;
        return parse_macro_expr(&token);
    }
    return new_Expr(Constant, 0, NULL, NULL);
} 

static Expr* primaryMacro(Token_t** token) {
    if(*token && (*token) -> kind == TK_CONST)
        return new_Expr(Constant, consume(token) -> val, NULL, NULL);
    if(*token && (*token) -> kind == TK_IDENT)
        return evalIdentInMacroExpr(expect_ident(token));
    if(find('(', token))
    {
        Expr* exp = exprMacro(token);
        expect(')', token);
        return exp;
    }
    if(*token && (*token) -> kind == TK_EOF)
        return NULL;
    error_at((*token) -> str, "invailed syntax");
}

int eval_Expr(Expr* exp) {
    Expr* l = exp -> left;
    Expr* r = exp -> right;
    switch(exp -> kind)
    {
        case Constant:
            return exp -> value;
        case Plus:
            return + eval_Expr(l);
        case Minus:
            return - eval_Expr(l);
        case LogNot:
            return !eval_Expr(l);
        case Mul:
            return eval_Expr(l) * eval_Expr(r); 
        case Div:
            return eval_Expr(l) / eval_Expr(r);
        case Add:
            return eval_Expr(l) + eval_Expr(r);
        case Sub:
            return eval_Expr(l) - eval_Expr(r);
        case Eq:
            return eval_Expr(l) == eval_Expr(r);
        case Neq:
            return eval_Expr(l) != eval_Expr(r);
        case Le:
            return eval_Expr(l) < eval_Expr(r);
        case Leq:
            return eval_Expr(l) <= eval_Expr(r);
        case Ge:
            return eval_Expr(l) > eval_Expr(r);
        case Geq:
            return eval_Expr(l) >= eval_Expr(r);
        case LogOr:
            return eval_Expr(l) || eval_Expr(r);
        case LogAnd:
            return eval_Expr(l) && eval_Expr(r);
    }
}

Vector* read_parameters(Token_t** token) {
    Vector* result = make_vector();
    while (!find(')', token))
    {
        Token_t* arg = Token_copy(*token);
        Vector_push(result, arg);
        if(!find(',', token))
            *token = (*token) -> next;
    }
    return result;
}

int macro_expansion(Token_t* token) {
    int macroWasExpanded = 0;

    Token_t* buf = token;// buf - token or buf == token
    while(token -> kind != TK_EOF) {
        if(token -> kind == TK_IDENT)
        {
            char* ident = expect_ident(&token);
            if(Map_contains(macros, ident))
            {
                MacroData* macroData = Map_at(macros, ident);
                Token_t* macro = macroData -> macroBody;
                Token_t* insert = Token_copy_all(macro);
                Vector* parameters = NULL;
                if(macroData -> tag == MACRO_FUNCTION && find('(', &token))
                {// function type macro
                    parameters = read_parameters(&token);
                }

                buf -> next = insert;
                Token_t* buf_in_macro = buf;// buf_in_macro - insert or buf_in_macro == insert
                while(insert -> next && insert -> next -> kind != TK_EOF)
                {
                    if(parameters && insert -> kind == TK_IDENT)
                    {
                        char* name = expect_ident(&insert);
                        int i = MacroData_contains_parameters(macroData, name);
                        if(i > -1)
                        {
                            Token_t* arg = Vector_at(parameters, i);
                            Token_t* replace_ident = Token_copy(arg);
                            buf_in_macro -> next = replace_ident;
                            replace_ident -> next = insert;
                        }
                        buf_in_macro = buf_in_macro -> next;
                        continue;
                    }
                    buf_in_macro = insert;
                    insert = insert -> next;
                }
                if(parameters && insert -> kind == TK_IDENT)
                {
                    char* name = expect_ident(&insert);
                    int i = MacroData_contains_parameters(macroData, name);
                    if(i > -1)
                    {
                        Token_t* arg = Vector_at(parameters, i);
                        Token_t* replace_ident = Token_copy(arg);
                        insert -> next = replace_ident;
                        replace_ident -> next = token;

                        buf_in_macro = insert;
                        insert = replace_ident;
                    }
                    else
                    {
                        buf_in_macro = buf_in_macro -> next;
                        buf_in_macro -> next = token;

                        insert = token;
                    }
                }
                else
                {
                    insert -> next = token;
                }

                buf = insert;
                macroWasExpanded = 1;
                continue;
            }
            else
            {
                buf = buf -> next;
            }
            continue;
        }
        buf = token;
        token = token -> next;
    }
    return macroWasExpanded;
}

Token_t* preprocess(Token_t* token) {
    Token_t* toReturn = token;

    for(;;)
    {
        if(macro_expansion(token))
            continue;
        break;
    }
    return toReturn;
}