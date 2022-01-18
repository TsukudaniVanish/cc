/**
 * @file Vector.c
 * @author Y.Naito
 * @brief vector is void* container
 * @version 0.1
 * @date 2021-12-29
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "cc.h"
extern void Memory_copy(void*,void*,unsigned int);

Vector *new_Vector(size_t alloc)
{
    Vector *vec = calloc(1,sizeof(Vector));
    vec -> allocsize = alloc;
    vec -> container = calloc(alloc,sizeof(void *));
    vec -> length = 0;
    return vec;
}

Vector *make_vector()
{
    return new_Vector(VEC_MIN_SIZE);
}

int _is_acceptable(Vector *vec,size_t _rsvlen)
{
    if(vec -> allocsize - vec -> length < _rsvlen)
        return 0;
    return 1;
}

void _maybe_realloc(Vector* vec)
{
    if(_is_acceptable(vec,1) == 0)
        return;
    void* new_container = malloc(vec -> length * 2 * sizeof(void*));
    Memory_copy(new_container,vec -> container,vec -> length * sizeof(void *));

    vec -> container = new_container;
    vec -> allocsize = vec -> length *2;
}

int Vector_get_length(Vector* vec)
{
    if(vec)
        return vec -> length;
    fprintf(stderr,"未作成のベクタの長さは未定義です\n");
    exit(1);
}

void Vector_push(Vector* vec,void* x)
{
    _maybe_realloc(vec);
    vec -> container[vec -> length] = x;
    vec -> length ++;
}

void* Vector_pop(Vector *vec)
{
    vec -> length --;
    if(vec -> length >= 0)
        return vec -> container[vec -> length];
    else
    {
        fprintf(stderr,"invailed pointer access");
        exit(1);
    }
}

void Vector_replace(Vector* vec, size_t index,void * p)
{
    if(index > vec -> length -1)
        return;
    vec -> container[index] = p;
}

void* Vector_at(Vector* vec, size_t index)
{
    if(index +1 > Vector_get_length(vec))
    {
        fprintf(stderr,"vectorの長さ以上の要素にはアクセスできません\n");
        exit(1);
    }
    return vec -> container[index];
}

void *Vector_get_tail(Vector* vec)
{
    
    return Vector_at(vec,Vector_get_length(vec)-1);
}
