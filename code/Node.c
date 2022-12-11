#include "cc.h"
// storage_class is initialized by SC_AUTO 
extern void* calloc(unsigned mnum, unsigned size);
Node_t *new_Node_t(Node_kind kind,Node_t *l,Node_t *r,int v,Type* tp,char *name)
{
	Node_t *node = calloc(1,sizeof(Node_t));
	node -> kind = kind;
	node -> left = l;
	node -> right = r;
	node -> val = v;
	node -> tp = tp;
	node -> name = name;
	node -> storage_class = SC_AUTO;
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


