int Character_conpair(char c, char d) {
	if(c == d) return 1;
	return 0;
}
unsigned int String_len(char* s) {
	unsigned int res = 0;
	while(s[res]) res++;
	return res;
}

int String_conpair(char* s1, char* s2, unsigned int size) {
	if(String_len(s1) == 0 || String_len(s2) == 0 || String_len(s1) < size || String_len(s2) < size) return 0;
	int res = 1;
	for(int i = 0; i < size; i++) {
		res = res && Character_conpair(s1[i], s2[i]);
	}
	return res;
}

void error_template(char* test, char *tmp) {
	test_print("error at");
	test_print(test);
	test_print("\n");
	test_print(tmp);
	test_print("\n");
	test_error();
}

void error_template_int(char *test_name,int expect , int a)
{
    test_print("\x1b[31merror\x1b[m at ");
    test_print(test_name);
    test_print("\n");
    test_print_int(expect);
    test_print(" was expected but got: \x1b[31m");
    test_print_int(a);
    test_print("\x1b[m\n");
    test_error();
}
void test_passed(char *p)
{
    test_print("\x1b[32m");
    test_print(p);
    test_print(" passed!!");
    test_print("\x1b[m\n");
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

void test_braket() {
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
	test_braket();
	test_unit_pm();
	test_passed("	arithmetic test");
	test_print("\n");
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

void test_array_accsess() {

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
	test_array_accsess();
	test_passed("	pointer array access test");
	test_print("\n");
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
		test_print("value of ww is ");
		test_print_int(ww);
		test_print("\n");
		error_template_int(test, 3, a[1]);
	}
	test_passed(test);
}

void test_decrement_postfix() {
	
	char * test = "decrement test";
	int x = 2;
	x--;
	if(x != 1)
	{
		error_template_int(test, 1, x);
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
	x = 100;
	--x;
	if(x != 99)
	{
		error_template_int(test, 99, x);
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
		test_print_int(i);
		test_print("\n");
		test_print("error in:");
		test_print(test);
		test_print("\n");
		test_error();
	}
	i = 1;
	if(x && z)
	{
		test_print_int(i);
		test_print("\n");
		test_print("error in:");
		test_print(test);
		test_print("\n");
		test_error();
	}
	i = 2;
	int* p = 0;
	if(z && *p)
	{
		test_print_int(i);
		test_print("\n");
		test_print("error in:");
		test_print(test);
		test_print("\n");
		test_error();
	}
	i = 3;
	if(x || z)
	{
		/*ok*/
	}
	else
	{
		test_print_int(i);
		test_print("\n");
		test_print("error in:");
		test_print(test);
		test_print("\n");
		test_error();	
	}
	i = 4;
	if(y || *p)
	{
		/*ok*/
	}
	else
	{
		test_print_int(i);
		test_print("\n");
		test_print("error in:");
		test_print(test);
		test_print("\n");
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
	if(Character_conpair(*a.c, *d) == 0)
	{
		test_print("error at struct test");
		test_error();
	}
	struct Hi* b = &a;
	if(b -> a != 10)
	{
		error_template_int(test, 10, b -> a);
	}
	if(String_len(b -> c) != 2 || String_conpair(a.c, b -> c, 2) == 0)
	{
		test_print("error at struct test\n");
		test_error();
	}

	test_passed(test);
	/*
	{
		test_print("error at");
		test_print(test);
		test_print("\n");
		test_print(a.c);
		test_error();
	}*/
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
	if(String_len(a.greeting) != 5 || String_conpair(a.greeting, "Hello", 5) == 0)
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
		test_print("\x1b[32m	macro TEST_DEFINE complied successfully! \x1b[m\n");
	int g = 10;
	if(timesTen(g) == 100)
		test_print("\x1b[32m 	macro timesTen complied successfully!\x1b[m\n");
	else
	{
		test_print("failed to complie timesTen: got ");
		test_print_int(timesTen(g));
		test_print("\n");
	}
	if(macro_in_macro(g) == 1000)
	{
		test_print("\x1b[32m 	macro in macro complied successfully!\x1b[m\n");
	}
	else
	{
		test_print("failed to complie macro in macro: got ");
		test_print_int(macro_in_macro(g));
		test_print("\n");
	}
	if(DEFINEOK != 20)
	{
		test_print("failed to complie macro in macro: got ");
		test_print_int(DEFINEOK);
		test_print("\n");
	}
	else
	{
		test_print("\x1b[32m 	conditional preprocess complied successfully!\x1b[m\n");
	}
	return;
}

#define TEST_DEFINE 100
#define timesTen(a) a*10
#define macro_in_macro(b) b*TEST_DEFINE
#if defined TEST_DEFINE
	#define DEFINEOK 20
#endif
int main(){// line comment 
    test_print("\x1b[32mHello\x1b[m\n"); /* display test */
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
	
	test_global();
	test_preprocess();
	
		
	return 0;
}
