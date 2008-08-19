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
#define LIBSMBIOS_SOURCE
#include "smbios/compat.h"

#include <iomanip>
#include <string.h>

#include "TokenImpl.h"

using namespace std;

namespace smbios
{
    CmosTokenD4::CmosTokenD4( const smbios::ISmbiosItem &initItem, const indexed_io_token *initToken )
            : IToken(), ICmosToken(), item(initItem.clone()), cmos(cmos::CmosRWFactory::getFactory()->getSingleton())
    {
        memcpy( const_cast<indexed_io_token *>(&token), initToken, sizeof(token) );

        size_t size;
        const u8 *ptr =  item->getBufferCopy(size) ; // MUST DELETE[]!
        memcpy( const_cast<indexed_io_access_structure*>(&structure), ptr, sizeof(structure) );
        delete [] const_cast<u8 *>(ptr); //const_cast to fix msvc++
    }

    // no dynamically allocated memory, yay!
    CmosTokenD4::~CmosTokenD4() throw()
    {}

    string CmosTokenD4::getTokenClass() const
    {
        return "TokenD4";
    }

    const ISmbiosItem &CmosTokenD4::getItemRef() const
    {
        return *item;
    }

    u32 CmosTokenD4::getType() const
    {
        return token.tokenId;
    }

    bool CmosTokenD4::isActive() const
    {
        if( isString() )
            throw InvalidAccessModeImpl("tried to call isActive() on a string token." );

        bool retval = false;

        u8 byte = cmos->readByte(
                      structure.indexPort,
                      structure.dataPort,
                      token.location
                  );

        if( (byte & (~token.andMask)) == token.orValue  )
            retval = true;

        return retval;
    }

    void CmosTokenD4::activate() const
    {
        if( isString() )
            throw InvalidAccessModeImpl("tried to activate() a string token." );

        u8 byte = cmos->readByte(
                      structure.indexPort,
                      structure.dataPort,
                      token.location
                  );

        byte = byte & token.andMask;
        byte = byte | token.orValue;

        cmos->writeByte(
            structure.indexPort,
            structure.dataPort,
            token.location,
            byte
        );
    }

    bool CmosTokenD4::isString() const
    {
        bool retval = false;
        if( 0 == token.andMask)
            retval = true;
        return retval;
    }

    bool CmosTokenD4::isBool() const
    {
        return ! isString();
    }

    const string CmosTokenD4::getString(u8 *byteArray, unsigned int size ) const
    {
        if( ! isString() )
            throw InvalidAccessModeImpl("tried to call getString() on a bit token.");

        bool allocatedMem = false;
        try
        {
            unsigned int strSize = getStringLength();
            if( !byteArray )
            {
                size = strSize + 1;
                byteArray = new u8[size];
                allocatedMem = true;
            }

            if( size < strSize + 1 )
                throw ParameterErrorImpl("called getString() with too small of a buffer."); // not enough space to store results

            for( unsigned int i=0; i<strSize; ++i )
                byteArray[i] = '\0';

            cmos::readByteArray(
                *cmos,
                structure.indexPort,
                structure.dataPort,
                token.location,
                byteArray,
                strSize
            );

            byteArray[ getStringLength() ] = '\0';
            string retval(reinterpret_cast<const char *>(byteArray));
            if( allocatedMem )
            {
                delete [] byteArray;
                byteArray = 0;
                allocatedMem = false;
            }
            return retval;

        }
        catch ( const std::exception & )
        {
            if( allocatedMem )
                delete [] byteArray;
            throw;
        }

    }

    void CmosTokenD4::setString( const u8 *byteArray, size_t size ) const
    {
        if( ! isString() )
            throw InvalidAccessModeImpl("tried to setString() on non-string.");

        unsigned int strSize = getStringLength();

        u8 *targetBuffer = new u8[strSize];
        memset(targetBuffer, 0, strSize);
        memcpy( targetBuffer, byteArray, size < strSize ? size : strSize );

        cmos::writeByteArray(
            *cmos,
            structure.indexPort,
            structure.dataPort,
            token.location,
            targetBuffer,
            strSize
        );

        delete[](targetBuffer);
    }

    unsigned int CmosTokenD4::getStringLength() const
    {
        if( ! isString() )
            throw InvalidAccessModeImpl("tried to getStringLength on non-string.");
        // STRING must be at least 1 byte. Does not make sense
        // otherwise. BIOS Error? NVRAM byte tokens (0x83/0x84) seem to be
        // string tokens of length 0. That looks wrong.
        return token.stringLength ? token.stringLength : 1;
    }

    void CmosTokenD4::getCMOSDetails( u16 *indexPort, u16 *dataPort, u8 *location ) const
    {
        *indexPort = structure.indexPort;
        *dataPort = structure.dataPort;
        *location = token.location;
        return;
    }

    std::ostream & CmosTokenD4::streamify( std::ostream & cout ) const
    {
        std::ios::fmtflags old_opts = cout.flags ();

        cout << "DMI type 0x" << hex << setfill ('0') << setw (2) << static_cast<int>(structure.type);
        cout << "  Handle 0x" << hex << setfill ('0') << setw (4) << static_cast<int>(structure.handle);
        cout << "  Index Port 0x" << hex << setw(2) << structure.indexPort;
        cout << "  Data Port 0x"  << hex << setw(2) << structure.dataPort;
        cout << "  Type 0x" << hex << setw(4) << static_cast<int>(getType());
        cout << "  Location 0x" << hex << setw(2) << static_cast<int>(token.location);
        if( isString() )
        {
            cout << " STRING  Length " << dec << setfill('0') << setw(2) << getStringLength() ;
            cout << " value(" << getString() << ")";
        }
        else
        {
            cout << " AND(" << setw(1) << static_cast<int>(token.andMask) << ") ";
            cout << "OR(" << setw(1) << static_cast<int>(token.orValue) << ") ";
            cout << " BITFIELD: " << isActive();
        }

        cout.flags (old_opts);

        return cout;
    }

}
