#include "cc.h"

#define TABLESIZE 128

extern unsigned int String_len(char* s);
extern int String_compare(char* s1, char* s2, unsigned int size);

extern void* calloc(unsigned nmem, unsigned size);
extern void exit(int);
static void* NULL = (void*) 0;

unsigned long hash(char* key){

	unsigned long res = 0;
	for(char *c = key; *c; c++){
		res = res + *c;
		res = res * *c;
	}
	return res;
}

Container* new_Container(char* k, void* d, Container* n, Container* p) {
	Container* container = calloc(1,sizeof(Container));
	container -> key = k;
	container -> data = d;
	container -> next = n;
	container -> prev = p;
	return container;
}

Container* make_container() {
	return new_Container(NULL,NULL,NULL,NULL);
}

Map* new_Map(long s, long b, Container** body) {
	if(s > b) return NULL;

	Map* map = calloc(1,sizeof(Map));
	map -> size = s;
	map -> bodySize = b;
	
	if(body == NULL)
	{
		map -> body = calloc(TABLESIZE,sizeof(Container));
		for(int i = 0; i < TABLESIZE; i++)
		{
			map -> body[i] = NULL;
		}
		return map;
	}
	map -> body = body;
	return map;
}

Map* make_Map() {
	return new_Map(0,TABLESIZE,NULL);
}

void Map_add(Map* m,char* key, void* data) {
	unsigned long index = hash(key) % m -> bodySize;
	Container* c = new_Container(key, data, m -> body[index], NULL);
	if(m -> body[index] != NULL) m -> body[index] -> prev = m -> body[index];
	m -> body[index] = c;
	m -> size++;
}
/* search key in m. find data which is stored at last*/
void* Map_at(Map* m, char* key) {
	unsigned long index = hash(key) % m -> bodySize;
	unsigned int len = String_len(key);
	for(Container* p = m -> body[index]; p; p = p -> next)
	{
		if(String_len(p -> key) == len && String_compare(p -> key, key, String_len(key))) 
			return p -> data;
	}
	return NULL;
}
/* find all data which has key*/
Vector* Map_get_all(Map* m, char* key) {
	Vector* toReturn = make_vector();
	unsigned long  index = hash(key) %m -> bodySize;
	unsigned int len = String_len(key);

	for(Container* p = m -> body[index]; p; p = p -> next)
	{
		if(String_len(p -> key) == len && String_compare(p -> key, key, len))
			Vector_push(toReturn, p -> data);
	}
	return toReturn;
}

void* Map_delete(Map* m, char* key) {
	unsigned long index = hash(key) % m -> bodySize;
	unsigned len = String_len(key);
	for(Container* p = m -> body[index]; p; p = p -> next)
	{
		if(String_len(p -> key) == len && String_compare(p -> key, key, String_len(key))) 
		{
			// pop out p from chain.
			if(p -> prev != NULL)
			{
				p -> prev -> next = p -> next;
			}
			if(p -> next != NULL)
			{
				p -> next -> prev = p -> prev;
			}
			if(m -> body[index] == p)
					m -> body[index] = p -> next;
			m -> size --;
			return p -> data;
		}
	}
	return NULL;
}

int Map_contains(Map* m, char* key) {
	unsigned long index = hash(key) % m -> bodySize;
	unsigned int len = String_len(key);
	for(Container* p = m -> body[index]; p ; p = p -> next)
	{
		if(String_len(p -> key) == len && String_compare(p -> key, key, len))
		{
			return 1;
		}
	}
	return 0;
}
