#include "dyn_array.h"
#include "platform/platform.h"
#include "junk/jnk_assert.h"
#include "junk/jnk_log.h"

#include <string.h>

#define DEFAULT_CAPACITY 0x01
#define RESIZE_FACTOR 0x02

enum { META_CAPACITY, META_LENGTH, META_STRIDE, META_COUNT };

void *arr_create(u64 capacities, u64 stride) {
    /* calculate space for metadata, array contents and allocate memory to give
     * space for both metadata and array contents.
     */
    u64 size_meta = META_COUNT * sizeof(u64);
    u64 size_array = capacities * stride;

    u64 *arr = platform_allocate(size_meta + size_array, false);
    if (!arr) return JNK_NULL;

    memset(arr, 0, size_meta + size_array);

    arr[META_CAPACITY] = capacities;
    arr[META_LENGTH] = 0;
    arr[META_STRIDE] = stride;

    /* give back allocation from above, but hide the metadata size, because
     * useful only the array contents.
     */
    void *data_ptr = (void *)(arr + META_COUNT);
    return data_ptr;
}

void arr_kill(void *arr) {
    /* go backwards to the metadata, then calculate size of metadata. then
     * calculate total memory (header of metadata + array contents.
     */
    u64 *header = (u64 *)arr - META_COUNT;
    u64 size_header = META_COUNT * sizeof(u64);
    u64 total = size_header + header[META_CAPACITY] * header[META_STRIDE];

    platform_free(header, total);
}

u64 arr_get_meta(void *arr, u64 field) {
    jnk_assert(arr != JNK_NULL && field < META_COUNT);
    u64 *header = (u64 *)arr - META_COUNT;
    return header[field];
}

void arr_set_meta(void *arr, u64 field, u64 value) {
    jnk_assert(arr != JNK_NULL && field < META_COUNT);
    u64 *header = (u64 *)arr - META_COUNT;
    header[field] = value;
}

void *arr_resize(void *arr) {
    if (!arr) return JNK_NULL;
    /* get current number of elements and the size of 1 element. then create
     * temporary array that can hold all the metadata x2 (RESIZE_FACTOR).
     */
    u64 length = jnk_darray_length(arr);
    u64 stride = jnk_darray_stride(arr);
    u64 old = jnk_darray_capacity(arr);

    void *temp = arr_create(RESIZE_FACTOR * old, stride);
    if (!temp) return JNK_NULL;

    /* update the metadata from calculation above.
     */
    memcpy(temp, arr, length * stride);
    arr_set_meta(temp, META_LENGTH, length);
    arr_kill(arr);
    return temp;
}

void *arr_push(void *arr, const void *value_ptr) {
    /* get current number of elements and the size of 1 element. and check if
     * number of current elements exciding the metadata capacities, resize it.
     */
    u64 length = jnk_darray_length(arr);
    u64 stride = jnk_darray_stride(arr);
    if (length >= jnk_darray_capacity(arr)) arr = arr_resize(arr);

    /* send to new element at the end of current last element. then update
     * metadata to knowing number of elements was increase.
     */
    uptr addr = (uptr)arr;
    addr += (length * stride);
    memcpy((void *)addr, value_ptr, stride);
    arr_set_meta(arr, META_LENGTH, length + 1);
    return arr;
}

void arr_pop(void *arr, void *target) {
    /* get current number of elements and the size of 1 element.
     */
    u64 length = jnk_darray_length(arr);
    u64 stride = jnk_darray_stride(arr);

    if (length == 0) return;

    /* remove the last element from the current elements, then update metadata
     * to knowing number of element was decreased.
     */
    void *src = (void *)((uptr)arr + (length - 1) * stride);
    memcpy(target, src, stride);
    arr_set_meta(arr, META_LENGTH, length - 1);
}

void *arr_pop_at(void *arr, u64 index, void *target) {
    /* get current number of elements and the size of 1 element. and if index
     * was too big, skip it.
     */
    u64 length = jnk_darray_length(arr);
    u64 stride = jnk_darray_stride(arr);
    if (index >= length) {
        jnk_log_warn(CH_MEMS,
                     "index outside of bounds. length: %lu, index: %lu.",
                     length, index);
        return arr;
    }

    uptr address = (uptr)arr;
    void *src = (void *)(address + index * stride);
    memcpy(target, src, stride);

    /* if not the last element, shift the other elements to fill the empty
     * index, then update metadata to knowing number of element was decreased.
     */
    if (index != length - 1) {
        void *dst = (void *)(address + index * stride);
        void *next = (void *)(address + (index + 1) * stride);
        u64 move_count = length - index - 1;
        memcpy(dst, next, move_count * stride);
    }

    arr_set_meta(arr, META_LENGTH, length - 1);
    return arr;
}

void *arr_insert_at(void *arr, u64 index, void *value_ptr) {
    /* get current number of elements and the size of 1 element. and if index
     * was too big, skip it. then if number of elements was exciding capacities
     * of metadata, resize it.
     */
    u64 length = jnk_darray_length(arr);
    u64 stride = jnk_darray_stride(arr);
    if (index >= length) {
        jnk_log_warn(CH_MEMS,
                     "index outside of bounds. length: %lu, index: %lu.",
                     length, index);
        return arr;
    }

    if (length >= jnk_darray_capacity(arr)) arr = arr_resize(arr);

    /* if not the last element, shift current elements to the right to make a
     * gap. then update metadata to knowing number of element was increase.
     */
    u64 address = (u64)arr;
    if (index != length - 1)
        memcpy((void *)(address + ((index - 1) * stride)),
               (void *)(address + (index * stride)), stride * (length - index));

    memcpy((void *)(address + (index * stride)), value_ptr, stride);
    arr_set_meta(arr, META_LENGTH, length + 1);
    return arr;
}

void *jnk_darray_init(u64 stride) {
    return arr_create(DEFAULT_CAPACITY, stride);
}

void *jnk_darray_reserved(u64 capacities, u64 stride) {
    return arr_create(capacities, stride);
}

void jnk_darray_kill(void **arr) {
    if (!arr || !*arr) return;
    arr_kill(*arr);
    *arr = JNK_NULL;
}

u64 jnk_darray_capacity(void *arr) { return arr_get_meta(arr, META_CAPACITY); }

u64 jnk_darray_length(void *arr) { return arr_get_meta(arr, META_LENGTH); }

u64 jnk_darray_stride(void *arr) { return arr_get_meta(arr, META_STRIDE); }

void jnk_darray_set_length(void *arr, u64 value) {
    arr_set_meta(arr, META_LENGTH, value);
}

void jnk_darray_push(void **arr, void *value, u64 stride) {
    if (!*arr) *arr = jnk_darray_init(stride);
    *arr = arr_push(*arr, value);
}

void jnk_darray_clear(void *arr) { arr_set_meta(arr, META_LENGTH, 0); }

void jnk_darray_pop(void *arr, void *value_ptr) { arr_pop(arr, value_ptr); }

void jnk_darray_pop_at(void *arr, u64 index, void *value_ptr) {
    arr_pop_at(arr, index, value_ptr);
}

void jnk_darray_insert_at(void **arr, u64 index, void *value, u64 stride) {
    (void)stride;
    *arr = arr_insert_at(*arr, index, value);
}
