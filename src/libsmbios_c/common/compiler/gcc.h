#define UNREFERENCED_PARAMETER(P)  (void)(P)

#if defined(__MINGW32__)
#define __internal
#define __hidden
#else
#define __internal __attribute__((visibility("internal")))
#define __hidden __attribute__((visibility("hidden")))
#endif

#define _null_call( args...) do {} while(0)
#define _dbg_printf(format, args...) do { fprintf(stderr , format , ## args); fflush(NULL); } while(0)
// dbg_printf gets redefined to _dbg_printf on a source file by source file basis

#ifdef DEBUG_OUTPUT_ALL
#include <stdio.h>
#define dbg_printf _dbg_printf
#else
#define dbg_printf _null_call
#endif

#define fnprintf(fmt, args...)  do { dbg_printf("%s: ", __PRETTY_FUNCTION__); dbg_printf( fmt, ## args); } while(0)
