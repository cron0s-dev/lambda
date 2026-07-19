#ifndef DYNAMIC_ARR_H
#define DYNAMIC_ARR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    void *data;
    size_t elem_size;
    size_t len;
    size_t cap;
} DynArr;

DynArr *da_init(size_t elem_size);
static int da_grow(DynArr *arr);
int da_push(DynArr *arr, const void *elem);
void da_pop(DynArr *arr);
void da_free(DynArr **arr);

#ifdef DA_IMPLEMENTATION 

DynArr *da_init(size_t elem_size)
{
    DynArr *arr = malloc(sizeof(*arr));

    arr->elem_size = elem_size;
    arr->data = NULL;
    arr->len = 0; 
    arr->cap = 0;

    return arr;
}

static int da_grow(DynArr *arr)
{
    void *data = NULL;

    if (!arr->cap) {
        data = malloc(arr->elem_size);
        if (!data)
            return -1;

        ++arr->cap;
        arr->data = data;
        return 0;
    }

    size_t new_cap = arr->cap * 2;
    data = realloc(arr->data, new_cap * arr->elem_size);
    if (!data)
        return -2;

    arr->cap = new_cap;
    arr->data = data;
    return 0;
}

int da_push(DynArr *arr, const void *elem)
{
    if (arr->len == arr->cap)
        if (da_grow(arr) != 0)
            return -1;

    void *dest = (char*)arr->data + arr->len * arr->elem_size;
    memcpy(dest, elem, arr->elem_size);

    arr->len++;
    return 0;
}

void da_pop(DynArr *arr)
{
    if (arr->len == 0)
        return;

    --arr->len;
}

void da_free(DynArr **arr)
{
    free((*arr)->data);

    (*arr)->data = NULL;
    (*arr)->len = 0;
    (*arr)->cap = 0;
    (*arr)->elem_size = 0;

    free(*arr);
}

#endif

#endif
