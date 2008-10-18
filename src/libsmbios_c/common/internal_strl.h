
#ifndef _LIBSMBIOS_C_INTERNAL_STRL_H
#define _LIBSMBIOS_C_INTERNAL_STRL_H

// define strlcpy and strlcat IFF platform doesnt have them

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_STRLCAT
size_t strlcat(char *dst, const char *src, size_t siz);
#endif

#ifndef HAVE_STRLCPY
size_t strlcpy(char *dst, const char *src, size_t siz);
#endif

#ifdef __cplusplus
}
#endif

#endif

