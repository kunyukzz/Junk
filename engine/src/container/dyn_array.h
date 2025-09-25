#ifndef JUNK_DYNAMIC_ARRAY_H
#define JUNK_DYNAMIC_ARRAY_H

#include "junk/jnk_define.h"

JNK_EXTERN_C_BEGIN

void *jnk_darray_init(u64 stride);
void *jnk_darray_reserved(u64 capacities, u64 stride);
void jnk_darray_kill(void **arr);

u64 jnk_darray_capacity(void *arr);
u64 jnk_darray_length(void *arr);
u64 jnk_darray_stride(void *arr);
void jnk_darray_set_length(void *arr, u64 value);

void jnk_darray_push(void **arr, void *value, u64 stride);
void jnk_darray_clear(void *arr);
void jnk_darray_pop(void *arr, void *value_ptr);
void jnk_darray_pop_at(void *arr, u64 index, void *value_ptr);
void jnk_darray_insert_at(void **arr, u64 index, void *value, u64 stride);

JNK_EXTERN_C_END
#endif // JUNK_DYNAMIC_ARRAY_H
