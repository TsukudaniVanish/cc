#include<stdio.h>
#include<stdlib.h>

#define TABLESIZE 128

typedef struct conteiner Container;
struct conteiner {
	char* key;
	void* data;
	Container* next;
	Container* prev;
};


typedef struct {
	unsigned long size;
	unsigned long bodySize;
	Container** body;
}Map;

int Character_conpair(char c, char d) {
	if(c == d) return 1;
	return 0;
}

long String_len(char* s) {
	long res = 0;
	while(s[res]) res++;
	return res;
}

int String_conpair(char* s1, char* s2, long size) {
	if(String_len(s1) == 0 || String_len(s2) == 0 || String_len(s1) < size || String_len(s2) < size) return 0;
	int res = 1;
	for(int i = 0; i < size; i++) {
		res = res & Character_conpair(s1[i], s2[i]);
	}
	return res;
}

unsigned long hash(char* key){

	unsigned long res = 0;
	for(char *c = key; *c; c++){
		res += *c;
		res *= *c;
	}
	return res;
}

Container* new_Container(char* k, void* d, Container* n, Container* p) {
	Container* conteiner = calloc(1,sizeof(Container));
	conteiner -> key = k;
	conteiner -> data = d;
	conteiner -> next = n;
	conteiner -> prev = p;
	return conteiner;
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
			printf("find %s!\n", key);
			if(p -> prev != NULL)
			{
				p -> prev -> next = p -> next;
			}
			if(p -> next != NULL)
			{
				p -> next -> prev = p -> prev;
			}
			if(m -> body[index] -> next == NULL)
					m -> body[index] = NULL;
			m -> size --;
			return p -> data;
		}
	}
	return NULL;
}

/*
typedef struct {
	char* name;
	int age;
} Person;

Person* recode_Person(char* n, int a) {
	Person *p = malloc(sizeof(Person));
	p -> name = n;
	p -> age = a;
	return p;
}

void Person_show(Person* p) {
	printf("name : %s, age : %d\n", p -> name, p -> age);
}

void Map_show(Map* m) {
	for(long i = 0; i < m -> bodySize; i++)
	{
		
	}
}

int main() {

	printf("ab ,len : %ld\n",String_len("ab"));
	printf("len of Empty string is %ld\n", String_len("\0"));
	printf("ab and ac is %s", String_conpair("ab","ac",String_len("ab"))? "equal\n": "not equal\n");
	printf("ab and ab is %s", String_conpair("ab","ab",String_len("ab"))? "equal\n": "not equal\n");
	
	Map *m = make_Map();
	Map_add(m,"a", recode_Person("James", 34));
	Map_add(m, "b", recode_Person("Bob",46));
	Map_add(m,"c", recode_Person("Alice", 54));
	Map_add(m, "d", recode_Person("Yu",14));
	Map_add(m,"aa", recode_Person("Kelie", 24));
	Map_add(m, "bb", recode_Person("Geoge",15));
	Map_add(m,"cc", recode_Person("Jackson", 22));
	Map_add(m, "dd", recode_Person("Maline",27));

	Map_delete(m, "a");
	Map_delete(m,"cc");

	printf("Start---\n");
	for(long i = 0; i < m -> bodySize; i++)
	{
		printf("	%ld:\n",i);
		if(m -> body[i] == NULL)
		{
			printf("		---None---\n");
			continue;
		}
		Person *person = NULL; 
		for(Container* p = m -> body[i]; p; p = p -> next)
		{
			person = p -> data;
			printf("		key : %s\n", p -> key);
			printf("		data : ");
			Person_show(person);
		}
	}
	printf("End---\n");
	printf("map size : %ld\n", m -> size);
	return 0;
}
*/

