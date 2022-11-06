#include "./test/test.h"

void test_print_int(int p)
{
    printf_h("%d",p);
}

void test_error()
{
    exit_h(1);
}

int Character_compare(char c, char d) {
	if(c == d) return 1;
	return 0;
}
unsigned int String_len(char* s) {
	unsigned int res = 0;
	while(s[res]) res++;
	return res;
}

int String_compare(char* s1, char* s2, unsigned int size) {
	if(String_len(s1) == 0 || String_len(s2) == 0 || String_len(s1) < size || String_len(s2) < size) return 0;
	int res = 1;
	for(int i = 0; i < size; i++) {
		res = res && Character_compare(s1[i], s2[i]);
	}
	return res;
}

void error_template(char* test, char *tmp) {
	printf_h("error at");
	printf_h(test);
	printf_h("\n");
	printf_h(tmp);
	printf_h("\n");
	test_error();
}

void error_template_int(char *test_name,int expect , int a)
{
    printf_h("\x1b[31merror\x1b[m at ");
    printf_h(test_name);
    printf_h("\n");
    test_print_int(expect);
    printf_h(" was expected but got: \x1b[31m");
    test_print_int(a);
    printf_h("\x1b[m\n");
    test_error();
}
void test_passed(char *p)
{
    printf_h("\x1b[32m");
    printf_h(p);
    printf_h(" passed!!");
    printf_h("\x1b[m\n");
}

void test_add_sub() {
	char * test = "add and sub test";
	int x = 20 + 5 - 4;
    if(x != 21)
    {
        error_template_int(test,21,x);
    }
    test_passed(test);
}

void test_bracket() {
	char *test = "bracket test";
    int x = (20 -10)*5;
    if(x != 50)
    {
        error_template_int(test,50,x);
    }
    test_passed(test);
}

void test_unit_pm() {

	char *test = "unit +,- test";
    int x = +10;
    if( x != 10)
    {
        error_template_int(test,10,x);
    }
    x = -10;
    if(x != -10)
    {
        error_template_int(test,-10,x);
    }
    test_passed(test);
}

void test_arithmetic() {
	test_add_sub();
	test_bracket();
	test_unit_pm();
	test_passed("	arithmetic test");
	printf_h("\n");
}

void test_ref_deref() {

	char *test = "ref deref test";
    int x;
    int *y;
    y = &x;
    *y = 3;
    if(x != 3)
    {
        error_template_int(test,3,x);
    }
    test_passed(test);
}

void test_arraycast () {

	char *test = "array type cast test";
    int a[2];
    *(a + 1) = 2;
    int *p;
    p = a;
    if(*(p + 1) != 2)
    {
        error_template_int(test,2,*(p+1));
    }
    test_passed(test);
}

void test_array_access() {

	char *test = "array index access test";
    int a[2];
    a[1] = 3;
    if(a[1] != 3)
    {
        error_template_int(test,3,a[1]);
    }
    test_passed(test);
}

void test_pointer_array_access() {
	test_ref_deref();
	test_arraycast();
	test_array_access();
	test_passed("	pointer array access test");
	printf_h("\n");
}

void test_increment_postfix() {
	char *test = "increment test";
	int zz = 100;
	zz++;
	if(zz != 101)
	{
		error_template_int(test,101,zz);
	}
	int ww = zz++ - 99;
	if(zz != 102)
	{
		error_template_int(test, 102, zz);
	}
	ww = 0;
	for(int i = 0; i < zz ; i++)
	{
		ww++;
	}
	if(ww != 102)
	{
		error_template_int(test, 102, ww);
	}
	int a[2];
	a[0] = 1;
	a[1] = 2;
	int *b = a;
	b++;
	if(*b != 2)
	{
		error_template_int(test, 2, *b);
	}
	ww = a[1]++;
	if(a[1] != 3)
	{
		printf_h("value of ww is %d\n");
		error_template_int(test, 3, a[1]);
	}
	test_passed(test);
}

void test_decrement_postfix() {
	
	char * test = "decrement test";
	int x = 2;
	x--;
	if(x++ != 1)
	{
		error_template_int(test, 1, x);
	}
	if( x != 2) {
		error_template_int(test, 2, x);
	}
	int a[2];
	a[0] = 22;
	a[1] = 2;
	int *b = a + 1;
	b--;
	if(*b != 22)
	{
		error_template_int(test, 22, *b);
	}
	test_passed(test);
}

void test_inc_dec_prefix() {
	char *test = "increment decrement prefix";
	int x = 100;
	++x;
	if(x != 101) 
	{
		error_template_int(test, 101, x);
	}
	x = --x;
	if(x != 100)
	{
		error_template_int(test, 100, x);
	}
	test_passed(test);
}

void test_logic() {
	char* test = "&& || test";
	int x = 1;
	int y = 11;
	int z = 0;
	int i = 0;
	if(x && y)
	{
		/* ok */
	}
	else
	{
		printf_h("%d:\nerror in: %s\n", i, test);
		test_error();
	}
	i = 1;
	if(x && z)
	{
		printf_h("%d:\nerror in: %s\n", i, test);
		test_error();
	}
	i = 2;
	int* p = 0;
	if(z && *p)
	{
		printf_h("%d:\nerror in: %s\n", i, test);
		test_error();
	}
	i = 3;
	if(x || z)
	{
		/*ok*/
	}
	else
	{
		printf_h("%d:\nerror in: %s\n", i, test);
		test_error();
	}
	i = 4;
	if(y || *p)
	{
		/*ok*/
	}
	else
	{
		printf_h("%d:\nerror in: %s\n", i, test);
		test_error();
	}
	test_passed(test);
	
}

void test_struct() {
	char* test = "struct test";
	struct Hi {
		int a;
		int b;
		char* c;
	};
	struct Hi a = {10, 11, "Hi"};
	if(a.a != 10)
	{
		error_template_int(test, 10, a.a);
	}
	if(a.b != 11)
	{
		error_template_int(test, 11, a.b);
	}
	char* d = "Hi";
	if(Character_compare(*a.c, *d) == 0)
	{
		printf_h("error at struct test");
		test_error();
	}
	struct Hi* b = &a;
	if(b -> a != 10)
	{
		error_template_int(test, 10, b -> a);
	}
	if(String_len(b -> c) != 2 || String_compare(a.c, b -> c, 2) == 0)
	{
		printf_h("error at struct test\n");
		test_error();
	}

	test_passed(test);
}
void test_array_init() {
	char* test = "array init test";
	int a[3] = {100, 101, 102};
	if(a[1] != 101)
	{
		error_template_int(test, 101, a[1]);
	}
	if(a[0] != 100)
	{
		error_template_int(test, 100, a[0]);
	}
	if(a[2] != 102)
	{
		error_template_int(test, 102, a[2]);
	}
	test_passed(test);
}

void test_union() {
	char* test = "union test";
	union Hi {
		char* greeting;
		int a;
	};
	union Hi a;
	a.greeting = "Hello";
	if(String_len(a.greeting) != 5 || String_compare(a.greeting, "Hello", 5) == 0)
	{
		error_template(test, a.greeting);
	}
	a.a = 4;
	if(a.a != 4)
	{
		error_template_int(test, 4, a.a);
	}
	test_passed(test);
}

void test_enum() {
	char* test = "enum test";
	enum Hi {
		HELLO,
		HI,
		GOODMORNING = 10,
	};
	int a = HELLO;
	if(a != 0)
		error_template_int(test, 0, a);
	if(GOODMORNING != 10)
		error_template_int(test, 10, 10);
	enum Hi greeting = HI;
	if(greeting != 1)
		error_template_int(test, 1, greeting);
	test_passed(test);
}

void test_log_not() {
	char* test = "log not test";
	int a = 100;
	if(!a)
	{
		error_template_int(test, 0, !a);
	}
	
	if(!0)
	{/* do nothing */}
	else
	{
		error_template_int(test, 0, !0);
	}
	test_passed(test);
}

int g = 100;
char a[11];
void test_global() {
	char * test ="global variable test";
	if(g != 100) {
		error_template_int(test, 100, g);
	}
	int g = 11;
	if(g != 11)
	{
		error_template_int(test, 11, g);
	}
	a[9] = 11;
	if(a[9] != 11)
	{
		error_template_int(test, 11, a[9]);
	}
	test_passed("global variable test");
}

void test_break() {
	char *test = "test break";
	int x = 0;
	while(x < 100) {
		if(x == 14) break;
		x++;
	}
	if(x != 14)
	{
		error_template_int(test, 14, x);
	}
	test_passed(test);
}

void test_continue() {
	char *test = "test continue";

	int x = 0;
	int y = 0;

	while(x < 20)
	{
		x++;
		if(x > 9)
		{
			continue;
		}
		y++;
	}

	if(y != 9)
	{
		error_template_int(test, 9, y);
	}
	test_passed(test);
}

void test_preprocess() {
	if(TEST_DEFINE == 100)
		printf_h("\x1b[32m	macro TEST_DEFINE complied successfully! \x1b[m\n");
	int g = 10;
	if(timesTen(g) == 100)
		printf_h("\x1b[32m 	macro timesTen complied successfully!\x1b[m\n");
	else
	{
		printf_h("failed to compile timesTen: got ");
		test_print_int(timesTen(g));
		printf_h("\n");
	}
	if(macro_in_macro(g) == 1000)
	{
		printf_h("\x1b[32m 	macro in macro complied successfully!\x1b[m\n");
	}
	else
	{
		printf_h("failed to compile macro in macro: got ");
		test_print_int(macro_in_macro(g));
		printf_h("\n");
	}
	if(DEFINEOK != 20)
	{
		printf_h("failed to compile macro in macro: got ");
		test_print_int(DEFINEOK);
		printf_h("\n");
	}
	else
	{
		printf_h("\x1b[32m 	conditional preprocess complied successfully!\x1b[m\n");
	}
	return;
}

void test_switch() {
	char * test = "switch test";
	int x = 10;
	switch(x) {
		case 11:
			printf_h("failed to compile!");
			error_template_int(test, 11, x);
		case 12:
			printf_h("failed to compile!");
			error_template_int(test, 12, x);
		case 10:
			printf_h("\x1b[32mOK!\x1b[m\n");
		default:
			x ++;
	}
	if(x != 11) {
		printf_h("Oh , default was not compiled?");
		error_template_int(test, 11, x);
	}

	switch(x) {
		case 20:
			printf_h("failed to compile!");
			error_template_int(test, 20, x);
		case 21:
			printf_h("failed to compile!");
			error_template_int(test, 20, x);
	}

	test_passed(test);
}

void test_typedef() {
	char* test = "typedef test";
	typedef struct {
		int a;
		int b;
	}Test;
	Test t = {10, 11};
	
	if(t.a != 10) {
		error_template_int(test, 10, t.a);
	}
	if(t.b != 11) {
		error_template_int(test, 11, t.b);
	}

	if(sizeof(t) != 8)
	{
		error_template_int(test, 8, sizeof(t));
	}
	test_passed(test);
}

void test_sizeof() {
	char *test = "sizeof type test";
	typedef struct {
		int a;
		int b;
	}Test;

	if(sizeof(int) != 4) {
		error_template_int(test, 4, sizeof(int));
	}
	if(sizeof (Test) != 8) {
		error_template_int(test, 8, sizeof(Test));
	}

	test_passed(test);
}

void test_do_while() {
	char* test = "do while test";
	int x = 0;
	int y = 110;
	do{
		x++;
	}while(0);

	if(x != 1) {
		error_template_int(test, 1, x);
	}

	do{
		y--;
	}while(y > 0);

	if(y != 0) {
		error_template_int(test, 0, y);
	}
	test_passed(test);
}

void test_include() {
	char* test = "include test";

	if(inc_test(3) != 300) {
		error_template_int(test, 300, inc_test(3));
	}
	if(INCLUDE_TEST != 100) {
		error_template(test, "include fail at #define");
	}
	test_passed(test);
}

int helper_func_for_conditional(int x) {
	return x * 10;
}
void test_conditional_operator() {
	char* test = "conditional operator test";
	int a = 0? 10: 11;
	if(a != 11) {
		error_template_int(test, 11, a);
	}
	a = 1? 100: 101;
	if(a != 100) {
		error_template_int(test, 100, a);
	}
	a = helper_func_for_conditional(0)? 0: helper_func_for_conditional(2);
	if(a != 20) {
		error_template_int(test, 20, a);
	}
	test_passed(test);
}

void test_long_long_int() {
	char* test = "long long int";
	long a = 1100;
	if(a != 1100) {
		error_template_int(test, 1100, a);
	}
	long int b[2] = {1, 2};
	if(b[1] != 2) {
		error_template_int(test, 2, b[1]);
	}
	if(sizeof a != 8) {
		error_template_int(test, 8, sizeof a);
	}
	test_passed(test);
}

void test_type_cast() {
	char* test = "type cast test";
	int a = 100;
	long b = 100;
	if(sizeof((long) a) != sizeof b) {
		error_template_int(test, sizeof b, sizeof((long) a));
	}
	if(sizeof(char) != 1){
		error_template_int(test, 1, sizeof(char));
	}
	test_passed(test);
}

void test_character_literal() {
	char* test = "charater literal test";
	char a = 'H';
	if(a != 'H') {
		error_template_int(test, 'H', a);
	}
	a = '\n';
	if(a != '\n') {
		error_template_int(test, '\n', a);
	}
	test_passed(test);
}

void test_ifdef_ifndef() {
	char* test = "ifdef ifndef";
	OK_IFDEF ok = {100};
	if(THIS_WILL_BE_DEFINED != 10) {
		error_template_int(test, 10, THIS_WILL_BE_DEFINED);
	}
	test_passed(test);
}

extern void test_place_holder(int, int, char*, ...);
extern OK_IFDEF* test_type_alias_function_declare(int, char*, struct struct_scope_test,OK_IFDEF, OK_IFDEF*);

#define TEST_DEFINE 100
#define timesTen(a) a*10
#define macro_in_macro(b) b*TEST_DEFINE
#if defined TEST_DEFINE
	#define DEFINEOK 20
#endif
#ifndef ELIF_OK
	fail to compile 
#endif 
int main(){// line comment 
    printf_h("\x1b[32mHello\x1b[m\n"); /* display test */
	test_arithmetic();
	test_pointer_array_access();
	test_increment_postfix();
	test_decrement_postfix();	
	test_inc_dec_prefix();
	test_logic();
	test_struct();
	test_array_init();
	test_union();
	test_enum();
	test_log_not();
	test_break();
	test_continue();
	test_switch();
	test_typedef();
	test_sizeof();
	test_do_while();
	test_include();
	test_conditional_operator();
	test_long_long_int();
	test_type_cast();
	test_character_literal();
	test_ifdef_ifndef();


	test_global();
	test_preprocess();
		
	return 0;
}
