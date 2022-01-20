#include "cc.h"

#define TABLESIZE 128

extern unsigned int String_len(char* s);
extern int String_conpair(char* s1, char* s2, unsigned int size);

unsigned long hash(char* key){

	unsigned long res = 0;
	for(char *c = key; *c; c++){
		res += *c;
		res *= *c;
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

Map* new_Map(long s, long b, Container* body[]) {
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
	if(m -> body[index] != NULL) m -> body[index] -> prev = c;
	m -> body[index] = c;
	m -> size++;
}

void* Map_at(Map* m, char* key) {
	unsigned long index = hash(key) % m -> bodySize;
	for(Container* p = m -> body[index]; p; p = p -> next)
	{
		if(String_conpair(p -> key, key, String_len(key))) return p -> data;

	}
	return NULL;
}

void* Map_delete(Map* m, char* key) {
	unsigned long index = hash(key) % m -> bodySize;
	for(Container* p = m -> body[index]; p; p = p -> next)
	{
		if(String_conpair(p -> key, key, String_len(key))) 
		{
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
