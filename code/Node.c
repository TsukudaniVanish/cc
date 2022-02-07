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




