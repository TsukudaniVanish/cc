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