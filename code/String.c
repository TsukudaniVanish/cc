#include "cc.h"
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
	int res = 1;
	for(int i = 0; i < size; i++) {
		if(s1[i] && s2[i])
			res = res && Character_conpair(s1[i], s2[i]);
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

