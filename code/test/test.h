#if (defined INCLUDE_TEST) == 0
#define INCLUDE_TEST 100

int inc_test(int x) {
    return x * 100;
}

#else 

union this_is_not_compiled {};
#endif 

#ifdef THIS_IS_NOT_DEFINED
#else 
#define ELSE_OK 1
#endif 

#ifndef ELSE_OK
 error else  
#endif 

typedef struct {
    int a;
    int b;
}TestStruct;

#ifdef INCLUDE_TEST
typedef struct {
    int a;
}OK_IFDEF;
#endif 

#ifndef THIS_IS_NOT_DEFINED
#define THIS_WILL_BE_DEFINED 10
#endif 

# define SPACE_DEFINE 1

#ifndef THIS_WILL_BE_DEFINED
struct this_is_not_compiled = {};
#elif defined THIS_WILL_BE_DEFINED
#define ELIF_OK 1
#endif 

typedef struct struct_scope_test SCOPE_TEST;