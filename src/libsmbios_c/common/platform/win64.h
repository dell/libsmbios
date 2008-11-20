// Enable 64-bit file access (changes off_t to 64-bit)
#ifndef FSEEK
#define FSEEK(fh, pos, whence) fseek(fh, (long)(pos), whence)
#endif

/* Windows is lame. */
#   define _snprintf    snprintf

