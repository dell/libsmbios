
#ifndef _LIBSMBIOS_C_INTERNAL_COMMON_H
#define _LIBSMBIOS_C_INTERNAL_COMMON_H

#include "internal_strl.h"

// avoid GNU braindamage... :(
void fixed_strerror(int errval, char *errbuf, size_t bufsize);

#endif

