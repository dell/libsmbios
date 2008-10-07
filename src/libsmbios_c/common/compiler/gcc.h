#define UNREFERENCED_PARAMETER(P)  (void)(P)
#define __internal __attribute__((visibility("internal")))
#define __hidden __attribute__((visibility("hidden")))
#define _null_call( args...) do {} while(0)
#define _dbg_printf(format, args...) do { fprintf(stderr , format , ## args);  } while(0)
// dbg_printf gets redefined to _dbg_printf on a source file by source file basis
#define dbg_printf _null_call
#define fnprintf(fmt, args...)  dbg_printf( "%s: " fmt, __PRETTY_FUNCTION__, ## args)
