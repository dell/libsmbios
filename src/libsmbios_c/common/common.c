#define LIBSMBIOS_C_SOURCE
#define DEBUG_MODULE_NAME "DEBUG_CONSTRUCTOR_C"

#include "smbios_c/compat.h"

#include <string.h>

#include "common_internal.h"
#include "libsmbios_c_intlize.h"

// constructor to set up locale stuff
__attribute__((constructor)) static void lib_initialize (void)
{
    fnprintf("CONSTRUCTOR: pkg: %s, dir: %s\n", GETTEXT_PACKAGE,LIBSMBIOS_LOCALEDIR);
    bindtextdomain (GETTEXT_PACKAGE, LIBSMBIOS_LOCALEDIR);
    fnprintf( LIBSMBIOS_C_GETTEXT_DEBUG_STRING "\n");
    fnprintf( _("This message should be localized if setlocale() has been called and gettext compiled in.\n") );
}


#ifdef LIBSMBIOS_C_PLATFORM_LINUX
void fixed_strerror(int errval, char *errbuf, size_t bufsize)
{
    fnprintf("\n");
    size_t curstrsize = strlen(errbuf);

    if ((size_t)(bufsize - curstrsize - 1) < bufsize)
    {
        char *buf = strerror_r(errval, errbuf + curstrsize, bufsize - curstrsize - 1);
        // GNU breakage... :(
        if (buf != (errbuf + curstrsize))
            strlcat(errbuf, buf, bufsize);
    }
}
#endif

#ifdef LIBSMBIOS_C_PLATFORM_SOLARIS
void fixed_strerror(int errval, char *errbuf, size_t bufsize)
{
    fnprintf("\n");
    size_t curstrsize = strlen(errbuf);
}
#endif
