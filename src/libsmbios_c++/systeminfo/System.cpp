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

#if defined(DEBUG_SYSINFO)
#define DEBUG_OUTPUT_ALL
#endif

#define LIBSMBIOS_SOURCE
#include "smbios/compat.h"

#include <string.h>

#include "smbios/ISmbios.h"
#include "smbios/IToken.h"
#include "smbios/ISmi.h"

#include "smbios/SystemInfo.h"
#include "smbios/IMemory.h"
#include "smbios/SmbiosDefs.h"
#include "ExceptionImpl.h"
#include "TokenLowLevel.h"

#include "DellMagic.h"

// this always should be included last (except config.h)
#include "smbios/message.h"

#if HAVE_CONFIG_H
#ifdef sun
#undef _FILE_OFFSET_BITS
#endif
#include "config.h"
#endif

using namespace smbios;
using namespace cmos;
using namespace std;

extern smbios::Exception<smbios::IException> SysInfoException;

// so we don't have to change API, add new tryBiosPassword() function
// that sets this password. This password will silently be used by
// functions that need a password
static std::string biosPassword = "";

static void stripString( char *str )
{
    if(!str)
        return;

    if(strlen(str) == 0)
        return;

    size_t ch = strlen(str); 
    do
    {
        --ch;
        if( ' ' == str[ch] )
            str[ch] = '\0';
        else
            break;

    } while(ch);
}

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
static void dell_decode_service_tag( char *tag, int len )
{
    // see encoding function for nice ascii art representation.
    //
    if( ((tag)[0] & (1<<7)) == (1<<7) )
    {
        char new_tag[SVC_TAG_LEN_MAX + 1] = {0,};

        // yuck.
        new_tag[6] = dell_decode_digit( (tag[4] & 0x1F) );
        new_tag[5] = dell_decode_digit( ((tag[3] & 0x03)<<3) | ((tag[4]>>5) & 0x07) );
        new_tag[4] = dell_decode_digit( ((tag[3] & 0x7C)>>2) );
        new_tag[3] = dell_decode_digit( (((tag[2] & 0x0F)<<1) | ((tag[3]>>7) & 0x01)) );
        new_tag[2] = dell_decode_digit( (((tag[1] & 0x01)<<4) | ((tag[2]>>4) & 0xF)) & 0x1F);
        new_tag[1] = dell_decode_digit( ((tag[1] & 0x3E)>>1) & 0x1F );
        new_tag[0] = (tag[0] ^ (1<<7));

        memset(tag, 0, len);
        strncpy(tag, new_tag, len < SVC_TAG_LEN_MAX ? len : SVC_TAG_LEN_MAX);
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
    int retval = 0;
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
    return static_cast<unsigned char>(retval);
}

static void dell_encode_service_tag( char *tag, size_t len )
{
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

    char tagToSet[SVC_TAG_LEN_MAX] = {0,};
    memcpy(tagToSet, tag, len < SVC_TAG_LEN_MAX ? len : SVC_TAG_LEN_MAX );

    char newTagBuf[SVC_TAG_CMOS_LEN_MAX] = {0,};

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


const char *SMBIOSGetLibraryVersionString()
{
    return PACKAGE_VERSION;
}

void SMBIOSFreeMemory( const char *ptr )
{
    delete [] const_cast<char *>(ptr);
}

/* only for service/asset tags. */
static char *getTagFromSMI(u16 select)
{
    u32 args[4] = {0,}, res[4] = {0,};
    smi::doSimpleCallingInterfaceSmi(11, select, args, res);

    char *retval = new char[16];
    memset(retval, '\0', 16);

    memcpy(retval, reinterpret_cast<u8 *>(&(res[1])), sizeof(res));

    for(size_t i=0; i<strlen(retval); i++)
        if( static_cast<unsigned char>(retval[i]) == 0xFF ) retval[i] = '\0';
    
    return retval;
}

/* only for service/asset tags. */
static void setTagUsingSMI(const char *newTag, u16 select)
{
    u32 args[4] = {0,}, res[4] = {0,};
    strncpy(reinterpret_cast<char *>(args), newTag, 12);
    args[3] = smi::getAuthenticationKey(biosPassword);
    smi::doSimpleCallingInterfaceSmi(11, select, args, res);
}

static char *getStringFromTable(unsigned int structure, unsigned int stringNumber)
{
    const smbios::ISmbiosTable *table = 0;
    table = smbios::SmbiosFactory::getFactory()->getSingleton();

    if (!table)
        throw InternalErrorImpl();

    const char *tempval = 0;
    tempval = getString_FromItem(*(*table)[structure], stringNumber);

    if(!tempval)
        throw exception();

    size_t slen = strlen(tempval);
    char *retval = new char[slen + 1];
    strncpy(retval,tempval,slen);
    retval[slen] = '\0';

    stripString(retval);
    if ( ! strlen(retval ))
    {
        delete [] retval;
        retval = 0;
        throw exception(); // skip this one because returned string was all spaces. 
    }

    return retval;
}

static char *getServiceTagFromSysInfo()
{
    DCOUT( "in getServiceTagFromSysInfo()" << endl);
    return getStringFromTable(System_Information, System_Information_Serial_Number_Offset);
}

static char *getServiceTagFromSysEncl()
{
    DCOUT( "in getServiceTagFromSysEncl()" << endl);
    return getStringFromTable(System_Enclosure_or_Chassis, System_Enclosure_or_Chassis_Service_Offset);
}

// not static so that unit tests can peek here. Not part of public API, though.
char *getServiceTagFromCMOSToken()
{
    smbios::ITokenTable *table = 0;
    table = smbios::TokenTableFactory::getFactory()->getSingleton();

    DCOUT( "in getServiceTagFromCMOSToken()" << endl);

    if (0 == table)
    {
        throw InternalErrorImpl();
    }

    char *tempval = 0;
    try
    {
        // Step 1: Get tag from CMOS
        tempval = new char[SVC_TAG_LEN_MAX + 1];
        memset(tempval, '\0', SVC_TAG_LEN_MAX + 1);
        // will throw an exception if not found.
        (*table)[Cmos_Service_Token]->getString(reinterpret_cast<u8*>(tempval), SVC_TAG_CMOS_LEN_MAX + 1);

        // Step 2: Decode 7-char tag from 5-char CMOS value
        dell_decode_service_tag( tempval, SVC_TAG_LEN_MAX + 1 );

        // Step 3: Make sure checksum is good before returning value
        u16 indexPort, dataPort;
        u8  location;

        smbios::IToken *token = &(*((*table)[ Cmos_Service_Token ]));
        dynamic_cast< smbios::ICmosToken * >(token)->getCMOSDetails( &indexPort, &dataPort, &location );

        u8 csum = 0;
        ICmosRW *cmos = cmos::CmosRWFactory::getFactory()->getSingleton();

        for( u32 i = 0; i < SVC_TAG_CMOS_LEN_MAX; i++)
        {
            // stupid stuff to avoid MVC++ .NET runtime exception check for cast to different size
            csum = (csum + cmos->readByte( indexPort, dataPort, location + i )) & 0xFF;
        }

        // stupid stuff to avoid MVC++ .NET runtime exception check for cast to different size
        csum = (csum - cmos->readByte( indexPort, dataPort, location + SVC_TAG_CMOS_LEN_MAX )) & 0xFF;
        if( csum ) // bad (should be zero)
            throw "Bad checksum";
    }
    catch( ... )
    {
        delete [] tempval;
        throw;
    }

    return tempval;
}

// not static so that unit tests can peek here. Not part of public API, though.
char *getServiceTagFromSMI()
{
    DCOUT( "in getServiceTagFromSMI()" << endl);
    return getTagFromSMI( 2 ); /* Read service tag select code */
}

// Code for getting the service tag from one of many locations
struct DellGetServiceTagFunctions
{
    char *(*f_ptr)();
}

/* try dynamic functions first to make sure we get current data. */
DellGetServiceTagFunctions[] = {
                                   {&getServiceTagFromSysInfo,},   // SMBIOS System Information Item
                                   {&getServiceTagFromSysEncl,},   // SMBIOS System Enclosure Item
                                   {&getServiceTagFromSMI,},       // SMI Token
                                   {&getServiceTagFromCMOSToken,}, // CMOS Token
                               };

const char *SMBIOSGetServiceTag()
{
    char *serviceTag = 0;
    int numEntries =
        sizeof (DellGetServiceTagFunctions) / sizeof (DellGetServiceTagFunctions[0]);

    DCOUT( "numEntries: " << numEntries << endl);

    for (int i = 0; (i < numEntries) && (!serviceTag); ++i)
    {
        // eat exceptions from lowlevel functions and keep going.
        try
        {
            DCOUT("  try #" << i << endl);
            // first function to return non-zero id wins.
            serviceTag = DellGetServiceTagFunctions[i].f_ptr ();
        }
        catch(const exception &e)
        {
            DCOUT("  Caught exception: " << e.what() << endl);
            SysInfoException.setMessageString(e.what());
        }
        catch(...)
        {
            DCOUT("  Caught unknown exception" << endl);
            SysInfoException.setMessageString( _("Unknown internal error occurred") );
        }

        if(serviceTag)
            DCOUT( "    GOT TAG: -->" << serviceTag << "<--" << endl);
    }
    stripString(serviceTag);
    return serviceTag;
}

void setServiceTagUsingCMOSToken(const char *newTag, size_t len)
{
    smbios::ITokenTable *table = 0;
    table = smbios::TokenTableFactory::getFactory()->getSingleton();

    if (0 == table)
    {
        throw InternalErrorImpl();
    }

    try
    {
        // don't want to modify user-supplied buffer, so copy new tag
        // to our own buffer.
        char codedTag[SVC_TAG_LEN_MAX + 1] = {0,}; // null padded
        // copy (possibly oversize) user input to our buffer.
        strncpy(codedTag, newTag, len < SVC_TAG_LEN_MAX ? len : SVC_TAG_LEN_MAX);
        // encode in place, if necessary
        dell_encode_service_tag(codedTag, len);
        // will throw an exception if not found.

        // Step 1: set string: safe to use whole codedTag as it is guaranteed zero-padded
        (*table)[Cmos_Service_Token]->setString(reinterpret_cast<const u8*>(codedTag), SVC_TAG_CMOS_LEN_MAX);

        // Step 2: reset checksum
        u16 indexPort, dataPort;
        u8  location;

        smbios::IToken *token = &(*((*table)[ Cmos_Service_Token ]));
        dynamic_cast< smbios::ICmosToken * >(token)->getCMOSDetails( &indexPort, &dataPort, &location );

        u8 csum = 0;
        ICmosRW *cmos = cmos::CmosRWFactory::getFactory()->getSingleton();

        for( u32 i = 0; i < SVC_TAG_CMOS_LEN_MAX; i++)
        {
            // stupid stuff to avoid MVC++ .NET runtime exception check for cast to different size
            csum = (csum + cmos->readByte( indexPort, dataPort, location + i )) & 0xFF;
        }

        cmos->writeByte(
            indexPort,
            dataPort,
            location + SVC_TAG_CMOS_LEN_MAX,
            csum
        );
    }
    catch( const smbios::IException & )
    {
        throw;
    }

}

// Important note from the docs:
/*  Only the manufacturing software that’s loading the service tag into the system should use this interface.
    Some systems may return an error when the service tag has already been set (i.e. they prevent this function from changing the service tag once it has been set).
    */
void setServiceTagUsingSMI(const char *newTag, size_t size)
{
    (void) size; // avoid unused var warning.
    setTagUsingSMI( newTag, 3 ); /* Write service tag select code */
}

// Code for getting the service tag from one of many locations
struct DellSetServiceTagFunctions
{
    void (*f_ptr)(const char *, size_t);
}

DellSetServiceTagFunctions[] = {
                                   {&setServiceTagUsingSMI,},   // SMBIOS System Information Item
                                   {&setServiceTagUsingCMOSToken,},   // SMBIOS System Information Item
                               };

int SMBIOSSetServiceTag(const char *password, const char *serviceTag, size_t len)
{
    int retval = -1;
    int numEntries =
        sizeof (DellSetServiceTagFunctions) / sizeof (DellSetServiceTagFunctions[0]);

    if(password)
        biosPassword = password;

    for (int i = 0; (i < numEntries); ++i)
    {
        // eat exceptions from lowlevel functions and keep going.
        try
        {
            // first function to return non-zero id wins.
            DellSetServiceTagFunctions[i].f_ptr (serviceTag, len);
            retval = 0;
        }
        catch(const smbios::IException &e)
        {
            SysInfoException.setMessageString(e.what());
        }
        catch(...)
        {
            SysInfoException.setMessageString( _("Unknown internal error occurred") );
        }
    }
    return retval;
}

static char *getAssetTagFromSysEncl()
{
    return getStringFromTable(System_Enclosure_or_Chassis, System_Enclosure_or_Chassis_Asset_Offset);
}

// not static so we can use it in unit test, but not part of public API.
// you have been warned.
char *getAssetTagFromToken()
{
    smbios::ITokenTable *table = 0;
    table = smbios::TokenTableFactory::getFactory()->getSingleton();

    if (0 == table)
    {
        throw InternalErrorImpl();
    }

    u8 *tempval = 0;
    try
    {
        tempval = new u8[ASSET_TAG_LEN_MAX + 1];
        memset(tempval, '\0', ASSET_TAG_LEN_MAX + 1);
        (*table)[Cmos_Asset_Token]->getString(tempval, ASSET_TAG_LEN_MAX + 1);

        // Step 3: Make sure checksum is good before returning value
        u16 indexPort, dataPort;
        u8  location;

        smbios::IToken *token = &(*((*table)[ Cmos_Asset_Token ]));
        dynamic_cast< smbios::ICmosToken * >(token)->getCMOSDetails( &indexPort, &dataPort, &location );

        u8 csum = 0;
        ICmosRW *cmos = cmos::CmosRWFactory::getFactory()->getSingleton();

        for( u32 i = 0; i < ASSET_TAG_CMOS_LEN_MAX; i++)
        {
            // stupid stuff to avoid MVC++ .NET runtime exception check for cast to different size
            csum = (csum + cmos->readByte( indexPort, dataPort, location + i )) & 0xFF;
        }

        // stupid stuff to avoid MVC++ .NET runtime exception check for cast to different size
        csum = (csum - cmos->readByte( indexPort, dataPort, location + ASSET_TAG_CMOS_LEN_MAX )) & 0xFF;
        if( csum ) // bad (should be zero)
            throw "Bad checksum";
    }
    catch (...)
    {
        delete [] tempval;
        throw;
    }

    return reinterpret_cast<char*>(tempval);
}

char *getAssetTagFromSMI()
{
    return getTagFromSMI( 0 ); /* Read asset tag select code */
}

// Code for getting the asset tag from one of many locations
struct DellAssetTagFunctions
{
    char *(*f_ptr)();
}

/* try dynamic functions first to make sure we get current data. */
DellAssetTagFunctions[] = {
                              {&getAssetTagFromSysEncl,}, // SMBIOS System Information Item
                              {&getAssetTagFromToken,},   // SMBIOS CMOS Token
                              {&getAssetTagFromSMI,},     // SMI
                          };

const char *SMBIOSGetAssetTag()
{
    char *assetTag = 0;
    int numEntries =
        sizeof (DellAssetTagFunctions) / sizeof (DellAssetTagFunctions[0]);

    for (int i = 0; (i < numEntries) && (!assetTag); ++i)
    {
        // eat exceptions from lowlevel functions and keep going.
        try
        {
            // first function to return non-zero id wins.
            assetTag = DellAssetTagFunctions[i].f_ptr ();
        }
        catch(const smbios::IException &e)
        {
            SysInfoException.setMessageString(e.what());
        }
        catch(...)
        {
            SysInfoException.setMessageString( _("Unknown internal error occurred") );
        }
    }
    stripString(assetTag);
    return assetTag;
}



void setAssetTagUsingCMOSToken(const char *newTag, size_t len)
{
    smbios::ITokenTable *table = 0;
    table = smbios::TokenTableFactory::getFactory()->getSingleton();

    if (0 == table)
    {
        throw InternalErrorImpl();
    }

    try
    {
        // Step 1: set string
        (*table)[Cmos_Asset_Token]->setString(reinterpret_cast<const u8*>(newTag), len < ASSET_TAG_CMOS_LEN_MAX? len : ASSET_TAG_CMOS_LEN_MAX);

        // Step 2: reset checksum
        u16 indexPort, dataPort;
        u8  location;

        smbios::IToken *token = &(*((*table)[ Cmos_Asset_Token ]));
        dynamic_cast< smbios::ICmosToken * >(token)->getCMOSDetails( &indexPort, &dataPort, &location );

        u8 csum = 0;
        ICmosRW *cmos = cmos::CmosRWFactory::getFactory()->getSingleton();

        for( u32 i = 0; i < ASSET_TAG_CMOS_LEN_MAX; i++)
        {
            // stupid stuff to avoid MVC++ .NET runtime exception check for cast to different size
            csum = (csum + cmos->readByte( indexPort, dataPort, location + i )) & 0xFF;
        }

        cmos->writeByte(
            indexPort,
            dataPort,
            location + ASSET_TAG_CMOS_LEN_MAX,
            csum
        );
    }
    catch( const smbios::IException & )
    {
        throw;
    }

}

void setAssetTagUsingSMI(const char *newTag, size_t size)
{
    (void) size; // avoid unused var warning.
    setTagUsingSMI( newTag, 1 ); /* Write asset tag select code */
}

// Code for getting the service tag from one of many locations
struct DellSetAssetTagFunctions
{
    void (*f_ptr)(const char *, size_t);
    const char * desc;
}

DellSetAssetTagFunctions[] = {
                                 {&setAssetTagUsingSMI, "SMI"},   // SMBIOS System Information Item
                                 {&setAssetTagUsingCMOSToken, "CMOS"},   // SMBIOS System Information Item
                             };

int SMBIOSSetAssetTag(const char *password, const char *assetTag, size_t len)
{
    int retval = -1;
    int numEntries =
        sizeof (DellSetAssetTagFunctions) / sizeof (DellSetAssetTagFunctions[0]);

    if(password)
        biosPassword = password;

    for (int i = 0; (i < numEntries); ++i)
    {
        // eat exceptions from lowlevel functions and keep going.
        try
        {
            // first function to return non-zero id wins.
            DellSetAssetTagFunctions[i].f_ptr (assetTag, len);
            retval = 0;
        }
        catch(const smbios::IException &e)
        {
            SysInfoException.setMessageString(e.what());
        }
        catch(...)
        {
            SysInfoException.setMessageString( _("Unknown internal error occurred") );
        }
    }
    return retval;
}


static char *getSystemNameFromSysInfo()
{
    return getStringFromTable(System_Information, System_Information_Product_Name_Offset);
}

// Struct for getting the system name from one of many locations
struct DellSystemNameFunctions
{
    char *(*f_ptr)();
}

DellSystemNameFunctions[] = {
                                {&getSystemNameFromSysInfo,}    // SMBIOS System Information Item
                            };

const char *SMBIOSGetSystemName()
{
    char *systemName= 0;
    int numEntries =
        sizeof (DellSystemNameFunctions) / sizeof (DellSystemNameFunctions[0]);

    for (int i = 0; (i < numEntries) && (!systemName); ++i)
    {
        // eat exceptions from lowlevel functions and keep going.
        try
        {
            // first function to return non-zero id wins.
            systemName = DellSystemNameFunctions[i].f_ptr ();
        }
        catch(const smbios::IException &e)
        {
            SysInfoException.setMessageString(e.what());
        }
        catch(...)
        {
            SysInfoException.setMessageString( _("Unknown internal error occurred") );
        }
    }

    stripString(systemName);
    return systemName;
}

// for Diamond only.
static char *getBiosVersionFromOneByteStructForDiamond()
{
    memory::IMemory *mem = 0;
    u8 strBuf[DELL_SYSTEM_STRING_LEN] = { 0, };
    u8 *biosVersion = 0;

    mem = memory::MemoryFactory::getFactory()->getSingleton();

    if( 0 == mem )
        throw InternalErrorImpl();

    // Step 1: Check that "Dell System" is present at the proper offset
    mem->fillBuffer( strBuf, DELL_SYSTEM_STRING_LOC_DIAMOND_1, DELL_SYSTEM_STRING_LEN - 1 );
    if( strncmp( reinterpret_cast<char*>(strBuf), DELL_SYSTEM_STRING, DELL_SYSTEM_STRING_LEN ) == 0 )
        if( SYSTEM_ID_DIAMOND == mem->getByte( ID_BYTE_LOC_DIAMOND_1 ) )
        {
            biosVersion = new u8[4];
            mem->fillBuffer(biosVersion, ID_BYTE_LOC_DIAMOND_1 + 1, 3);
            biosVersion[3] = '\0';
        }

    mem->fillBuffer( strBuf, DELL_SYSTEM_STRING_LOC_DIAMOND_2, DELL_SYSTEM_STRING_LEN - 1 );
    if( strncmp( reinterpret_cast<char*>(strBuf), DELL_SYSTEM_STRING, DELL_SYSTEM_STRING_LEN ) == 0 )
        if( SYSTEM_ID_DIAMOND == mem->getByte( ID_BYTE_LOC_DIAMOND_2 ) )
        {
            biosVersion = new u8[4];
            mem->fillBuffer(biosVersion, ID_BYTE_LOC_DIAMOND_2 + 1, 3);
            biosVersion[3] = '\0';
        }

    return reinterpret_cast<char*>(biosVersion);
}

static char *getBiosVersionFromSmbios()
{
    return getStringFromTable(BIOS_Information, BIOS_Information_Version_Offset);
}

// Code for getting the service tag from one of many locations
struct DellBiosVersionFunctions
{
    char *(*f_ptr)();
}
DellBiosVersionFunctions[] = {
                                 {&getBiosVersionFromOneByteStructForDiamond,},    // Diamond
                                 {&getBiosVersionFromSmbios,}
                             };

const char *SMBIOSGetBiosVersion()
{
    char *systemName= 0;
    int numEntries =
        sizeof (DellBiosVersionFunctions) / sizeof (DellBiosVersionFunctions[0]);

    for (int i = 0; (i < numEntries) && (!systemName); ++i)
    {
        // eat exceptions from lowlevel functions and keep going.
        try
        {
            // first function to return non-zero id wins.
            systemName = DellBiosVersionFunctions[i].f_ptr ();
        }
        catch(const smbios::IException &e)
        {
            SysInfoException.setMessageString(e.what());
        }
        catch(...)
        {
            SysInfoException.setMessageString( _("Unknown internal error occurred") );
        }
    }

    stripString(systemName);
    return systemName;
}


const char *SMBIOSGetVendorName()
{
    char *retval = 0;

    try
    {
        retval = getStringFromTable(System_Information, System_Information_Manufacturer_Offset);
    }
    catch(const smbios::IException &e)
    {
        SysInfoException.setMessageString(e.what());
    }
    catch(...)
    {
        SysInfoException.setMessageString( _("Unknown internal error occurred") );
    }

    stripString(retval);
    return retval;
}


int SMBIOSHasNvramStateBytes()
{
    int retval = 1;
    try
    {
        smbios::TokenTableFactory *ttFactory = smbios::TokenTableFactory::getFactory() ;
        smbios::ITokenTable *tokenTable = ttFactory->getSingleton();

        u8 tempData[2] = {0,0};
        (*tokenTable)[ NvramByte1_Token  ]->getString( tempData, 2 );
        (*tokenTable)[ NvramByte2_Token  ]->getString( tempData, 2 );
    }
    catch(const smbios::IException &e)
    {
        SysInfoException.setMessageString(e.what());
        retval = 0;
    }
    catch(...)
    {
        SysInfoException.setMessageString( _("Unknown internal error occurred") );
    }

    return retval;
}


// user =
//      0x0000 = DSA
//      0x8000 = OM Toolkit
//      0x9000 = open
//      0xA000 = open
//      0xB000 = open
//      0xC000 = open
//      0xD000 = open
//      0xE000 = open
//      0xF000 = expand to whole byte
int SMBIOSGetNvramStateBytes( int user )
{
    u8 tempData[2] = {0,0};
    int retval = 0;
    try
    {
        smbios::TokenTableFactory *ttFactory = smbios::TokenTableFactory::getFactory() ;
        smbios::ITokenTable *tokenTable = ttFactory->getSingleton();

        (*tokenTable)[ NvramByte1_Token  ]->getString( tempData, 2 );
        retval = *tempData;
        (*tokenTable)[ NvramByte2_Token  ]->getString( tempData, 2 );
        retval |= (*tempData << 8);
    }
    catch(const smbios::IException &e)
    {
        SysInfoException.setMessageString(e.what());
    }
    catch(...)
    {
        SysInfoException.setMessageString( _("Unknown internal error occurred") );
    }

    if( user == 0x0000 )  // DSA
    {
        if( (retval & 0x8000) != user )
        {
            retval = 0;  // user doesn't match, return default
        }
        retval &= ~0x8000; // mask user bits
    }
    else
    {
        if ((user & 0xF000) == 0xF000 ) // probably will never be used
        {
            if( (retval & 0xFF00) != user )
            {
                retval = 0;// user doesn't match, return default
            }
            retval &= ~0xFF00; // mask user bits
        }
        else
        {
            if( (retval & 0xF000) != user ) // Toolkit (or users 0x9 - 0xE)
            {
                retval = 0;// user doesn't match, return default
            }
            retval &= ~0xF000; // mask user bits
        }
    }
    return retval;
}

void SMBIOSSetNvramStateBytes(int value, int user)
{
    try
    {
        if ( user == 0x0000 ) // DSA
        {
            value &= ~0x8000;  // mask user bits
            value |= user;     // set user
        }
        else if( (user & 0xF000) == 0xF000 )
        {
            value &= ~0xFF00;   // mask user bits
            value |= user;      // set user
        }
        else
        {
            value &= ~0xF000;   // mask user bits
            value |= user;      // set user
        }

        smbios::TokenTableFactory *ttFactory = smbios::TokenTableFactory::getFactory() ;
        smbios::ITokenTable *tokenTable = ttFactory->getSingleton();

        u8 *tempData = reinterpret_cast<u8*>(&value);
        (*tokenTable)[ NvramByte1_Token  ]->setString( tempData, 1 );
        (*tokenTable)[ NvramByte2_Token  ]->setString( tempData+1, 1 );
    }
    catch(const smbios::IException &e)
    {
        SysInfoException.setMessageString(e.what());
    }
    catch(...)
    {
        SysInfoException.setMessageString( _("Unknown internal error occurred") );
    }
    return;
}


static bool getUpOffsetAndFlag (up_info *up)
{
    memory::IMemory *mem =
        memory::MemoryFactory::getFactory()->getSingleton();

    up_info tempUP;
    memset(&tempUP, 0, sizeof(tempUP));
    int step_size = 16;

    unsigned int fp = 0xF0000;
    bool found = false;
    while( fp < (0xFFFFFUL - sizeof(tempUP)) )
    {
        mem->fillBuffer(
            reinterpret_cast<u8 *>(&tempUP),
            fp,
            sizeof(tempUP)
        );

        if ( 0 == memcmp( &(tempUP.anchor), "_UP_", 4))
        {
            found = true;
            break;
        }

        fp += step_size;
        // for buggy BIOSen. If we don't find it on a paragraph boundary,
        // start over and do byte-by-byte
        if( step_size > 1 && fp >= (0xFFFFFUL - sizeof(tempUP)) )
        {
            step_size = 1;
            fp = 0xF0000;
        }
    }

    if( found )
        memcpy( up, &tempUP, sizeof(tempUP) );

    return found;
}

static int upBootHelper(bool set
                            =false, bool value=false)
{
    // retval = 0: NO BOOT TO UP CAPABILITY
    // retval = 1 && set; set to value
    // retval = 2 && !set; UP not active
    // retval = 3 && !set; UP Active
    int retval = 0;
    const u8 *buf = 0;

    up_info up;
    memset( reinterpret_cast<u8*>(&up), 0, sizeof(up));
    try
    {
        bool found = getUpOffsetAndFlag( &up );

        if( !found )
            goto out;

        smbios::TokenTableFactory *ttFactory = smbios::TokenTableFactory::getFactory() ;
        smbios::ITokenTable *tokenTable = ttFactory->getSingleton();
        size_t length;
        buf = (*tokenTable)[ NvramByte2_Token  ]->getItemRef().getBufferCopy(length);

        const indexed_io_access_structure *io_struct =
            reinterpret_cast<const indexed_io_access_structure *>(buf);

        cmos::ICmosRW *cmos = cmos::CmosRWFactory::getFactory()->getSingleton();

        u8 byte = cmos->readByte( io_struct->indexPort, io_struct->dataPort, up.offset );

        if( set
              )
            {
                // default to set
                byte |= up.flag;
                retval = 1;
                if (!value) // clear
                {
                    byte &= ~up.flag;
                }
                cmos->writeByte( io_struct->indexPort, io_struct->dataPort, up.offset, byte );
            }
        else
        {
            if( (byte & up.flag) == up.flag )
                retval = 3;

            if( (byte & up.flag) != up.flag )
                retval = 2;
        }

    }
    catch(const smbios::IException &e)
    {
        SysInfoException.setMessageString(e.what());
    }
    catch(...)
    {
        SysInfoException.setMessageString( _("Unknown internal error occurred") );
    }

    delete [] const_cast<u8 *>(buf);
    buf = 0;

out:
    return retval;
}

int SMBIOSHasBootToUp()
{
    return upBootHelper();
}

int SMBIOSGetBootToUp()
{
    int retval = upBootHelper();
    retval -= 2;
    return retval;
}

void SMBIOSSetBootToUp(int state)
{
    bool value = (state == 1) ? true: false;
    upBootHelper(true, value);
}


int SMBIOSGetSmiPasswordCoding()
{
    int fmt=0;
    try
    {
        fmt = smi::getPasswordFormat();
    }
    catch(const exception &)
    {}

    return fmt;
}

