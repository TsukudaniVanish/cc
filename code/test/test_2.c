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
	return 0;
}
