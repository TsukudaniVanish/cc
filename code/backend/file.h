#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief vector that contains void*
 * 
 */
typedef struct vector Vector;
struct vector {
	void **container;
	unsigned int length;
	unsigned int allocsize;
};