#include "cc.h"

void assert(char *test_name,char *format,...)
{
	va_list arg;
	va_start(arg,format);
	fprintf(stderr,"\x1b[31mUnit test error at %s\xb1[m\n",test_name);
	fprintf(stderr,format,arg);
	va_end(arg);
	return;
}

void assert_int(char *test_name,long int expect, long int input)
{
	if(expect != input)
		assert(test_name,"%ld was expected but got %ld\n",expect,input);
	return;
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

	printf("Unit test: %s \x1b[32mpassed!!\x1b[m\n",test);
}
