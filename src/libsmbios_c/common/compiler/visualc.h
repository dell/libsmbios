#define UNREFERENCED_PARAMETER(P)  (P)

#define strtoll(p, e, b) _strtoi64(p, e, b)

#define __internal
#define __hidden

// variadic macros added in vc 8.0: http://msdn.microsoft.com/en-us/library/ms177415(VS.80).aspx


#define _env_dbg_printf(env, format, ...) \
    do { \
        char w[256] = "LIBSMBIOS_C_"; \
        strncat(w, env, 256 - 1 - strlen(w));   \
        const char *v = 0;  \
        const char *u = getenv("LIBSMBIOS_C_DEBUG_OUTPUT_ALL");    \
        if (env) v = getenv(w);    \
        if ( (u && atoi(u) > 0) || (v && atoi(v) > 0) ) {   \
        fprintf(stderr , format , __VA_ARGS__); \
        fflush(NULL); \
        }\
    } while(0)

#define _stderr_dbg_printf(format, ...) do { fprintf(stderr , format , __VA_ARGS__);  } while(0)

#define _null_call(...) do {} while(0)

// default to env-controlled
#if !defined(DEBUG_OUTPUT_ALL) && !defined(SUPRESS_DEBUGGING_OUTPUT)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#  define dbg_printf(args...) _env_dbg_printf( DEBUG_MODULE_NAME, ## args )

#elif defined(DEBUG_OUTPUT_ALL)
#  include <stdio.h>
#  define dbg_printf _stderr_dbg_printf

#elif defined(SUPRESS_DEBUGGING_OUTPUT)
#define dbg_printf _null_call
#endif

#define fnprintf(fmt, ...)  do { dbg_printf("%s: ", __FUNCTION__); dbg_printf( fmt, __VA_ARGS__ ); } while(0)

// turn off the warnings before we #include anything
// 4503: warning: decorated name length exceeded
// 4250: 'class1' : inherits 'class2::member' via dominance
// 4201: nonstandard extension used : nameless struct/union
// 4127: warning: conditional expression is constant
#pragma warning( disable : 4201 4250 4503 4127 )

#ifndef DEBUG
// 4702: unreachable code
#pragma warning( disable : 4702 ) // disable in release because MS headers have tons of unreachable code
#endif

