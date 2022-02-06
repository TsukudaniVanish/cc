#include "cc.h"

void assert(char* , char*, ...);
void __Map_show(Map* m);
void Node_show_all(Node_t* node, unsigned depth);
/*
 * for Map test
 */
extern int String_len(char*);
extern int String_conpair(char*, char*, unsigned int);
extern void Memory_copy(void*, void*, unsigned int);
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
	fprintf(stderr, "name : %s, age : %d\n", p -> name, p -> age);
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
		fprintf(stderr, "	expect\n		");
		Person_show(expect);
		fprintf(stderr, "	get\n		");
		Person_show(get);
		assert(test, "		not equal people\n");
		return 1;
	}
	return 0;
}

void __Map_show(Map* m) {
	Person *person = NULL;
	fprintf(stderr, "Start---\n");
	for(long i = 0; i < m -> bodySize; i++)
	{
		if(m -> body[i] == NULL)
		{
			continue;
		}
		fprintf(stderr, "	%ld:\n",i);
		for(Container* p = m -> body[i]; p; p = p -> next)
		{
			person = p -> data;
			fprintf(stderr, "		key : %s\n", p -> key);
			fprintf(stderr, "		data : ");
			Person_show(person);
		}
	}
	fprintf(stderr, "End---\n");
	fprintf(stderr, "map size : %d\n", m -> size);
	
}
int Token_equal(Token_t* token, Token_t* tester) {
	if(token -> kind != tester -> kind)
		return 0;
	if(token -> length != tester -> length)
	{
		return 0;
	}
	if(tester -> length > 0)
	{
		if(!String_conpair(token -> str, tester -> str, tester -> length))
		{	
			return 0;
		}
	}
	return 1;
}

void Token_show(Token_t* token) {
	fprintf(stderr, "kind :%d\n", token -> kind);
	if(token -> length > 0)
	{
		fprintf(stderr, "%*s", token -> length, token -> str);
	}
	return;
}
void Token_show_all(Token_t* token) {
	for(Token_t* p = token; p -> kind != TK_EOF; p = p -> next)
	{
		Token_show(token);
	}
	return;
}
void Type_show(Type* tp) {
	if(tp == NULL)
	{
		fprintf(stderr, "NULL\n");
		return;
	}
	fprintf(stderr, "kind: %d, size: %d, name: %s\n", tp -> Type_label, tp -> size, tp -> name != NULL? tp -> name: "NULL");
}

void StructData_show(StructData* data) {
	Vector* v = data -> memberNames;
	Map* m = data -> memberContainer;

	fprintf(stderr, "    member names:\n");
	for(unsigned int i = 0; i < Vector_get_length(v); i++)
	{
		char* name = Vector_at(v, i);
		fprintf(stderr, "        %d: %s\n", i, name);
	}

	fprintf(stderr, "    member data:\n");
	Node_t* node = NULL;
	for(unsigned j = 0; j < m -> bodySize; j++)
	{
		for(Container* con = m -> body[j]; con; con = con -> next)
		{
			node = con -> data;
			fprintf(stderr, "        key: %s", con -> key);
			Node_show_all(node, 0);
		}
	}

}

void Node_show_all(Node_t* node, unsigned depth) {
	if(node == NULL)
	{
		fprintf(stderr, "%*sNULL\n", depth, "    ");
		return;
	}
	fprintf(stderr, "kind: %d\n", node -> kind);
	fprintf(stderr, "%*s type: ", depth, "    ");
	Type_show(node -> tp);
	fprintf(stderr, "%*sleft:\n", depth, "    ");
	Node_show_all(node -> left, depth + 1);
	fprintf(stderr, "%*sright:\n", depth, "    ");
	Node_show_all(node -> right, depth + 1);
}
void assert(char *test_name,char *format,...)
{
	va_list arg;
	va_start(arg,format);
	fprintf(stderr,"	\x1b[31mUnit test error at %s\xb1[m\n",test_name);
	vfprintf(stderr,format,arg);
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

void test_passed(char* test) {
	printf("	Unit test : %s \x1b[32mpassed\x1b[m\n", test);
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
	
	test_passed(test);
}


void unit_test_String(){
	
	assert_int("String length: ab",2,String_len("ab"));
	assert_int("String length: empty",0, String_len("\0"));
	assert_int("String cmp: ab, ac",0, String_conpair("ab","ac",String_len("ab")));
	assert_int("String cmp: ab, ab",1, String_conpair("ab","ab",String_len("ab")));
	assert_int("long string cmp:",1,String_conpair("==","== 1llls}",2));

	char *test = "String copy test";
	char * x = calloc(3,sizeof(char));
	Memory_copy(x,"abc",3);
	assert_int(test,'a',*x);
	assert_int(test,'b',*(x+1));
	assert_int(test,'c',*(x+2));

	test_passed("String test");
	return;
}

void unit_test_Map() {
	char *test = "Map test";
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
	
	if(Map_contains(m, "a"))
	{
		__Map_show(m);
		exit(1);
	}
	if(!Map_contains(m, "aa"))
	{
		__Map_show(m);
		exit(1);
	}
	
	test_passed(test);
	return;
}




void unit_test_tokenize_struct() {
	char* test = "struct tokenize test";

	char* arg = "struct { int s; };";
	user_input = arg;
	Token_t* token = tokenize(arg);
	
	if(!Token_equal(consume(&token), new_Token_t(TK_STRUCT, NULL, 0, 6, "struct", NULL)))
	{
		Token_show_all(token);
		exit(1);
	}
	expect('{', &token);
	if(!Token_equal(consume(&token), new_Token_t(TK_TypeINT, NULL, 0, 3, "int", NULL)))
	{
		Token_show_all(token);
		exit(1);
	}
	expect_ident(&token);
	expect(';', &token);
	expect('}', &token);
	expect(';', &token);
	test_passed(test);
}

void unit_test_parse_struct() {
	char* test = "struct parsing test";
	char* arg = "struct Hi { int s; char a; unsigned len; }; int main(){ struct Hi greeting;}";
	user_input = arg;
	Token_t* token = tokenize(arg);
	Vector* v = init_parser();
	program(&token, v);
	Node_t* node = Vector_at(v, 0);

	if(node == NULL)
	{
		Token_show_all(token);
		exit(1);
	}

	if(node -> kind != ND_GLOBVALDEF)
	{
		Node_show_all(node, 0);
		exit(1);
	}
	if(node -> tp == NULL)
	{
		Node_show_all(node, 0);
		exit(1);
	}
	if(node -> tp -> Type_label != TP_STRUCT)
	{
		Node_show_all(node, 0);
		exit(1);
	}
	if(node -> tp -> size != 12)
	{
		Node_show_all(node, 0);
		exit(1);
	}
	if(!Map_contains(tagNameSpace, "Hi"))
	{
		Node_show_all(node, 0);
		fprintf(stderr, "tag names:\n");
		__Map_show(tagNameSpace);
		exit(1);
	}
	StructData* data = Map_at(tagNameSpace, "Hi");
	if(!Map_contains(data -> memberContainer, "len"))
	{
		StructData_show(data);
		exit(1);
	}

	node = Vector_at(v, 1);
	Lvar* table = Vector_at(nameTable, Vector_get_length(nameTable) - 2);
	Lvar* lval = find_lvar("greeting", String_len("greeting"), &table);
	if(lval == NULL)
	{
		fprintf(stderr, "failed to find greeting\n");
		Node_show_all(node, 0);
		exit(1);
	}
	if(lval -> tp -> Type_label != TP_STRUCT)
	{
		Node_show_all(node, 0);
		exit(1);
	}
	if(!String_conpair(lval -> tp -> name, "Hi", 2))
	{
		Node_show_all(node, 0);
		exit(1);
	}
	test_passed(test);
}

void unit_test() {
	unit_test_Vector();
	unit_test_String();
	unit_test_Map();
	unit_test_tokenize_struct();
	unit_test_parse_struct();
}
