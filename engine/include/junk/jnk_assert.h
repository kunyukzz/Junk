#ifndef JUNK_ASSERT_H
#define JUNK_ASSERT_H

#include "jnk_define.h"

JNK_EXTERN_C_BEGIN

JNK_API void report(const char *expr, const char *msg, const char *file,
                    i32 line);

#ifdef JNK_ASSERT_ENABLE
#    ifdef _MSC_VER
#        include <intrin.h>
#        define debug_break() __debugbreak()
#    else
#        define debug_break() __builtin_debugtrap()
#    endif //_MSC_VER

#    ifdef JNK_DEBUG
#        define jnk_assert(expr)                                               \
            do {                                                               \
                if (!(expr)) {                                                 \
                    report(#expr, "", __FILE__, __LINE__);                     \
                    debug_break();                                             \
                }                                                              \
            } while (0)

#        define jnk_assert_msg(expr, message)                                  \
            do {                                                               \
                if (!(expr)) {                                                 \
                    report(#expr, message, __FILE__, __LINE__);                \
                    debug_break();                                             \
                }                                                              \
            } while (0)
#    else
#        define jnk_assert(expr)                                               \
            do {                                                               \
                if (!(expr)) {                                                 \
                    report(#expr, "", __FILE__, __LINE__);                     \
                    debug_break();                                             \
                }                                                              \
            } while (0)
#        define jnk_assert_msg(expr, message)                                  \
            do {                                                               \
                if (!(expr)) {                                                 \
                    report(#expr, message, __FILE__, __LINE__);                \
                    debug_break();                                             \
                }                                                              \
            } while (0)
#    endif // JNK_DEBUG

#else
#    define jnk_assert(expr) ((void)0)
#    define jnk_assert_msg(expr, message) ((void)0)
#endif // JNK_ASSERT_ENABLE

JNK_EXTERN_C_END
#endif // JUNK_ASSERT_H
