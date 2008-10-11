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

#ifndef DELLMAGIC_H
#define DELLMAGIC_H

// private stuff
#define NvramByte1_Token                            0x83
#define NvramByte2_Token                            0x84
#define BIOS_Information_Version_Offset             0x05
#define System_Information_Manufacturer_Offset      0x04
#define System_Information_Product_Name_Offset      0x05
#define System_Information_Serial_Number_Offset     0x07
#define System_Enclosure_or_Chassis_Service_Offset  0x07
#define System_Enclosure_or_Chassis_Asset_Offset    0x08
#define ID_Byte_Location                            0xFE845
#define ID_Word_Location                            0xFE840
#define Cmos_Asset_Token                            0xC000
#define Cmos_Service_Token                          0xC003
#define OEM_String_Field_Number                     1
#define Bayonet_Detect_String                       "Dell System"
#define DELL_SYSTEM_STRING_LOC                      0xFE076
#define DELL_SYSTEM_STRING                          "Dell System"
#define DELL_SYSTEM_STRING_LEN                      12
#define TWO_BYTE_STRUCT_LOC                         0xFE840
#define DELL_SYSTEM_STRING_LOC_DIAMOND_1            0xD8044
#define DELL_SYSTEM_STRING_LOC_DIAMOND_2            0xDC044
#define ID_BYTE_LOC_DIAMOND_1                       0xD8040
#define ID_BYTE_LOC_DIAMOND_2                       0xDC040
#define SYSTEM_ID_DIAMOND                           0x8C
#define OEM_Dell_String                             "Dell"
#define OEM_String_Location                         0xFE076
#define SVC_TAG_CMOS_LEN_MAX                        5
#define SVC_TAG_LEN_MAX                             7
#define ASSET_TAG_CMOS_LEN_MAX                      10
#define ASSET_TAG_LEN_MAX                           10

namespace smbios
{
#if defined(_MSC_VER)
#pragma pack(push,1)
#endif
    // all of these are packed, so put them all between the above ifdef/below
    // endif.

    struct up_info
    {
        char anchor[4];
        u16  stuff1; // anybody know what this is?
        u8   offset;
        u16  stuff2; // anybody know what this is?
        u8   flag;
    }
    LIBSMBIOS_PACKED_ATTR;

    struct one_byte_structure
    {
        u8   bios_version[3];
        u8   system_id;
        u8   platform_revision;
        u8   checksum; //(offsets 3+4+5 must equal 0)
    }
    LIBSMBIOS_PACKED_ATTR;

    struct two_byte_structure
    {
        u16  two_byte_id;
        u8   bios_version[3];
        u8   system_id;
        u8   platform_revision;
        u8   checksum; // (offsets 5+6+7 must equal 0)
        u8   Reserved1;
        u8   Reserved2;
        u8   extended_checksum;// ( offsets 0x0 + 0x1 + 0xA + 0xB + extension bytes must equal 0)
        u8   extension_byte_count;
        u8   first_extended_byte; // Brand ID
        //u8   ... more extension bytes ...
    }
    LIBSMBIOS_PACKED_ATTR;

#if defined(_MSC_VER)
#pragma pack(pop)
#endif
}

#endif
