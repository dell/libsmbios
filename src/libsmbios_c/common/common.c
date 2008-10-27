#if HAVE_CONFIG_H
#include "config.h"
#endif

#define LIBSMBIOS_C_SOURCE
#include "libsmbios_c_intlize.h"

#if defined(DEBUG_CONSTRUCTOR_C)
#   include <stdio.h>
#   undef dbg_printf
#   define dbg_printf _dbg_printf
#endif

__attribute__((constructor)) static void lib_initialize (void)
{
    fnprintf("CONSTRUCTOR: pkg: %s, dir: %s\n", GETTEXT_PACKAGE,LIBSMBIOS_LOCALEDIR);
    bindtextdomain (GETTEXT_PACKAGE, LIBSMBIOS_LOCALEDIR);
    fnprintf( LIBSMBIOS_C_GETTEXT_DEBUG_STRING "\n");
    fnprintf( _("This message should be localized if setlocale() has been called and gettext compiled in.\n") );
}

