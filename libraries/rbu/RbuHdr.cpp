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

// compat header should always be first header if including system headers
#include "smbios/compat.h"

#include <errno.h>
#include <string.h>  // strerror()
#include <iostream>
#include <sstream>
#include <iomanip>

#include "RbuImpl.h"
#include "smbios/IToken.h"
#include "smbios/SystemInfo.h"

// always include last if included.
#include "smbios/message.h"  // not needed outside of this lib. (mainly for gettext i18n)

using namespace std;

namespace rbu
{

    RbuFactory::~RbuFactory() throw() {}
    RbuFactory::RbuFactory() {}

    RbuFactory *RbuFactory::getFactory()
    {
        // reinterpret_cast<...>(0) to ensure template parameter is correct
        // this is a workaround for VC6 which cannot use explicit member template
        // funciton initialization.
        return RbuFactoryImpl::getFactory(reinterpret_cast<RbuFactoryImpl *>(0));
    }

    RbuFactoryImpl::~RbuFactoryImpl() throw() { }
    RbuFactoryImpl::RbuFactoryImpl() { }
    IRbuHdr *RbuFactoryImpl::makeNew(std::string filename)
    {
        return new RbuHdr( filename );
    }

    IRbuHdr::IRbuHdr() {}
    IRbuHdr::~IRbuHdr() {}


    RbuHdr::RbuHdr( string filename ) : hdrFh(fopen(filename.c_str(), "rb"))
    {
        if(!hdrFh)
        {
            string errmsg = strerror(errno);
            throw HdrFileIOErrorImpl(errmsg);
        }

        memset(&header, 0, sizeof(header));
        size_t bytesRead = fread(&header, 1, sizeof(header), hdrFh);  // short read handled
        if (bytesRead != sizeof(header))
        {
            fclose(hdrFh);
            hdrFh=0;
            throw InvalidHdrFileImpl("Couldnt read full header.");
        }
        fseek(hdrFh, 0, 0);
        if (
                header.headerId[0] == '$' &&
                header.headerId[1] == 'R' &&
                header.headerId[2] == 'B' &&
                header.headerId[3] == 'U'
           )
        {
        }
        else 
        {
            fclose(hdrFh);
            hdrFh=0;
            throw InvalidHdrFileImpl("Did not pass header $RBU check.");
        }

        // initialize system id list
        memset(sysIdList, 0, sizeof(sysIdList));
        for( unsigned int i=0; i < (sizeof(header.systemIdList)/sizeof(header.systemIdList[0])); ++i)
        {
            if(i >= static_cast<unsigned int>(NUM_SYS_ID_IN_HDR)) break;  // safety... 

            // bits 8:10 are hw rev id, extract them and paste the rest together
            u32 val = header.systemIdList[i];
            u32 id = (val & 0xFF) | ((val & 0xF800) >> 3);

            if(!id) break;

            sysIdList[i] = id;
        }
    }

    RbuHdr::~RbuHdr() 
    {
        if(hdrFh)
            fclose(hdrFh);
    }

    string RbuHdr::getBiosVersion() const 
    {
        string ver("");
        if( header.headerMajorVer < 2 )
        {
            if( isalnum(header.biosVersion[0]) )
                ver = ver + header.biosVersion[0];
    
            if( isalnum(header.biosVersion[1]) )
                ver = ver + header.biosVersion[1];
    
            if( isalnum(header.biosVersion[2]) )
                ver = ver + header.biosVersion[2];
        } 
        else 
        {
            std::ostringstream rep;
            rep << static_cast<int>(header.biosVersion[0]) << "."
                << static_cast<int>(header.biosVersion[1]) << "."
                << static_cast<int>(header.biosVersion[2]);
            ver = rep.str();
        }
        return ver;
    }

    void RbuHdr::getHdrVersion(unsigned int &major, unsigned int &minor) const 
    { 
        major = header.headerMajorVer;
        minor = header.headerMinorVer;
    }

    const u32 * RbuHdr::getSystemIdList() const {return sysIdList;}

    
    void RbuHdr::doUpdate() const {}

    bool checkSystemId(const IRbuHdr &hdr, u16 sysId )
    {
        const u32 *idList = hdr.getSystemIdList();
        for (const u32 *ptr = idList; *ptr; ++ptr)
            if( *ptr == sysId )
                return true;

        return false;
    }

    FILE *RbuHdr::getFh() const 
    { 
        return hdrFh;
    }

    ostream & operator << (ostream & cout, const IRbuHdr & hdr)
    {
        return hdr.streamify (cout);
    }

    ostream & RbuHdr::streamify( ostream &cout ) const 
    {
        std::ios::fmtflags old_opts = cout.flags ();
        cout << "HeaderId : " 
            << header.headerId[0]
            << header.headerId[1]
            << header.headerId[2]
            << header.headerId[3] << endl;

        cout << "Header Length: " << static_cast<int>(header.headerLength) << endl;
        cout << "Header Major Ver: " << static_cast<int>(header.headerMajorVer) << endl;
        cout << "Header Minor Ver: " << static_cast<int>(header.headerMinorVer) << endl;
        cout << "Num Systems: " << static_cast<int>(header.numSystems) << endl;

        cout << "Version: " << getBiosVersion() << endl; 

        char quickCheck[41] = {0};
        strncpy(quickCheck, header.quickCheck, 40);
        cout << "Quick Check: " << quickCheck << endl;

        cout << "System ID List:" << hex;

        for (const u32 *ptr = sysIdList; *ptr; ++ptr)
            cout << " 0x" << setfill ('0') << setw (4) << *ptr;

        cout << endl << dec;
        cout.flags (old_opts);

        return cout;
    }

    // Helper functions
    static string stringToLower(string in)
    {
        for(unsigned int i=0;i<in.length();i++)
            in[i] = tolower(in[i]);

        return in;
    }

    static int compareSamePrefixOldBiosVersion(std::string ver1, std::string ver2)
    {
        if(ver1 > ver2)
            return -1;

        if(ver1 < ver2)
            return 1;

        // should _NEVER_ get here.
        return 0;
    }

    static int compareOldBiosVersion(std::string ver1, std::string ver2)
    {
        if (ver1[0] == ver2[0])
            return compareSamePrefixOldBiosVersion(ver1, ver2);

        if (tolower(ver1[0]) == 'a')
            return -1;

        if (tolower(ver2[0]) == 'a')
            return 1;

        if (tolower(ver1[0]) == 'x')
            return -1;

        if (tolower(ver2[0]) == 'x')
            return 1;

        if (tolower(ver1[0]) == 'p')
            return -1;

        if (tolower(ver2[0]) == 'p')
            return 1;

        if (ver1[0] > ver2[0])
            return -1;

        return 1;
    }

    // should be static, but we want to test this in unit test suite. NOT PART OF PUBLIC API
    void splitNewVersion(std::string ver, unsigned int &maj, unsigned int &min, unsigned int &ext)
    {
        unsigned int verSplit[3] = {0,};

        DCOUT("splitNewVersion( \""<< ver <<"\" )" << endl);

        size_t start=0, end=0;
        for(int i=0; i<3; i++)
        {
            string verPart = "";
            end = ver.find('.', start);
            verPart.append(ver, start, end - start);

            verSplit[i] = strtoul(verPart.c_str(), 0, 10);
            DCOUT("Start(" << start << ") End(" << end << ") verPart(\"" << verPart << "\") verSplit[" << i << "](" << verSplit[i] << ")" << endl);

            if( end == string::npos )
                break;

            start = end + 1;
        }

        maj = verSplit[0];
        min = verSplit[1];
        ext = verSplit[2];

        DCOUT("Return: (" << maj << ", " << min << ", " << ext << ")" << endl);
    }
    

    static int compareNewBiosVersion(std::string ver1, std::string ver2)
    {
        unsigned int maj1, min1, ext1;
        unsigned int maj2, min2, ext2;
        splitNewVersion( ver1, maj1, min1, ext1 );
        splitNewVersion( ver2, maj2, min2, ext2 );

        // check for 90-99 major. Should never win against non-90-99 ver:
        const unsigned int SPECIAL_VER_START = 90;
        if (maj1 >= SPECIAL_VER_START && maj2 < SPECIAL_VER_START)
            return 1;
        if (maj1 < SPECIAL_VER_START && maj2 >= SPECIAL_VER_START)
            return -1;

        if (maj1 > maj2)
            return -1;
        if (maj1 < maj2)
            return  1;
 
        if (min1 > min2)
            return -1;
        if (min1 < min2)
            return  1;
 
        if (ext1 > ext2)
            return -1;
        if (ext1 < ext2)
            return  1;
        
        // should never get here as == versions should already be handled.
        return 0;
    }


    int compareBiosVersion(std::string ver1, std::string ver2)
    {
        ver1 = stringToLower(ver1);
        ver2 = stringToLower(ver2);

        // same string
        if(ver1 == ver2)
            return 0;

        // both old style
        if ( isalpha(ver1[0]) && isalpha(ver2[0]) )
            return compareOldBiosVersion(ver1, ver2);

        // one new, one old, new wins
        if ( ! isalpha(ver1[0]) && isalpha(ver2[0]) )
            return -1;

        // one new, one old, new wins
        if ( isalpha(ver1[0]) && !isalpha(ver2[0]) )
            return 1;

        if ( ! isalpha(ver1[0]) && !isalpha(ver2[0]) )
            return compareNewBiosVersion(ver1, ver2);

        // cannot get here...
        return 0;
    }


    // private functions

    packet_type getSupportedPacketType(void)
    {
        packet_type pt = pt_mono;
        try
        {
            smbios::SmbiosFactory *smbiosFactory = smbios::SmbiosFactory::getFactory();
            smbios::ISmbiosTable *table = smbiosFactory->getSingleton();
            const smbios::ISmbiosItem &rbuStructure = *((*table)[RBU_SMBIOS_STRUCT]);
    
            u8 byte = getU8_FromItem(rbuStructure, 0x0F); // Characteristics field
            if( byte & 0x01 ) 
                pt = pt_packet;
        }
        catch(const smbios::DataOutOfBounds &)
        {
            // only happens when old-style struct present w/o characteristics field
            // this means only mono supported.
        }
        return pt;
    }
        
    void activateRbuToken()
    {
        smbios::TokenTableFactory *ttFactory = smbios::TokenTableFactory::getFactory() ;
        smbios::ITokenTable *tokenTable = ttFactory->getSingleton();
        (*tokenTable)[ RBU_ACTIVATE ]->activate();
    }
    
    void cancelRbuToken()
    {
        smbios::TokenTableFactory *ttFactory = smbios::TokenTableFactory::getFactory() ;
        smbios::ITokenTable *tokenTable = ttFactory->getSingleton();
        (*tokenTable)[ RBU_CANCEL ]->activate();
    }

    void checksumPacket(rbu_packet *pkt, size_t size)
    {
        u16 *buf = reinterpret_cast<u16 *>(pkt);
        pkt->pktChksum = 0;
    
        u16 csum = 0;
        for(size_t i=0; i<size/2; ++i)
            csum = csum + buf[i];
    
        pkt->pktChksum = -csum;
    }

    void createPacket(char *buffer, size_t bufSize, size_t imageSize)
    {
        // set up packet
        rbu_packet *pkt = reinterpret_cast<rbu_packet *>(buffer);
    
        pkt->pktId = 0x4B505224;  //2452504B;   // must be '$RPK'
        pkt->pktSize = bufSize / 1024;    // size of packet in KB
        pkt->reserved1 = 0;  //
        pkt->hdrSize = 2;    // size of packet header in paragraphs (16 byte chunks)
        int datasize = bufSize - (16 * pkt->hdrSize);

        pkt->reserved2 = 0;  //
        pkt->pktSetId = 0x12345678;   // unique id for packet set, can be anything
        pkt->pktNum = 0;     // sequential pkt number (only thing that changes)
        pkt->totPkts = (imageSize/datasize) + ((imageSize % datasize) ? 1:0) + 1;// total number of packets
        pkt->pktVer = 1;     // version == 1 for now
        pkt->pktChksum = 0;  // sum all bytes in pkt must be zero
    
        checksumPacket(pkt, bufSize);
    }
}
