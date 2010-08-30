#if defined(linux) || defined(__linux) || defined(__linux__)
// linux:
#  define LIBSMBIOS_C_INTERNAL_PLATFORM_CONFIG "platform/linux.h"

#elif defined(_WIN64) || defined(__WIN64__) || defined(WIN64)
// win64:
#  define LIBSMBIOS_C_INTERNAL_PLATFORM_CONFIG "platform/win64.h"

#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
// win32:
#  define LIBSMBIOS_C_INTERNAL_PLATFORM_CONFIG "platform/win32.h"
#elif defined(sun)
// solaris:
#  define LIBSMBIOS_C_INTERNAL_PLATFORM_CONFIG "platform/solaris.h"
#else

// this must come last - generate an error if we don't
// recognise the platform:
#     error "Unknown platform - please report to libsmbios maintainer."

#endif
