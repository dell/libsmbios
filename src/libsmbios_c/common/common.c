#define LIBSMBIOS_C_SOURCE

#include "smbios_c/compat.h"

#include <string.h>

#include "common_internal.h"
#include "libsmbios_c_intlize.h"

#if defined(DEBUG_CONSTRUCTOR_C)
#   include <stdio.h>
#   undef dbg_printf
#   define dbg_printf _dbg_printf
#endif

// constructor to set up locale stuff
__attribute__((constructor)) static void lib_initialize (void)
{
    fnprintf("CONSTRUCTOR: pkg: %s, dir: %s\n", GETTEXT_PACKAGE,LIBSMBIOS_LOCALEDIR);
    bindtextdomain (GETTEXT_PACKAGE, LIBSMBIOS_LOCALEDIR);
    fnprintf( LIBSMBIOS_C_GETTEXT_DEBUG_STRING "\n");
    fnprintf( _("This message should be localized if setlocale() has been called and gettext compiled in.\n") );
}



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

