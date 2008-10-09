#define UNREFERENCED_PARAMETER(P)  (P)

#define strtoll(p, e, b) _strtoi64(p, e, b)

#define __internal
#define __hidden

// variadic macros added in vc 8.0: http://msdn.microsoft.com/en-us/library/ms177415(VS.80).aspx
#define _null_call(...) do {} while(0)
#define _dbg_printf(format, ...) do { fprintf(stderr , format , __VA_ARGS__);  } while(0)
// dbg_printf gets redefined to _dbg_printf on a source file by source file basis

#ifdef DEBUG_OUTPUT_ALL
#include <stdio.h>
#define dbg_printf _dbg_printf
#else
#define dbg_printf _null_call
#endif

#define fnprintf(fmt, ...)  dbg_printf( "%s: " fmt, __FUNCTION__, __VA_ARGS__)



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

