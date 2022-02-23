#include "cc.h"

Token_t* preprocess(Token_t* token) {
    Token_t* toReturn = token;

    Token_t* buf = token;
    while(token -> kind != TK_EOF) {
        if(token -> kind == TK_IDENT)
        {
            char* ident = expect_ident(&token);
            if(Map_contains(macros, ident))
            {
                Token_t* macro = Map_at(macros, ident);
                Token_t* insert = Token_copy(macro);
                buf -> next = insert;
                while(insert -> next && insert -> next -> kind != TK_EOF)
                {
                    insert = insert -> next;
                }
                insert -> next = token;
                continue;
            }
            continue;
        }
        buf = token;
        token = token -> next;
    }
    return toReturn;
}