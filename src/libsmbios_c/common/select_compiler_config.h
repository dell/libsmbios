# if defined __GNUC__
//  GNU C++:
#   define LIBSMBIOS_C_INTERNAL_COMPILER_CONFIG "compiler/gcc.h"

#elif defined _MSC_VER
//  Microsoft Visual C++
//
//  Must remain the last #elif since some other vendors also #define _MSC_VER
#   define LIBSMBIOS_C_INTERNAL_COMPILER_CONFIG "compiler/visualc.h"

#elif defined(__SUNPRO_C) || defined(__SUNPRO_CC)
// Sun Studio Compiler
#define LIBSMBIOS_C_INTERNAL_COMPILER_CONFIG "compiler/sunpro_cc.h"

#else
// this must come last - generate an error if we don't
// recognise the compiler:
#  error "Unknown compiler - please report to libsmbios maintainer."

#endif
