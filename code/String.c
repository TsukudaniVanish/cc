#include "cc.h"
int Character_compare(char c, char d) {
	if(c == d) return 1;
	return 0;
}

/*
 * @brief return length of string not included  character '\0'.
*/
unsigned int String_len(char* s) {
	unsigned int res = 0;
	while(s[res]) res++;
	return res;
}

int String_compare(char* s1, char* s2, unsigned int size) {
	int res = 1;
	for(int i = 0; i < size; i++) {
		if(s1[i] && s2[i])
			res = res && Character_compare(s1[i], s2[i]);
		else
			return 0;
	}
	return res;
}

void Memory_copy(void* dest,void* source, unsigned int size) {
	char* d = dest;
	char* s = source;
	for(unsigned int i = 0; i < size ; i++)
	{
		*d++ = *s++;
	}
	return;
}

/*
 * @brief allocate memory and make new string has length len
*/
char* new_String(unsigned int len) {
	char* a = calloc(len, sizeof(char));
	a[len - 1] = '\0';
	return a;
}

char* String_add(char* s1, char* s2) {
	unsigned int len1 = String_len(s1);
	unsigned int len2 = String_len(s2);
	char* s = new_String(len1 + len2 + 1); // with '\0'
	Memory_copy(s, s1,len1);
	Memory_copy(s +len1,s2,len2);
	return s;
}

char* i2a(int d) {
	// check size 
	unsigned int b = Max(b, -b);
	unsigned int q = 0;
	while (b > 0)
	{
		b = b / 10;
		q++;
	}
	char* s = new_String(q + 1);
	int_to_string(s, d);
	return s;
}

char* l2a(long d) {
	unsigned int b = Max(b, -b);
	unsigned int q = 0;
	while (b > 0)
	{
		b = b / 10;
		q++;
	}
	char* s = new_String(q + 1);
	long_to_string(s, d);
	return s;
}


char* ui2a(unsigned int d) {
	long ld = d;
	return l2a(d);
}