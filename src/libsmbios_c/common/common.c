#if HAVE_CONFIG_H
#include "config.h"
#endif

#include "libsmbios_c_intlize.h"

__attribute__((constructor)) static void lib_initialize (void)
{
    bindtextdomain (GETTEXT_PACKAGE, LIBSMBIOS_LOCALEDIR);
}

