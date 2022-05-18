#include "./test/test.h"
#include "./test/test.h"
void print_int(int x) {
	test_print_int(x);
	test_print("\n");
}

int main() {
	int x = 0;
	int y = 110;
	do{
		x++;
	}while(0);

	print_int(x);
	do{
		y--;
	}while(y > 0);

	print_int(y);

	print_int(inc_test(3));
	return 0;
}
