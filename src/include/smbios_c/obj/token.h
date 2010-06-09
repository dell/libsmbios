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


#ifndef C_OBJ_TOKEN_H
#define C_OBJ_TOKEN_H

// include smbios_c/compat.h first
#include "smbios_c/compat.h"
#include "smbios_c/types.h"

// abi_prefix should be last header included before declarations
#include "smbios_c/config/abi_prefix.h"

EXTERN_C_BEGIN;

#define TOKEN_DEFAULTS       0x0000
#define TOKEN_GET_SINGLETON  0x0001
#define TOKEN_GET_NEW        0x0002
#define TOKEN_UNIT_TEST_MODE 0x0004
#define TOKEN_NO_ERR_CLEAR   0x0008

struct token_table;
struct token_obj;

// construct
LIBSMBIOS_C_DLL_SPEC struct token_table *token_table_factory(int flags, ...);

// destruct
LIBSMBIOS_C_DLL_SPEC void token_table_free(struct token_table *);

// format error string
LIBSMBIOS_C_DLL_SPEC const char *token_table_strerror(const struct token_table *);
LIBSMBIOS_C_DLL_SPEC const char *token_obj_strerror(const struct token_obj *);

// for looping/searching
LIBSMBIOS_C_DLL_SPEC const struct token_obj *token_table_get_next(const struct token_table *, const struct token_obj *cur);
LIBSMBIOS_C_DLL_SPEC const struct token_obj *token_table_get_next_by_id(const struct token_table *, const struct token_obj *cur, u16 id);

LIBSMBIOS_C_DLL_SPEC u16 token_obj_get_id(const struct token_obj *);

#define token_table_for_each(table_name, struct_name)  \
        for(    \
            const struct token_obj *struct_name = token_table_get_next(table_name, 0);\
            struct_name;\
            struct_name = token_table_get_next(table_name, struct_name)\
           )

#define token_table_for_each_id(table_name, struct_name, id)  \
        for(    \
            const struct token_obj *struct_name = token_table_get_next_id(table_name, 0, id);\
            struct_name;\
            struct_name = token_table_get_next_id(table_name, struct_name, id)\
           )

LIBSMBIOS_C_DLL_SPEC int token_obj_get_type(const struct token_obj *);
LIBSMBIOS_C_DLL_SPEC bool token_obj_is_bool(const struct token_obj *);
LIBSMBIOS_C_DLL_SPEC int token_obj_is_active(const struct token_obj *);  // return 0,1 or negative error
LIBSMBIOS_C_DLL_SPEC int token_obj_activate(const struct token_obj *);   // return error

LIBSMBIOS_C_DLL_SPEC bool token_obj_is_string(const struct token_obj *);
LIBSMBIOS_C_DLL_SPEC char* token_obj_get_string(const struct token_obj *, size_t *len);  // return 0 on error
LIBSMBIOS_C_DLL_SPEC int token_obj_set_string(const struct token_obj *, const char *, size_t size);  // return error

LIBSMBIOS_C_DLL_SPEC const struct smbios_struct *token_obj_get_smbios_struct(const struct token_obj *);
LIBSMBIOS_C_DLL_SPEC int token_obj_try_password(const struct token_obj *, const char *pass_ascii, const char *pass_scancode);
LIBSMBIOS_C_DLL_SPEC const void *token_obj_get_ptr(const struct token_obj *t);

#if defined(_MSC_VER)
#pragma pack(push,1)
#endif
struct indexed_io_token
{
    u16 tokenId;
    u8  location;
    u8  andMask;
    union {
        u8 orValue;
        u8 stringLength;
    };
}
LIBSMBIOS_C_PACKED_ATTR;

struct indexed_io_access_structure
{ /* 0xD4 structure */
    u8	     type;
    u8	     length;
    u16	     handle;
    u16      indexPort;
    u16      dataPort;
    u8       checkType;
    u8       checkedRangeStartIndex;
    u8       checkedRangeEndIndex;
    u8       checkValueIndex;
    //variable number of tokens present, but at least one.
    struct   indexed_io_token  tokens[];
}
LIBSMBIOS_C_PACKED_ATTR;

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
#if defined(_MSC_VER)
#pragma pack(pop)
#endif

enum
{
    CHECK_TYPE_WORD_CHECKSUM   = 0x00, //simple running sum in word
    CHECK_TYPE_BYTE_CHECKSUM   = 0x01, //simple running sum in byte
    CHECK_TYPE_WORD_CRC        = 0x02, // crc
    CHECK_TYPE_WORD_CHECKSUM_N = 0x03, //simple runnign sum in word, then (~result + 1)
};

EXTERN_C_END;

// always should be last thing in header file
#include "smbios_c/config/abi_suffix.h"

#endif  /* TOKEN_H */
