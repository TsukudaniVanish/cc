#if (defined INCLUDE_TEST) == 0
#define INCLUDE_TEST 100

int inc_test(int x) {
    return x * 100;
}

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
#define THIS_WILL_DEFINE 10
#endif 

#ifndef THIS_WILL_DEFINE
struct this_is_not_compiled = {};
#endif 