#include "cc.h"



Token_t *new_token(Token_kind kind,Token_t *cur,char *str){


	Token_t *token = calloc(1,sizeof(Token_t));
	token ->kind = kind;
	token -> str = str;
	cur ->next = token;
	return token;
}



Token_t *new_keyword(Token_kind kind,Token_t*cur,char *p){

	cur = new_token( kind,cur,p);
	// keywordのlength を計算 TK_Typeの時はポインタ型の読み取りに使用する
	char *q = p;
	while (!(is_ope_or_pun(q) || is_space(*q))){
		q++;
	}
	
	(cur) -> length = q -p;
	
	if(kind > 299){//ポインタ型か判定


		(cur) -> tp = new_tp(kind -300,NULL,sizeof_token(kind));

		while (is_space(*q) || *q=='*'){
			
			if(is_space(*q)){
				
				q++;
				(cur) -> length++;
				continue;
			}
			(cur) -> length++;
			Type *pointer_to = new_tp(TP_POINTER,(cur) ->tp,8);
			(cur) -> tp = pointer_to;
			q++;
		}//qには識別子の名前があるはず
	
		if(kind > 299  && is_ope_or_pun(q)  ){//識別子があるか判定

			error_at(p,"識別子が必要です");
		}
	}
	return cur;
}

char *get_operator(int kind)
{
	switch(kind)
	{
	case EQUAL:
		return "==";
	case NEQ:
		return "!=";
	case LEQ:
		return "<=";
	case GEQ:
		return	">=";
	default:
		fprintf(stderr,"invalid kind of operator : %d",kind);
		exit(1);
	}
}

bool find(int kind,Token_t **token){

	if(kind > MULTOPERATOR_START){ // len >= 2 identifier
		char *multoper = get_operator(kind);
		if((*token) -> length == strlen(multoper) && !strncmp(multoper,(*token) -> str,strlen(multoper)))
		{	consume(token);
			return true;
		}
		return false;
	}

	if((*token) -> str[0] != kind){

		return false;

	}else{


		consume(token);
		return true;
	}
}




void expect(char *string ,Token_t **token){


	if( (*token) -> kind > 100 | (*token) -> length != strlen(string) | memcmp( (*token) -> str,string,(*token) -> length ) ){
		
		error_at( (*token) -> str ,"不正な文字");
	
	}else{
	
		(*token) = (*token) -> next;
		
	}
}

char *expect_ident(Token_t **token){


	if( (*token)->kind != TK_IDENT ){


		error_at( (*token)-> str,"無効な変数名" );
	
	}else{


		char *name = calloc((*token) -> length, sizeof(char));
		memcpy(name,(*token) -> str, (*token) -> length);
		consume(token);
		return name;
	}
}

int expect_num(Token_t **token){

	
	if( (*token) -> kind != TK_CONST  ){


		error_at( (*token) -> str ,"数ではありません");
	
	}else{
		
		int v = (*token) -> val;	
		(*token) = (*token) -> next;
		return v;
	}
}

bool at_eof(Token_t **token){


	if( (*token)-> kind != TK_EOF ){


		return false;
	
	}else{


		return true;
	}
}


Token_t *consume(Token_t **token){
	Token_t *tok = *token;
	*token = (*token)-> next;
	return tok;
}

int is_functioncall(Token_t **token)
{
    Token_t * buf = (*token);

    if(buf -> kind > 299)
        consume(&buf);

    expect_ident(&buf);

    if(find('[',&buf))
    {
        expect_num(&buf);
        expect("]",&buf);
    }
    if(find('(',&buf))
        return 1;
        
    return 0;
}
