#ifndef JUNK_DEFINE_H
#define JUNK_DEFINE_H

#define JNK_NULL (void *)(0)
#define JNK_CLAMP(value, min, max)                                             \
    (value <= min) ? min : (value >= max) ? max : value

#define JNK_INVALID_8 0xFFu
#define JNK_INVALID_16 0xFFFu
#define JNK_INVALID_32 0xFFFFFFFFu
#define JNK_INVALID_64 0xFFFFFFFFFFFFFFFFu
#define true 1
#define false 0

#ifdef __cplusplus
#    define JNK_EXTERN_C_BEGIN extern "C" {
#    define JNK_EXTERN_C_END }
#else
#    define JNK_EXTERN_C_BEGIN
#    define JNK_EXTERN_C_END
#endif

#ifndef JNK_LINUX
#    if defined(__linux__) || defined(__gnu_linux__)
#        define JNK_LINUX 1
#        ifndef _POSIX_C_SOURCE
#            define _POSIX_C_SOURCE 200809L
#        endif
#    endif
#endif

#ifndef JNK_WINDOWS
#    if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
#        define JNK_WINDOWS 1
#    endif
#endif

#if defined(__clang__) || defined(__gcc__)
#    define STATIC_ASSERT(cond, msg)                                           \
        typedef char static_assertion_##msg[(cond) ? 1 : -1]
#else
#    define STATIC_ASSERT static_assert
#endif

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char i8;
typedef signed short i16;
typedef signed int i32;
typedef signed long long i64;
typedef float f32;
typedef double f64;
typedef _Bool b8;
typedef unsigned long uptr;

STATIC_ASSERT(sizeof(u8) == 1, u8_must_be_1_byte);
STATIC_ASSERT(sizeof(u16) == 2, u16_must_be_2_byte);
STATIC_ASSERT(sizeof(u32) == 4, u32_must_be_4_byte);
STATIC_ASSERT(sizeof(u64) == 8, u64_must_be_8_byte);
STATIC_ASSERT(sizeof(i8) == 1, i8_must_be_1_byte);
STATIC_ASSERT(sizeof(i16) == 2, i16_must_be_2_byte);
STATIC_ASSERT(sizeof(i32) == 4, i32_must_be_4_byte);
STATIC_ASSERT(sizeof(i64) == 8, i64_must_be_8_byte);
STATIC_ASSERT(sizeof(uptr) == 8, up8_must_be_8_byte);

#ifdef JNK_EXPORT
#    ifdef _MSC_VER
#        define JNK_API __declspec(dllexport)
#    else
#        define JNK_API __attribute__((visibility("default")))
#    endif
#else
#    ifdef _MSC_VER
#        define JNK_API __declspec(dllimport)
#    else
#        define JNK_API
#    endif
#endif

#if defined(__clang__) || defined(__gcc__)
#    define JNK_INL __attribute__((always_inline)) inline
#    define JNK_NOINL __attribute__((noinline))
#elif defined(_MSC_VER)
#    define JNK_INL __forceinline
#    define JNK_NOINL __declspec(noinline)
#endif

#ifdef _MSC_VER
#    define JNK_ALIGNAS __declspec(align(16))
#else
#    define JNK_ALIGNAS __attribute__((aligned(16)))
#endif

#endif // JUNK_DEFINE_H
