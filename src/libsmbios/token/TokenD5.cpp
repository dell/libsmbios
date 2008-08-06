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

#include <sstream>
#include <iomanip>
#include <string.h>

#include "TokenImpl.h"

using namespace std;

namespace smbios
{
    CmosTokenD5::CmosTokenD5( const smbios::ISmbiosItem &initItem, std::vector< CmosRWChecksumObserver > &initChecksumList)
            : IToken(), ICmosToken(), IProtectedToken(), item(initItem.clone()), cmos(cmos::CmosRWFactory::getFactory()->getSingleton()),
            validationKey(""), checksumList(initChecksumList)
    {
        size_t size;
        const u8 *ptr =  item->getBufferCopy(size) ; // MUST DELETE[]!
        size = size < sizeof(structure)? size : sizeof(structure);
        memcpy( const_cast<dell_protected_value_1_structure*>(&structure), ptr, size );
        delete [] const_cast<u8 *>(ptr); //const_cast to fix msvc++

        // note that if password is set (validation key non-null), then most
        //  of the structure is "encrypted" with the password
        if (! structure.validationKey )
        {
            addChecksumObserver();
        }
    }

    // no dynamically allocated memory, yay!
    CmosTokenD5::~CmosTokenD5() throw()
    {}

    string CmosTokenD5::getTokenClass() const
    {
        return "TokenD5";
    }

    u32 CmosTokenD5::getValueFormat() const
    {
        return structure.valueFormat;
    }

    const ISmbiosItem &CmosTokenD5::getItemRef() const
    {
        return *item;
    }

    u32 CmosTokenD5::getType() const
    {
        return structure.tokenId;
    }

    bool CmosTokenD5::isActive() const
    {
        throw InvalidAccessModeImpl();
    }

    void CmosTokenD5::activate() const
    {
        throw InvalidAccessModeImpl();
    }

    bool CmosTokenD5::isString() const
    {
        return true;
    }

    bool CmosTokenD5::isBool() const
    {
        return ! isString();
    }

    const string CmosTokenD5::getString(u8 *byteArray, unsigned int size ) const
    {
        if ( structure.validationKey )
            throw NeedAuthenticationImpl("not decoded yet");

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
                throw ParameterErrorImpl(); // not enough space to store results

            for( unsigned int i=0; i<strSize; ++i )
                byteArray[i] = '\0';

            cmos::readByteArray(
                *cmos,
                structure.indexPort,
                structure.dataPort,
                structure.valueStartIndex,
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
            {
                delete [] byteArray;
                byteArray = 0;
                allocatedMem = false;
            }
            throw;
        }

    }

    void CmosTokenD5::setString( const u8 *byteArray, size_t size ) const
    {
        if ( structure.validationKey )
            throw NeedAuthenticationImpl("not decoded yet");

        unsigned int strSize = getStringLength();

        u8 *targetBuffer = new u8[strSize];
        try
        {
            memset(targetBuffer, 0, strSize);
            memcpy( targetBuffer, byteArray, size < strSize ? size : strSize );

            cmos::writeByteArray(
                *cmos,
                structure.indexPort,
                structure.dataPort,
                structure.valueStartIndex,
                targetBuffer,
                strSize
            );
            // keep this in sync with below
            delete[](targetBuffer);
            targetBuffer=0;
        }
        catch(...)
        {
            // keep this in sync with above
            delete[](targetBuffer);
            targetBuffer=0;
            throw;
        }
    }

    unsigned int CmosTokenD5::getStringLength() const
    {
        // STRING must be at least 1 byte. Does not make sense
        // otherwise. BIOS Error? NVRAM byte tokens (0x83/0x84) seem to be
        // string tokens of length 0. That looks wrong.
        return structure.valueLen ? structure.valueLen : 1;
    }

    void CmosTokenD5::getCMOSDetails( u16 *indexPort, u16 *dataPort, u8 *location ) const
    {
        if ( structure.validationKey )
            throw NeedAuthenticationImpl("not decoded yet");

        *indexPort = structure.indexPort;
        *dataPort = structure.dataPort;
        *location = structure.valueStartIndex;
        return;
    }

    void CmosTokenD5::addChecksumObserver() const
    {
        ostringstream ost;
        ost << *item;

        CmosRWChecksumObserver chk(
            ost.str(),
            cmos,
            structure.checkType,
            structure.indexPort,
            structure.dataPort,
            structure.valueStartIndex,
            structure.valueStartIndex + structure.valueLen - 1,
            structure.checkIndex  );
        checksumList.push_back( chk );
    }

    bool CmosTokenD5::tryPassword(std::string pw) const
    {
        cout << "Password decode code not yet present." << pw << endl;
        return false;
        // addChecksumObserver() after we successfully decode password
    }

    std::ostream & CmosTokenD5::streamify( std::ostream & cout ) const
    {
        std::ios::fmtflags old_opts = cout.flags ();

        cout << "DMI type 0x" << hex << setfill ('0') << setw (2) << static_cast<int>(structure.type);
        cout << "  Handle 0x" << hex << setfill ('0') << setw (4) << static_cast<int>(structure.handle);
        cout << "  Index Port 0x" << hex << setw(2) << structure.indexPort;
        cout << "  Data Port 0x"  << hex << setw(2) << structure.dataPort;
        cout << "  Type 0x" << hex << setw(4) << static_cast<int>(getType());
        cout << "  Location 0x" << hex << setw(2) << static_cast<int>(structure.valueStartIndex);
        cout << " STRING  Length " << dec << setfill('0') << setw(2) << getStringLength() ;
        cout << " value(" << getString() << ")";

        cout.flags (old_opts);

        return cout;
    }

}
