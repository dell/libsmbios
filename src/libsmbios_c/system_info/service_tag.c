/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:cindent:textwidth=0:
 *
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

#define LIBSMBIOS_C_SOURCE
#include "smbios_c/compat.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>  // toupper

#include "smbios_c/smbios.h"
#include "smbios_c/obj/token.h"
#include "smbios_c/token.h"
#include "smbios_c/cmos.h"
#include "smbios_c/system_info.h"
#include "smbios_c/smi.h"
#include "dell_magic.h"
#include "sysinfo_impl.h"

/***********************************************
 * specialty functions to decode dell service tag
 *
 * note: funny naming for the following functions
 *       as they were copied from another project
 **********************************************/
static unsigned char dell_decode_digit( char tagval )
{
    // input == value from 0 - 0x1E
    // output == ascii
    // --> take value from range 0 - 0x1E and give ascii value
    if( tagval > 0x19 )
        tagval += 0x3C;   /* V-Z, 0x1A-0x1E */
    else if( tagval > 0x14 )
        tagval += 0x3B;   /* P-T, 0x15-0x19 */
    else if( tagval > 0x0F )
        tagval += 0x3A;   /* J-N, 0x10-0x14 */
    else if( tagval > 0x0C )
        tagval += 0x39;   /* F-H, 0x0D-0x0F */
    else if( tagval > 0x09 )
        tagval += 0x38;   /* B-D, 0x0A-0x0C */
    else
        tagval += 0x30;   /* 0-9, 0x00-0x09 */

    return tagval;
}

// decodes tag in-place
// see encoding function for nice ascii art representation.
static void dell_decode_service_tag( char *new_tag, char *tag, int len )
{
    memcpy(new_tag, tag, len);
    if( ((tag)[0] & (1<<7)) == (1<<7) )
    {
        new_tag[6] = dell_decode_digit( (tag[4] & 0x1F) );
        new_tag[5] = dell_decode_digit( ((tag[3] & 0x03)<<3) | ((tag[4]>>5) & 0x07) );
        new_tag[4] = dell_decode_digit( ((tag[3] & 0x7C)>>2) );
        new_tag[3] = dell_decode_digit( (((tag[2] & 0x0F)<<1) | ((tag[3]>>7) & 0x01)) );
        new_tag[2] = dell_decode_digit( (((tag[1] & 0x01)<<4) | ((tag[2]>>4) & 0xF)) & 0x1F);
        new_tag[1] = dell_decode_digit( ((tag[1] & 0x3E)>>1) & 0x1F );
        new_tag[0] = (tag[0] ^ (1<<7));
        memset(tag, 0, len);
    }
}

static unsigned char dell_encode_digit( char ch )
{
    // input == ascii
    // output == value from 0 - 0x1E
    // scale ascii value down to range 0-0x1E
    // valid input ascii == Alphanumeric - vowels
    // invalid input is converted to the char '0' (zero)
    int uc = toupper(ch);
    unsigned char retval = 0;
    if ( uc >= '0' && uc <= '9' )
        retval = uc - 0x30;
    if ( uc >= 'B' && uc <= 'D' )
        retval = uc - 0x38;
    if ( uc >= 'F' && uc <= 'H' )
        retval = uc - 0x39;
    if ( uc >= 'J' && uc <= 'N' )
        retval = uc - 0x3A;
    if ( uc >= 'P' && uc <= 'T' )
        retval = uc - 0x3B;
    if ( uc >= 'V' && uc <= 'Z' )
        retval = uc - 0x3C;
    return retval;
}

static void dell_encode_service_tag( char *tag, size_t len )
{
    char tagToSet[SVC_TAG_LEN_MAX] = {0,};
    char newTagBuf[SVC_TAG_CMOS_LEN_MAX] = {0,};

    if (len <= SVC_TAG_CMOS_LEN_MAX)
        return;

    // codes a 7-char value into 5 bytes
    //
    //    byte       byte        byte        byte         byte
    //     0           1           2           3           4
    //|----|----| |----|----| |----|----| |----|----| |----|----|
    // 1  0 0000     11 1112   2222 3333   3444 4455   5556 6666
    //     char0     char1  char2    char3  char4  char5    char6
    //
    // note: high bit set in byte0 to indicate coded tag.

    memcpy(tagToSet, tag, len < SVC_TAG_LEN_MAX ? len : SVC_TAG_LEN_MAX );

    // char 0
    newTagBuf[0] = tagToSet[0] | 1<<7;

    // char 1
    newTagBuf[1] = dell_encode_digit(tagToSet[1]) << 1;

    // char 2
    newTagBuf[1] = newTagBuf[1] | dell_encode_digit(tagToSet[2]) >> 4;
    newTagBuf[2] = dell_encode_digit(tagToSet[2]) << 4;

    // char 3
    newTagBuf[2] = newTagBuf[2] | dell_encode_digit(tagToSet[3]) >> 1;
    newTagBuf[3] = dell_encode_digit(tagToSet[3]) << 7;

    // char 4
    newTagBuf[3] = newTagBuf[3] | dell_encode_digit(tagToSet[4]) << 2;

    // char 5
    newTagBuf[3] = newTagBuf[3] | dell_encode_digit(tagToSet[5]) >> 3;
    newTagBuf[4] = dell_encode_digit(tagToSet[5]) << 5;

    // char 6
    newTagBuf[4] = newTagBuf[4] | dell_encode_digit(tagToSet[6]);

    memset(tag, 0, len);
    memcpy(tag, newTagBuf, len < SVC_TAG_CMOS_LEN_MAX ? len: SVC_TAG_CMOS_LEN_MAX);
    return;
}


static char *getServiceTagFromCMOSToken()
{
    const struct smbios_struct *s;
    char *tempval = 0;
    char *tag = 0;
    u16 indexPort, dataPort;
    u8  location;
    u8 csum = 0;
    u8 byte;
    int ret;

    fnprintf("\n");

    // Step 1: Get tag from CMOS
    fnprintf("- get string\n");
    size_t len = 0;
    tempval = token_get_string(Cmos_Service_Token, &len);  // allocates mem
    if (!tempval)
        goto out_err;

    //fnprintf("- current string: '%s', len: %zd, strlen %zd, tagsize %d", tempval, len, strlen(tempval), SVC_TAG_CMOS_LEN_MAX);

    // if we got a value, we have to allocate a larger buffer to hold the result
    tag = calloc(1, SVC_TAG_LEN_MAX + 1);

    // Step 2: Decode 7-char tag from 5-char CMOS value
    fnprintf("- decode string\n");
    dell_decode_service_tag(tag, tempval, len);
    free(tempval);
    tempval = 0;
    fnprintf("- GOT: '%s'\n", tag);

    // Step 3: Make sure checksum is good before returning value
    fnprintf("- csum: ");
    s = token_get_smbios_struct(Cmos_Service_Token);
    indexPort = ((struct indexed_io_access_structure*)s)->indexPort;
    dataPort = ((struct indexed_io_access_structure*)s)->dataPort;
    location = ((struct indexed_io_token *)token_get_ptr(Cmos_Service_Token))->location;

    // calc checksum
    for( u32 i = 0; i < SVC_TAG_CMOS_LEN_MAX; i++)
    {
        ret = cmos_read_byte(&byte, indexPort, dataPort, location + i);
        if (ret<0)
            goto out_err;

        csum += (u8)byte;
        dbg_printf(" 0x%02x|0x%02x", (unsigned char)byte, (unsigned char)csum);
    }
    csum = ~csum + 1;
    dbg_printf("\n");

    // get checksum byte
    ret = cmos_read_byte(&byte, indexPort, dataPort, location + SVC_TAG_CMOS_LEN_MAX);
    if (ret<0)
        goto out_err;

    fnprintf("- read from cmos: %x  calculated: %x\n", byte, csum);
    if (csum != byte) // bad (should be zero)
        goto out_err;

    fnprintf("GOT CMOS TAG: %s\n", tag);
    goto out;

out_err:
    fnprintf("- out_err\n");
    free(tag);
    tag = 0;

out:
    fnprintf("- out\n");
    return tag;
}

static char *getServiceTagFromSysInfo()
{
    fnprintf("\n");
    return smbios_struct_get_string_from_table(System_Information_Structure, System_Information_Serial_Number_Offset);
}

static char *getServiceTagFromSysEncl()
{
    fnprintf("\n");
    return smbios_struct_get_string_from_table(System_Enclosure_or_Chassis_Structure, System_Enclosure_or_Chassis_Service_Offset);
}


/* only for service/asset tags. */
__hidden char *getTagFromSMI(u16 select)
{
    u32 args[4] = {0,0,0,0}, res[4] = {0,0,0,0};
    char *retval = 0;
    int ret = 0;
    fnprintf("\n");
    ret = dell_simple_ci_smi(11, select, args, res);

    fnprintf("res[0] = %d\n", (unsigned int)res[0]);
    if (ret || res[0])
        goto out;

    retval = calloc(1, MAX_SMI_TAG_SIZE + 1); // smi function can hold at most 12 bytes, add one for '\0'
    memcpy(retval, (u8 *)(&(res[1])), MAX_SMI_TAG_SIZE);

    fnprintf("raw = ");
    for(int i=0; i<MAX_SMI_TAG_SIZE; i++)
        dbg_printf("0x%02x ", retval[i]);
    dbg_printf("\n");

    for(size_t i=0; i<MAX_SMI_TAG_SIZE; ++i)
        if ((unsigned char)(retval[i])==(unsigned char)0xFF)
            retval[i] = '\0';

    fnprintf("cooked = ");
    for(int i=0; i<MAX_SMI_TAG_SIZE; i++)
        dbg_printf("0x%02x ", retval[i]);
    dbg_printf("\n");

out:
    return retval;
}


static char *getServiceTagFromSMI()
{
    fnprintf("\n");
    return getTagFromSMI( 2 ); /* Read service tag select code */
}



// Code for getting the service tag from one of many locations
/* try dynamic functions first to make sure we get current data. */
static struct DellGetServiceTagFunctions
{
    char *(*f_ptr)();
} DellGetServiceTagFunctions[] = {
                                   {&getServiceTagFromSysInfo,},   // SMBIOS System Information Item
                                   {&getServiceTagFromSysEncl,},   // SMBIOS System Enclosure Item
                                   {&getServiceTagFromSMI,},       // SMI Token
                                   {&getServiceTagFromCMOSToken,}, // CMOS Token
                               };

LIBSMBIOS_C_DLL_SPEC char *sysinfo_get_service_tag()
{
    char *serviceTag = 0;
    int numEntries =
        sizeof (DellGetServiceTagFunctions) / sizeof (DellGetServiceTagFunctions[0]);

    sysinfo_clearerr();
    fnprintf("\n");
    for (int i = 0; (i < numEntries) && (!serviceTag); ++i)
    {
        fnprintf("Call fn pointer %p\n", DellGetServiceTagFunctions[i].f_ptr);
        // first function to return non-zero id with strlen()>0 wins.
        serviceTag = DellGetServiceTagFunctions[i].f_ptr ();
        fnprintf("got result: %p\n", serviceTag);
        if (serviceTag)
        {
            strip_trailing_whitespace(serviceTag);
            if (!strlen(serviceTag))
            {
                fnprintf("string is zero len, discarding\n");
                free(serviceTag);
                serviceTag=0;
            }
        }
    }
    return serviceTag;
}



/* only for service/asset tags. */
// negative error, 0 on success
__hidden u32 setTagUsingSMI(u16 select, const char *newTag, u16 security_key)
{
    u32 args[4] = {0,}, res[4] = {0,};
    int ret = 0;
    strncpy((char *)args, newTag, MAX_SMI_TAG_SIZE);
    args[3] = security_key;
    ret = dell_simple_ci_smi(11, select, args, res);
    if (ret || res[0])
        return -1;
    else
        return 0;
}

int setServiceTagUsingCMOSToken(const char *newTag, const char *pass_ascii, const char *pass_scancode)
{
    const struct smbios_struct *s;
    u16 indexPort, dataPort;
    u8  location, csum = 0, byte;
    int retval = -1, ret;
    char codedTag[SVC_TAG_LEN_MAX + 1] = {0,}; // null padded

    UNREFERENCED_PARAMETER(pass_ascii);
    UNREFERENCED_PARAMETER(pass_scancode);

    // don't want to modify user-supplied buffer, so copy new tag
    // to our own buffer.
    // copy (possibly oversize) user input to our buffer.
    strncpy(codedTag, newTag, SVC_TAG_LEN_MAX);

    // CMOS has 5 bytes of space, but up to 7 chars can be coded into those 5 bytes if needed
    if(strnlen(codedTag, SVC_TAG_LEN_MAX) > SVC_TAG_CMOS_LEN_MAX )
        dell_encode_service_tag(codedTag, SVC_TAG_LEN_MAX);

    // Step 1: set string: safe to use whole codedTag as it is guaranteed zero-padded
    fnprintf("- set string: ");
    for(int i=0; i<SVC_TAG_LEN_MAX; i++)
        dbg_printf("0x%02x ", (unsigned char)codedTag[i]);
    dbg_printf("\n");

    if(isalpha(codedTag[0]))
        fnprintf("- is alpha string: %s\n", codedTag);

    ret = token_set_string(Cmos_Service_Token, codedTag, SVC_TAG_CMOS_LEN_MAX);
    if (ret)
        goto out;

    // Step 2: reset checksum
    fnprintf("- csum ");
    s = token_get_smbios_struct(Cmos_Service_Token);
    indexPort = ((struct indexed_io_access_structure*)s)->indexPort;
    dataPort = ((struct indexed_io_access_structure*)s)->dataPort;
    location = ((struct indexed_io_token *)token_get_ptr(Cmos_Service_Token))->location;

    // calc checksum
    for( u32 i = 0; i < SVC_TAG_CMOS_LEN_MAX; i++)
    {
        ret = cmos_read_byte(&byte, indexPort, dataPort, location + i);
        if (ret)
            goto out;

        csum += (u8)byte;
        dbg_printf(" 0x%02x|0x%02x", (unsigned char)byte, (unsigned char)csum);
    }
    csum = ~csum+1;
    dbg_printf("\n");
    fnprintf(" csum = 0x%02x\n",(unsigned char)csum);

    //debug only
    ret = cmos_read_byte(&byte, indexPort, dataPort, location + SVC_TAG_CMOS_LEN_MAX);
    fnprintf(" read current csum = 0x%02x\n",(unsigned int)byte);

    // write checksum byte
    ret = cmos_write_byte(csum, indexPort, dataPort, location + SVC_TAG_CMOS_LEN_MAX);
    if (ret<0)
        goto out;

    //debug only
    ret = cmos_read_byte(&byte, indexPort, dataPort, location + SVC_TAG_CMOS_LEN_MAX);
    fnprintf(" reread current csum = 0x%02x\n",(unsigned int)byte);

    retval = 0;

out:
    fnprintf("- out\n");
    return retval;
}


// Important note from the docs:
/*  Only the manufacturing software that’s loading the service tag into the system should use this interface.
    Some systems may return an error when the service tag has already been set (i.e. they prevent this function from changing the service tag once it has been set).
    */
int setServiceTagUsingSMI(const char *newTag, const char *pass_ascii, const char *pass_scancode)
{
    int retval = 0;
    u16 security_key = 0;
    const char *whichpw = pass_scancode;
    if (dell_smi_password_format(DELL_SMI_PASSWORD_ADMIN) == DELL_SMI_PASSWORD_FMT_ASCII)
        whichpw=pass_ascii;
    retval = -2;
    if(dell_smi_get_security_key(whichpw, &security_key))
        goto out;

    retval = -1;
    if (setTagUsingSMI( 3, newTag, security_key)) /* Write service tag select code */
        goto out;

    retval = 0;

out:
    return retval;
}

// Code for getting the service tag from one of many locations
struct DellSetServiceTagFunctions
{
    const char *name;
    int (*f_ptr)(const char *, const char *, const char *);
}

DellSetServiceTagFunctions[] = {
                                   {"setServiceTagUsingSMI", &setServiceTagUsingSMI,},   // SMBIOS System Information Item
                                   {"setServiceTagUsingCMOSToken", &setServiceTagUsingCMOSToken,},   // SMBIOS System Information Item
                               };

LIBSMBIOS_C_DLL_SPEC int sysinfo_set_service_tag(const char *serviceTag, const char *pass_ascii, const char *pass_scancode)
{
    int ret = -1;
    int numEntries =
        sizeof (DellSetServiceTagFunctions) / sizeof (DellSetServiceTagFunctions[0]);

    sysinfo_clearerr();
    fnprintf("\n");
    for (int i = 0; (i < numEntries) && (ret != 0); ++i)
    {
        fnprintf("Call fn pointer to %s\n", DellSetServiceTagFunctions[i].name);
        // first function to return non-zero id with strlen()>0 wins.
        ret = DellSetServiceTagFunctions[i].f_ptr (serviceTag, pass_ascii, pass_scancode);
    }
    return ret;
}
