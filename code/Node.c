#include "cc.h"

Node_t *new_Node_t(Node_kind kind,Node_t *l,Node_t *r,int v,unsigned int off,Type* tp,char *name)
{
	Node_t *node = calloc(1,sizeof(Node_t));
	node -> kind = kind;
	node -> left = l;
	node -> right = r;
	node -> val = v;
	node -> offset = off;
	node -> tp = tp;
	node -> name = name;
	return node;
}

int is_lval(Node_t* node) {
	if(node -> tp)
	switch(node -> kind)
	{
		case ND_FUNCTIONCALL: return 1;
		case ND_GLOBVALCALL: return 1;
		case ND_LVAL: return 1;
		case ND_DEREF: return 1;
		case ND_STRINGLITERAL: return 1;
		case ND_DOT: return 1;
		default:
			return 0;
	}
}


