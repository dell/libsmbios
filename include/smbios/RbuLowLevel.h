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

#ifndef RBULOWLEVEL_H
#define RBULOWLEVEL_H

// compat header should always be first header if including system headers
#include "smbios/compat.h"

#   define RBU_SMBIOS_STRUCT (0xDE)
#   define RBU_ACTIVATE (0x005c)
#   define RBU_CANCEL   (0x005d)

// WARNING WARNING WARNING
// 
// This file is not guaranteed to be API/ABI stable. Include it at your own risk.
//
// WARNING WARNING WARNING

namespace rbu 
{
#   if defined(_MSC_VER)
#       pragma pack(push,1)
#   endif

    /* RBU packets are 4 KB-aligned and consist of a header and data. The packet header contains the necessary information for BIOS to find the packets in memory, and assemble them in proper order. */

    struct  rbu_packet
    {
        u32 pktId;      // must be '$RPK'
        u16 pktSize;    // size of packet in KB
        u16 reserved1;  // 
        u16 hdrSize;    // size of packet header in paragraphs (16 byte chunks)
        u16  reserved2; //
        u32 pktSetId;   // unique id for packet set, can be anything
        u16 pktNum;     // sequential pkt number (only thing that changes)
        u16 totPkts;    // total number of packets
        u8  pktVer;     // version == 1 for now
        u8  reserved[9];
        u16 pktChksum;  // sum all bytes in pkt must be zero
        u8  pktData;  // Start of packet data.
    }
    LIBSMBIOS_PACKED_ATTR;


    /* RBU Packet Requirements
    
    1.All values in the packet header except PktNum must be the same for all packets in a set with the following exception:
            -- Packet 0 may have a different packet size (PktSize).
            -- checksums
    2.Packet 0 data does not contain RBU data. Packet 1 contains the first chunk of RBU data.
    3.Packet data begins immediately after the header. Packet data size and offset can be calculated from PktSize and HdrSize. 
    4.Reserved fields are 0.
    5.If multiple packets sets are written to memory, all packet sets must be identical.
    6.All packets must start on 4 KB boundaries.
    7.All packets must be placed in non-paged memory.
    8.The maximum size of a packet is 64 MB.
    9.The maximum size of a packet header is 4 KB.
    10.The maximum number of packets is 64 KB - 1.
    11.CPU INIT# must be immediately asserted (e.g. via OS shutdown/restart) after the RBU packet set is placed in memory.
    12.PktChk is the value resulting in a zero sum of all packet words (header and data).
    13.PktSetId uniquely identifies a packet set. BIOS aborts the packet search if all packets do not have the same PkSetId. Example identifiers: a 4-character ASCII ID string (e.g. “_A00”), a 4-byte hash value (e.g. CRC).
        */
    
    /*  RBU Packet 0 */
    
    struct  rbu_packet_0
    {
        rbu_packet  header;
        u8  passwordCheckInfo;  // bit 7: passwordCheck is present   bits 0-6: reserved
        u32 passwordCheck;      // crc-32 of admin/setup password
        // the rest is reserved for future expansion.
    }
    LIBSMBIOS_PACKED_ATTR;

    /* RBU Packet 0 Definition

    Packet 0 is reserved for packet set information. Packet 0 data consists of data items -- each item consists of an info byte followed by the actual data item. If bit 0 of the info byte is 1, the actual data starting at the next byte is present. If bit 0 is 0, the data is not present.
    
    The system flash password is currently defined as the admin or setup password.
    
    BIOS reject the packet set when:
    1.The packet set flash password CRC and the system flash password CRC do not match.
    2.The packet set flash password CRC is not present but the system flash password is present.
    
    */


    /* 
       RBU BIOS UPDATE HEADER FILE (.HDR) structure
       */

    const int NUM_SYS_ID_IN_HDR = 12;

    struct rbu_hdr_file_header
    {
        char headerId[4];
        u8  headerLength;
        u8  headerMajorVer;
        u8  headerMinorVer;
        u8  numSystems;
        char quickCheck[40];
        char biosVersion[3];
        u8  miscFlags;
        u8  biosInternalUse;
        u8  reserved[5];
        u16 compatFlags;
        u16 systemIdList[NUM_SYS_ID_IN_HDR]; /* 
            Contains the list of NumSystems Dell System ID and Hardware Revision 
            ID pairs for which the Image Data is valid, in the following format:
            Bit Range  Description
            15:11      Dell System ID, bits 12:8.  
                        This range is set to 00000b if the Dell System ID 
                        is a 1-byte value.
            10:8       Hardware Revision ID
            7:0        Dell System ID, bits 7:0. */
    }
    LIBSMBIOS_PACKED_ATTR;

#   if defined(_MSC_VER)
#       pragma pack(pop)
#   endif

}

#endif /* RBUIMPL_H */
