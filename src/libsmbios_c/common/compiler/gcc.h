#define UNREFERENCED_PARAMETER(P)  (void)(P)

#if defined(__MINGW32__)
#define __internal
#define __hidden
#else
#define __internal __attribute__((visibility("internal")))
#define __hidden __attribute__((visibility("hidden")))
#endif

#ifndef DEBUG_MODULE_NAME
#  define DEBUG_MODULE_NAME "DEBUG_OUTPUT_ALL"
#endif

#define _env_dbg_printf(env, format, args...) \
    do { \
        char w[256] = "LIBSMBIOS_C_"; \
        strncat(w, env, 256 - 1 - strlen(w));   \
        const char *v = 0;  \
        const char *u = getenv("LIBSMBIOS_C_DEBUG_OUTPUT_ALL");    \
        if (env) v = getenv(w);    \
        if ( (u && atoi(u) > 0) || (v && atoi(v) > 0) ) {   \
        fprintf(stderr , format , ## args); \
        fflush(NULL); \
        }\
    } while(0)

#define _stderr_dbg_printf(format, args...) do { fprintf(stderr , format , ## args); fflush(NULL); } while(0)

#define _null_call( args...) do {} while(0)

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

#define fnprintf(fmt, args...)  do { dbg_printf("%s: ", __PRETTY_FUNCTION__); dbg_printf( fmt, ## args); } while(0)
