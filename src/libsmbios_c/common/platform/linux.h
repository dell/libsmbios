// If we are on IA64 we will need to macro define inb_p and outb_p
#if defined(__ia64__)
#    define outb_p outb
#    define inb_p  inb
#endif

// required for compile on RHEL4. fseeko not defined unless we use this
#ifndef _LARGEFILE_SOURCE
#define _LARGEFILE_SOURCE
#endif

// Enable 64-bit file access
#ifndef FSEEK
#define FSEEK(fh, pos, whence) fseeko(fh, (off_t)(pos), whence)
#endif

