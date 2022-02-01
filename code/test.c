int Character_conpair(char c, char d) {
	if(c == d) return 1;
	return 0;
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
int main(){
    test_print("\x1b[32mHello\x1b[m\n"); /* display test */
	test_arithmetic();
	test_pointer_array_access();
	test_increment_postfix();
	test_decrement_postfix();	
	test_inc_dec_prefix();
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
	
	test_global();
	test_print_int(Character_conpair(2,2));
	test_print("\n");
	return 0;
}
