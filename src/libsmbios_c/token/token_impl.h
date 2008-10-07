// vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:
/*
 * Copyright (C) 2005 Dell Inc.
 *  by Michael Brown <Michael_E_Brown@dell.com>
 * Licensed under the Open Software License version 2.1
 *
 * Alternatively, you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.

 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 */

#ifndef TOKEN_IMPL_H
#define TOKEN_IMPL_H

#include "smbios_c/compat.h"
#include "smbios_c/obj/token.h"
#include "smbios_c/types.h"

EXTERN_C_BEGIN;

#ifndef dprintf
#if defined(DEBUG_TOKEN_C)
#   include <stdio.h>
#   define dprintf(format, args...) do { fprintf(stderr , format , ## args);  } while(0)
#else
#   define dprintf(format, args...) do {} while(0)
#endif
#endif

enum  // Smbios Structure types
{
    DellIndexedIoTokenType = 0xD4,
    DellProtectedAreaType1 = 0xD5,
    DellProtectedAreaType2 = 0xD6,
    DellCallingInterface   = 0xDA,
};

enum // Token types
{
    TokenTypeEOT = 0xffff,
};

struct token_obj
{
    const char* (*get_type)(const struct token_obj*);
    int (*get_id)(const struct token_obj*);
    int (*is_bool)(const struct token_obj*);
    int (*is_string)(const struct token_obj*);

    int (*is_active)(const struct token_obj*);
    int (*activate)(const struct token_obj*);

    char* (*get_string)(const struct token_obj*);
    int (*get_string_len)(const struct token_obj*);
    int (*set_string)(const struct token_obj*, const char *, size_t size);

    int (*try_password)(const struct token_obj *, const char *);

    const struct smbios_struct *smbios_structure;
    void *token_ptr;
    struct token_obj *next;
};

struct token_table
{
    int initialized;
    struct smbios_table *smbios_table;
    struct token_obj *list_head;
};

void __internal add_token(struct token_table *t, struct token_obj *o);
void __internal add_d4_tokens(struct token_table *t);


#if defined(_MSC_VER)
#pragma pack(push,1)
#endif
struct dell_protected_value_1_structure
{  /* 0xD5 structure */
    u8	     type;
    u8	     length;
    u16	     handle;
    u16      tokenId;
    u8       valueLen;
    u8       valueFormat;
    u16      validationKey;
    u16      indexPort;
    u16      dataPort;
    u8       checkType;
    u8       valueStartIndex;
    u8       checkIndex;
}
LIBSMBIOS_C_PACKED_ATTR;

struct dell_protected_value_2_structure
{  /* 0xD6 structure */
    u8	     type;
    u8	     length;
    u16	     handle;
    u16      tokenId;
    u8       valueLen;
    u8       valueFormat;
    u16      validationKey;
    u16      indexPort;
    u16      dataPort;
    u8       checkType;
    u8       valueStartIndex;
    u8       checkIndex;
    u8       rangeCheckType;
    u8       rangeCheckStart;
    u8       rangeCheckEnd;
    u8       rangeCheckIndex;
}
LIBSMBIOS_C_PACKED_ATTR;

struct calling_interface_token
{
    u16 tokenId;
    u16 location;  // 0 for string tokens
    union {
        u16 value;
        u16 stringLength;
    };
}
LIBSMBIOS_C_PACKED_ATTR;

struct calling_interface_structure
{ /* 0xDA structure */
    u8	     type;
    u8	     length;
    u16	     handle;

    u16      cmdIOAddress;
    u8       cmdIOCode;
    u32      supportedCmds;
    struct   calling_interface_token  first_token[];
}
LIBSMBIOS_C_PACKED_ATTR;

enum  // protected value format types
{
    pvFmtAlphaNumericScanCode = 0,
    pvFmtAlphaNumericAscii    = 1,
    pvFmtAlphaNumericScanCodeNS = 2,
    pvFmtAlphaNumericAsciiNS   = 3,
};
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

struct cmos_access_obj; // forward declare so we dont have to include cmos.h

__internal u16 byteChecksum( u32 start, u32 end, u32 indexPort, u32 dataPort );
__internal u16 wordChecksum( u32 start, u32 end, u32 indexPort, u32 dataPort);
__internal u16 wordChecksum_n( u32 start, u32 end, u32 indexPort, u32 dataPort);
__internal u16 wordCrc( u32 start, u32 end, u32 indexPort, u32 dataPort );
__internal int update_checksum(const struct cmos_access_obj *c, bool do_update, void *userdata);

struct checksum_details
{
    u32 csumloc;
    u32 csumlen;
    u32 start;
    u32 end;
    u32 indexPort;
    u32 dataPort;
    u32 checkType;
    u16 (*csum_fn)( u32 start, u32 end, u32 indexPort, u32 dataPort );
};

EXTERN_C_END;

#endif /* TOKEN_IMPL_H */
