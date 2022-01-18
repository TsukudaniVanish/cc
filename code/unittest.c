#include "cc.h"

void assert(char *test_name,char *format,...)
{
	va_list arg;
	va_start(arg,format);
	fprintf(stderr,"	\x1b[31mUnit test error at %s\xb1[m\n",test_name);
	fprintf(stderr,format,arg);
	va_end(arg);
	return;
}

int assert_int(char *test_name,long int expect, long int input)
{
	if(expect != input)
	{	
		assert(test_name,"		%ld was expected but got %ld\n",expect,input);
		exit(1);
	}
	return 0;
}

void unit_test_Vector()
{
	char *test = "Vector test";
	// make_vector
	Vector* v = make_vector();
	assert_int(test,0,Vector_get_length(v));

	int a[5] = {1,1,1,3,3};
	for(int i = 0; i< 5;i++)
	{
		Vector_push(v,a + i);
	}

	int *x;
	x = Vector_at(v,3);
	assert_int(test,3,*x);
	x = Vector_at(v,0);
	assert_int(test,1,*x);
	x = Vector_get_tail(v);
	assert_int(test,3,*x);

	Vector_pop(v);
	assert_int(test,4,Vector_get_length(v));

	printf("	Unit test: %s \x1b[32mpassed!!\x1b[m\n",test);
}
/*
 * for Map test
 */
extern int String_len(char*);
extern int String_conpair(char*, char*, long);
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

int Person_conpair(Person* p, Person* q) {
	if(p == NULL || q == NULL)	return 0;
	if(String_len(p -> name) != String_len(q -> name)) return 0;
	if(!String_conpair(p -> name, q -> name,String_len(p -> name))) return 0;
	if(p -> age != q -> age) return 0;
	return 1;
}

int assert_Person(char* test, Person* expect, Person* get) {
	if(expect == NULL)
	{
		if(get != NULL) 
		{
			assert(test, "		expected NULL but not NULL pointer got\n");
			Person_show(get);
			return 1;
		}
		return 0;
	}
	if(!Person_conpair(expect,get)) 
	{
		printf("	expect\n		");
		Person_show(expect);
		printf("	get\n		");
		Person_show(get);
		assert(test, "		not equal people\n");
		return 1;
	}
	return 0;
}

void __Map_show(Map* m) {
	Person *person = NULL;
	printf("Start---\n");
	for(long i = 0; i < m -> bodySize; i++)
	{
		if(m -> body[i] == NULL)
		{
			continue;
		}
		printf("	%ld:\n",i);
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
	
}

void unit_test_Map() {
	char *test = "Map test";
	assert_int("String length: ab",2,String_len("ab"));
	assert_int("String length: empty",0, String_len("\0"));
	assert_int("String cmp: ab, ac",0, String_conpair("ab","ac",String_len("ab")));
	assert_int("String cmp: ab, ab",1, String_conpair("ab","ab",String_len("ab")));
	
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

	Person *person = NULL;
	person = Map_at(m, "aa");
	if(assert_Person(test, recode_Person("Kelie",24),person))
	{
		__Map_show(m);
		exit(1);
	}
	person = Map_at(m,"cc");
	if(assert_Person(test,NULL,person))
	{
		__Map_show(m);
		exit(1);
	}

	printf("	Unit test: %s \x1b[32mpassed!!\x1b[m\n",test);
	
	
	return;
}

