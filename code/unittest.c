#include "cc.h"

void assert(char* , char*, ...);
void __Map_show(Map* m);
void Node_show_all(Node_t* node, unsigned depth);
/*
 * for Map test
 */
extern int String_len(char*);
extern int String_compare(char*, char*, unsigned int);
extern void Memory_copy(void*, void*, unsigned int);
extern char* String_add(char*, char*);
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

int Person_compare(Person* p, Person* q) {
	if(p == NULL || q == NULL)	return 0;
	if(String_len(p -> name) != String_len(q -> name)) return 0;
	if(!String_compare(p -> name, q -> name,String_len(p -> name))) return 0;
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
	if(!Person_compare(expect,get)) 
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
		if(!String_compare(token -> str, tester -> str, tester -> length))
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
		fprintf(stderr, "%s", token -> str);
	}
	return;
}
void Token_show_all(Token_t* token) {
	for(Token_t* p = token; p -> kind != TK_EOF; p = p -> next)
	{
		Token_show(p);
	}
	return;
}
#define ERROR_TOKEN(a) do{Token_show_all(a); exit(1);}while(0)

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
#define NODE_ERR(a) {Node_show_all(a, 0); exit(1);}while(0)

void assert(char *test_name,char *format,...)
{
	va_list arg;
	va_start(arg,format);
	fprintf(stderr,"	\x1b[31mUnit test error at %s\x1b[m\n",test_name);
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
	assert_int("String cmp: ab, ac",0, String_compare("ab","ac",String_len("ab")));
	assert_int("String cmp: ab, ab",1, String_compare("ab","ab",String_len("ab")));
	assert_int("long string cmp:",1,String_compare("==","== 1llls}",2));

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
	Map_add(m, "b", recode_Person("BobI", 88));
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
	Vector* v = Map_get_all(m, "b");
	if(Vector_get_length(v) != 2)
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
	Token_t* token = lexical_analyze(arg);
	
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
	char* arg = "struct Hi { int s; int a; char* len; }; int main(){ struct Hi greeting;}";
	user_input = arg;
	Token_t* token = lexical_analyze(arg);
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
	if(node -> tp -> size != 16)
	{
		assert(test, "sizeof error: %d\n", node -> tp -> size);
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
	if(!String_compare(lval -> tp -> name, "Hi", 2))
	{
		Node_show_all(node, 0);
		exit(1);
	}
	test_passed(test);
}
#ifdef NODE_ERR
	#define NODE_ERR_STRUCT(a) do{ assert(test, "%d:", i); NODE_ERR(a);}while(0);
#endif
void unit_test_parse_struct_init() {
	char * test = "struct init test";
	char* arg = "struct Hi {int a; int b; char* c; }; struct Hi a = {10, 10, \"Greeting\"}; int main() { a.a = 100; struct Hi* b = &a; b -> b = 222; return 0;}";
	user_input = arg;
	Token_t* token = lexical_analyze(arg);
	Vector* v = init_parser();
	program(&token, v);
	Node_t* node = Vector_at(v, 1);
	int i = 0;
	if(node == NULL)
	{
		NODE_ERR_STRUCT(node);
	}
	i = 1;
	if(node -> kind != ND_INITLIST)
	{
		NODE_ERR_STRUCT(node);
	}
	i = 2;
	if(global == NULL)
	{
		NODE_ERR_STRUCT(node);
	}
	i = 3;
	if(String_len(global -> name) != 1 || !String_compare(global -> name, "a", 1))
	{
		NODE_ERR_STRUCT(node);
	}
	i = 4;
	if(node -> right -> kind != ND_BLOCK)
	{
		NODE_ERR_STRUCT(node);
	}
	node = Vector_at(v, 2);
	i = 5;
	if(node == NULL || node -> right ==  NULL || node -> right -> left == NULL)
	{
		NODE_ERR_STRUCT(node);
	}
	Node_t* top = node;
	node = node -> right -> left;
	i = 6;
	if(node -> kind != ND_ASSIGN)
	{
		NODE_ERR_STRUCT(node);
	}
	i = 7;
	if(node -> left == NULL || node -> left -> kind != ND_DOT)
		NODE_ERR_STRUCT(node);
	node = top -> right;
	
	test_passed(test);
}
void unit_test_tokenize_union() {
	char* test = "union tokenize test";
	char* arg = "union Hi{ char* greeting, char greeting[]};";
	Token_t* token = lexical_analyze(arg);
	
	if(token == NULL)
		ERROR_TOKEN(token);
	if(token -> kind != TK_UNION)
		ERROR_TOKEN(token);
	test_passed(test);
}

void unit_test_tokenize_enum() {
	char* test = "enum tokenize test";
	char* arg = "enum Hi { GOOD_MORNING = 1, HELLO, GOOD_NIGHT};";
	Token_t* token = lexical_analyze(arg);

	if(token == NULL)
	{
		ERROR_TOKEN(token);
	}
	if(token -> kind != TK_ENUM)
	{
		ERROR_TOKEN(token);
	}
	test_passed(test);
}

void unit_test_tokenize_include() {
	char* test = "include test";
	char* buf = "#include \"./test/test.h\"\n#include \"./test/test.h\"\nint foo(){return 1;}";

	macros = make_Map();
	Token_t* token = lexical_analyze(buf);
	Token_show_all(token);
}

#ifdef NODE_ERR
	#define NODE_ERR_PRASE_FAILED(a) do{assert(test, ""); NODE_ERR(a);}while(0)
#endif
void unit_test_parse_union() {
	char* test = "union parsing test";
	char* arg = "union Hi { char* greeting; int a;}; union Hi u;";

	Token_t* token = lexical_analyze(arg);
	
	parsing_here = arg;
	user_input = arg;
	controller = NULL;
	Vector* v = init_parser();
	program(&token, v);
	Node_t* node = Vector_at(v, 1);

	if(node == NULL)
		NODE_ERR_PRASE_FAILED(node);
	if(node -> tp == NULL || node -> tp -> Type_label != TP_UNION)
		NODE_ERR_PRASE_FAILED(node);
	if(node -> tp -> size != 8)
		NODE_ERR_PRASE_FAILED(node);
	test_passed(test);
}

void unit_test_parse_enum() {
	char* test = "enum parse test";
	char* arg = "enum Greeting { HI, HELLO, GOOD_MORNING = 10}; int main(){ enum Greeting formal = GOOD_MORNING; enum Greeting casual = HELLO; return 0;}";

	user_input = arg;
	parsing_here = arg;
	controller = NULL;

	Token_t *token = lexical_analyze(arg);
	Vector* v = init_parser();
	program(&token, v);

	Node_t* node = Vector_at(v, 0);
	if(node == NULL)
		NODE_ERR_PRASE_FAILED(node);
	if(node -> tp == NULL || node -> tp -> Type_label != TP_ENUM)
		NODE_ERR_PRASE_FAILED(node);
	if(node -> tp -> size != 4)
		NODE_ERR_PRASE_FAILED(node);

	//test_passed("enum declare");
	
	node = Vector_at(v, 1);
	if(node == NULL || node -> kind != ND_FUNCTIONDEF)
		NODE_ERR_PRASE_FAILED(node);
	if(node -> right == NULL || node -> right -> left == NULL || node -> right -> right == NULL)
		NODE_ERR_PRASE_FAILED(node);
	//test_passed("enum function declare");
	
	Node_t* node_stmt = node -> right;
	node = node -> right -> left;
	if(node -> kind != ND_ASSIGN)
		NODE_ERR_PRASE_FAILED(node);
	if(node -> left -> tp -> Type_label != TP_ENUM || node -> right -> val != 10)
		NODE_ERR_PRASE_FAILED(node);
	//test_passed("enum assign first");

	node = node_stmt -> right -> left;
	if(node == NULL || node -> kind != ND_ASSIGN)
		NODE_ERR_PRASE_FAILED(node);
	if(node -> left -> tp -> Type_label != TP_ENUM || node -> right -> val != 1)
		NODE_ERR_PRASE_FAILED(node);
	test_passed(test);
}

#ifdef NODE_ERR
	#define NODE_ERR_NOT(a) do{ assert(test, ""); NODE_ERR(a);}while(0)
#endif
void unit_test_parse_not() {
	char* test = "Not parsing test";
	char* arg = "int foo(){ !1;}";

	user_input = arg;
	parsing_here = arg;
	Token_t* token = lexical_analyze(arg);

	controller = NULL;
	ordinaryNameSpace = NULL;
	Vector* v = init_parser();
	program(&token, v);
	Node_t* node = Vector_at(v, 0);

	if(node == NULL || node -> right == NULL)
		NODE_ERR_NOT(node);
	node = node -> right -> left;
	// test_passed("NULL check");

	if(node -> kind != ND_LOGNOT)
		NODE_ERR_NOT(node);
	if(node -> left == NULL)
		NODE_ERR_NOT(node);
	test_passed(test);
}

#define TOKEN_ERR(token) do{ Token_show_all(token); exit(1);}while(0)
#ifdef TOKEN_ERR
	#define TOKEN_ERR_MACRO_DEFINE(a, token) do{assert(test, a); TOKEN_ERR(token);}while(0)
#endif 
void unit_test_tokenize_macro_define() {
	char* test = "define macro tokenize test";
	char* arg = "#define Ten 10 \n int g = Ten; int main(){return 0;}";
	macros = make_Map();
	Token_t* token = lexical_analyze(arg);

	if(token == NULL)
		TOKEN_ERR_MACRO_DEFINE("returned null token\n", token);
	if(token -> kind != TK_TypeINT)
		TOKEN_ERR_MACRO_DEFINE("expected TK_TypeINT\n", token);
	if(!Map_contains(macros, "Ten"))
		TOKEN_ERR_MACRO_DEFINE("can't find Ten in macros\n", token);
	MacroData* macroData = Map_at(macros, "Ten");
	if(macroData == NULL || macroData -> tag != MACRO_OBJECT)
		TOKEN_ERR_MACRO_DEFINE("fail to tokenize marco\n", NULL);
	Token_t* macro = macroData -> macroBody;
	if(macro == NULL || macro -> kind != TK_CONST)
		TOKEN_ERR_MACRO_DEFINE("fail to tokenize marco\n", macro);
	test_passed(test);
}

#ifdef TOKEN_ERR
	#define TOKEN_ERR_MACRO_FUNCTION(fmt, token) do{assert(test, fmt); TOKEN_DEFINE(token)}while(0)
#endif
void unit_test_tokenize_macro_function() {
	char* test = "macro function test";
	char* arg = "#define timesTen(a) a * 10\n int main(){int qaq = 9; return timesTen(qaq);}";

	macros = make_Map();
	Token_t* token = lexical_analyze(arg);

	if(token == NULL)
		TOKEN_ERR_MACRO_DEFINE("returned null token\n", token);
	if(token -> kind != TK_TypeINT)
		TOKEN_ERR_MACRO_DEFINE("expected TK_TypeINT\n", token);

	if(!Map_contains(macros, "timesTen"))
		TOKEN_ERR_MACRO_DEFINE("can't find timesTen in macros\n", token);
	MacroData* macroData = Map_at(macros, "timesTen");
	if(macroData == NULL || macroData -> tag != MACRO_FUNCTION)
		TOKEN_ERR_MACRO_DEFINE("fail to tokenize marco macroData can't find\n", NULL);

	Token_t* macro = macroData -> macroBody;
	if(macro == NULL || macro -> kind != TK_IDENT)
		TOKEN_ERR_MACRO_DEFINE("fail to tokenize marco\n", macro);

	token = preprocess(token);
	// Token_show_all(token);
	test_passed(test);
}

#define EXP_ERR(exp) do{fprintf(stderr, "error at %s\n", test); exit(1);}while(0)
void unit_test_preprocess_macro_expression() {
	char* test = "parsing Macro expression";
	char* arg = "10* 1";

	Token_t* token = lexical_analyze(arg);
	if(token == NULL)
		TOKEN_ERR(token);
	Expr* exp = parse_macro_expr(&token);
	if(exp == NULL || exp -> kind != Mul)
	{
		fprintf(stderr, "error at %s\n", test);
		if(exp)
			fprintf(stderr, "%d was expected but got %d\n", Mul, exp -> kind);
		exit(1);
	}
	arg = "a * (10 - 1)";
	token = lexical_analyze(arg);
	if(token == NULL)
		TOKEN_ERR(token);
	exp = parse_macro_expr(&token);
	if(exp == NULL || exp -> kind != Mul)
		EXP_ERR(exp);
	exp = exp -> left;
	if(exp == NULL || exp -> kind != Constant || exp -> value != 0)
	{
		fprintf(stderr, "kind %d,val %d", exp -> kind, exp -> value);
		EXP_ERR(exp);
	}
	test_passed(test);
}

void unit_test_preprocess_perse_defined() {
	char* test = "parse defined";
	char* arg = "defined MACRO";
	macros = make_Map();
	MacroData* macro = new_MacroData("MACRO", TAG_OBJECT, NULL, NULL);
	Map_add(macros,"MACRO", macro);

	Token_t* token = lexical_analyze(arg);
	Expr* exp = parse_macro_expr(&token);
	if(exp == NULL || exp -> kind != Constant || exp -> value != 1)
	{
		fprintf(stderr, "parse defined failed\n");
		if(exp == NULL)
			fprintf(stderr, "exp is NULL\n");
		if(exp -> kind != Constant)
			fprintf(stderr, "incorrect kind expression %d, but got %d\n", Constant, exp -> kind);
		if(exp -> value != 1)
			fprintf(stderr, "incorrect value expect %d, but got %d\n", 1, exp -> value);
		exit(1);
	}
	test_passed(test);
}


void unit_test_preprocess_macro_exp_eval() {
	char* test = "eval exp in macro";
	Expr ten = { Constant, 10, NULL, NULL};
	Expr eleven = {Constant, 11, NULL, NULL};
	Expr one = {Constant, 1, NULL, NULL};
	Expr zero = {Constant, 0, NULL, NULL};

	Expr add = { Add, 0, NULL, NULL};
	Expr sub = { Sub, 0, NULL, NULL};
	Expr mul = { Mul, 0, NULL, NULL};

	Expr ge = {Ge, 0, NULL, NULL};

	Expr or = { LogOr, 0, NULL, NULL};

	Expr input = { LogAnd, 0, &or, &one};
	or.left = &add;
	or.right = &ge;

	ge.left = &sub;
	ge.right = &zero;

	add.right = &ten;
	add.left = &zero;

	sub.left = &ten;
	sub.right = &eleven;

	if(eval_Expr(&input) != 1)
	{
		fprintf(stderr, "eval expr failed: expect 1 but got %d\n", eval_Expr(&input));
		exit(1);
	}
	test_passed(test);
}

void unit_test_preprocess_if() {
	char* test = "preprocess if endif test";
	char* arg = "#if defined MACRO\n #define MACROS 10\n#endif";

	macros = make_Map();
	Token_t* token = lexical_analyze(arg);
	if(Map_contains(macros, "MACROS"))
	{
		fprintf(stderr, "parse #if ... #endif failed");
		exit(1);
	}
	test_passed(test);
}

void unit_test_token_splice() {
	char* test = "token splice";
	Token_t* ins = new_Token_t(TK_CONST, new_Token_t(TK_EOF, NULL, 0, 0, NULL, NULL), 12, 0, NULL, NULL);
	Token_t* head = new_Token_t(TK_IDENT, NULL, 0, 0, NULL, NULL);
	Token_t* tail = new_Token_t(TK_EOF, NULL, 0, 0, NULL, NULL);
	head -> next = tail;
	Token_splice(ins, head, tail);

	Token_t* check = head -> next;
	if(check -> kind != TK_CONST || check -> val != 12) {
		fprintf(stderr, "splicing fail expected : kind -> %d, val -> %d but got \n", TK_CONST, 12);
		fprintf(stderr, "                         kind -> %d, val -> %d\n", check -> kind, check -> val);
		exit(1);
	}
	test_passed(test);
}

void unit_test_parse_static() {
	char * test = "parse_static";
	char* arg = "static int a = 0;";

	Token_t* token = lexical_analyze(arg);

	user_input = arg;
	parsing_here = arg;
	controller = NULL;

	Vector* asts = init_parser();

	program(&token, asts);

	Lvar* l = find_lvar("a", 1, &global);
	if(l == NULL){
		fprintf(stderr, "%s: got NULL", test);
		exit(1);
	}
	if(l -> storage_class != SC_STATIC) {
		fprintf(stderr, "SC_STATIC was expected but got %d", l -> storage_class);
		exit(1);
	}
	test_passed(test);
}

void unit_test_conditional_operator() {
	char* test = "conditional operator test";
	char* arg = "3? 1: 0;";
	Token_t* token = lexical_analyze(arg);

	token = token -> next;

	if(token == NULL) {
		fprintf(stderr, "lexical analyze: left is null");
		exit(1);
	}
	if(token -> kind != TK_OPERATOR || token -> str[0] != '?') {
		fprintf(stderr, "lexical analyze: expect '?' but got %c", token -> str[0]);
		exit(1);
	} 

	// parsing test 
	char* arg2 = "int main() { 3? 1: 0; return 0;}";
	Token_t *token2 = lexical_analyze(arg2);
	user_input = arg;
	parsing_here = arg;
	Vector* v = init_parser();
	program(&token2, v);
	Node_t* node = Vector_at(v, 0);
	node = node -> right -> left;
	if(node == NULL) {
		fprintf(stderr, "parse: expect not null");
		exit(1);
	}
	if(node -> kind != ND_CONDITIONAL) {
		fprintf(stderr, "parse: expect %d but got %d", ND_CONDITIONAL, node -> kind);
		exit(1);
	}
	Node_t* r = node -> right;
	if(r == NULL) {
		fprintf(stderr, "parse: expect not null (node -> right)");
		exit(1);
	}
	if(r -> kind != ND_CONDITIONAL_EXPRS) {
		fprintf(stderr, "parse: expect node -> r -> kind == %d but got %d", ND_CONDITIONAL_EXPRS, r -> kind);
		exit(1);
	}

	test_passed(test);
}

void uint_test_String_add() {
	char* test = "string add test";
	char* a = "abc";
	char* b = "abcd";
	char* c = String_add(a, b); 
	if(!String_compare(c, "abcabcd", 7)) {
		fprintf(stderr, "expected abcabcd but got %s", c);
		exit(1);
	}
	test_passed(test);
}

int unit_test() {
	unit_test_Vector();
	unit_test_String();
	unit_test_Map();
	unit_test_tokenize_struct();
	unit_test_parse_struct();
	unit_test_parse_struct_init();
	unit_test_tokenize_union();
	unit_test_tokenize_enum();
	unit_test_parse_union();
	unit_test_parse_enum();
	unit_test_parse_not();
	unit_test_tokenize_macro_define();
	unit_test_tokenize_macro_function();
	unit_test_preprocess_macro_expression();
	unit_test_preprocess_macro_exp_eval();
	unit_test_preprocess_perse_defined();
	unit_test_preprocess_if();
	unit_test_token_splice();
	unit_test_parse_static();
	unit_test_conditional_operator();
	uint_test_String_add();
	// unit_test_tokenize_include(); just show tokens
	return 0;
}
