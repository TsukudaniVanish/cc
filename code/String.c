#include "cc.h"

extern void sprintf_h(char*, char*, long);

/*
 * @brief allocate memory and make new string has length len
*/
char* new_String(unsigned int len) {
	char* a = calloc(len + 1, sizeof(char));
	a[len] = '\0';
	return a;
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
 * @brief return length of string not included  character '\0'.
*/
unsigned int String_len(char* s) {
	unsigned int res = 0;
	while(s[res]) res++;
	return res;
}

int Character_compare(char c, char d) {
	if(c == d) return 1;
	return 0;
}

// 
/**
 * @brief This fuction compares s1 and s2 in the length of size.
 * s1 and s2 is assumed length <= size.
 * if s1 or s2 has length > size then return false(0).
 * 
 * @param s1 
 * @param s2 
 * @param size 
 * @return int 
 */
int String_compare(char* s1, char* s2, unsigned int size) {
	for(int i = 0; i < size; i++) {
		if(s1[i] && s2[i]){
			if(s1[i] != s2[i]) {
				return 0;
			}
			continue;
		}
		return  0;
	}
	return 1;
}

int String_compare_from_tail(char* s1, char* s2, unsigned int size) {
	unsigned l1 = String_len(s1);
	unsigned l2 = String_len(s2);

	if(l1 < size || l2 < size) {
		return 0;
	}
	for(unsigned i = 0; i < size; i ++) {
		if(s1[l1 - i - 1] != s2[l2 - i - 1]) {
			return 0;
		}
	}
	return 1;
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
	sprintf_h(s, "%d", d);
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
	sprintf_h(s, "%ld", d);
	return s;
}


char* ui2a(unsigned int d) {
	long ld = d;
	return l2a(d);
}

// This function finds a index of a givin character c.
// if  c is not found in string, it returns -1.
int String_index(char* s, char c) {
	for(int i = 0; s[i] != '\0'; i++) {
		if(s[i] == c) {
			return i;
		}
	}
	return -1;
}

// This function takes a substring from s.
// if start and end is not in 0 <= x <= len(s) then this function returns NULL.
// if end < start then this function returns NULL.
char* String_substring(char* s, int start , int end) {
	int sub_len = end - start;
	if(sub_len < 0) {
		return NULL;
	}
	if(start < 0) {
		return NULL;
	}
	char* substring = new_String(sub_len);
	for(int i = start; i < end; i++) {
		char d = s[i];
		if(d == '\0') {
			return NULL;
		}
		substring[i - start] = d;
	}
	return substring;
}

// This function separates a string s with separator sep.
Vector* String_split(char* s, char sep) {
	Vector* v = make_vector();
	int start = 0;
	while(s[start] != '\0') {
		int end = String_index(s + start, sep);
		if(end == -1) {
			Vector_push(v,String_substring(s, start, String_len(s)));
			break;
		}
		char* sub = String_substring(s, start, end + start);
		if(sub == NULL) {
			fprintf(stderr, "string split fail\ns: %s\nstart: %d\nend: %d\n", s,start, end + start);
			return v;
		}
		Vector_push(v, sub);
		start = end + start + 1;
	}
	return v;
}