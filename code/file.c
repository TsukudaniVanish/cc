#include "cc.h"
//#include<stdarg.h>
#include<string.h>
#include<errno.h>
#include<dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

extern int String_len(char*);
extern int String_compare(char*, char*, unsigned int);
extern int String_compare_from_tail(char* s1, char* s2, unsigned int size);
extern char* String_add(char* , char*);

void error(char *fmt,...)
{
	va_list arg;
	va_start(arg,fmt);
	fprintf(stderr,fmt,arg);
	va_end(arg);
	exit(1);
}


char* file_open(char* path)
{
	FILE *fp = fopen(path , "r");
	if(!fp)
		error("Failed to find file. given pass: %s\n error: %s",path,strerror(errno));
	
	//check file length
	if(fseek(fp,0,SEEK_END) == -1)
	{
		error("%s : fseek :%s",path,strerror(errno));
	}
	
	size_t size = ftell(fp);

	if(fseek(fp,0,SEEK_SET) == -1)
	{
		error("%s : fseek :%s",path,strerror(errno));
	}

	char *buf = calloc(1,size +2);
	fread(buf,size,1,fp);

	//make sure file ended at newline
	if(size == 0 || buf[size -1] != '\n')
	{
		buf[size] = '\n';
	}
	buf[size] = '\0';
	fclose(fp);

	return buf;
	
}

int is_file(char* path) {
	struct stat path_stat;
	if(stat(path, &path_stat) != 0) {
		return 0;
	}
	return S_ISREG(path_stat.st_mode);
}

int is_dir(char* path) {
	struct stat path_stat;
	if(stat(path, &path_stat) != 0) {
		return 0;
	}
	return S_ISDIR(path_stat.st_mode);
}

// This function reads files or dirs from the dir of a given dir_path and return 0.
// Founded directory contents are stored file_container or dir_container with its path.
// Directory path always end with '/'.
// if error occur return -1.
int aggregate_names_of_contents_from_dir(char* dir_path /* example: ./directory/ */, Vector* file_container, Vector* dir_container) {
	if(file_container == NULL || dir_container == NULL) {
		fprintf(stderr, "container is NULL\nIs vec null: %d\nIs dir null: %d\n", file_container == NULL, dir_container == NULL);
		return -1;
	}
	DIR* target_dir = opendir(dir_path);
	if(target_dir == NULL) {
		fprintf(stderr, "can't open dir");
		return -1;
	}
	struct dirent* content = readdir(target_dir);
	while(content != NULL) {
		if(String_compare(content -> d_name, ".", String_len(content -> d_name))) {
			content = readdir(target_dir);
			continue;
		}
		if(String_compare(content -> d_name, "..", String_len(content -> d_name))) {
			content = readdir(target_dir);
			continue;
		}
		char* path = String_add(dir_path, content -> d_name);

		if(is_file(path) ){
			Vector_push(file_container, path);
			content = readdir(target_dir);
			continue;
		}
		if(is_dir(path)) {
			Vector_push(dir_container, String_add(path, "/"));
			content = readdir(target_dir);
			continue;
		}
		content = readdir(target_dir);
	}
	return 0;
}

// This function search target_name from file_path_container.
// Matching start from tail.
// If it can't find file, then return NULL.
char* find_file_from_file_path_container(Vector* file_path_container, char* target_name) {
	unsigned l = String_len(target_name);

	while(file_path_container -> length > 0) {
		char* file_path = Vector_pop(file_path_container);

		if(String_compare_from_tail(file_path, target_name, l)) {
			return file_path;
		}
	}
	return NULL;
}


// This function searches file from root_path recursively down file tree.
// If it failed to find file, it returns NULL 
char* find_file_from(char* root_path/* example: ./dirname/ */, char* target_name) {
	char* path = root_path;
	Vector* d = make_vector();
	Vector* f = make_vector();
	Vector_push(d, path);
	while (d->length > 0)
	{
		path = Vector_pop(d);
		if(aggregate_names_of_contents_from_dir(path, f, d) == -1) {
			fprintf(stderr, "Error: in searching file.path: %s\n", path);
			exit(1);
		}

		char* file_path = find_file_from_file_path_container(f, target_name);
		if(file_path != NULL) {
			return file_path;
		}
	}
	return NULL;
}