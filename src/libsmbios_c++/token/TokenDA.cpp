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

// compat header should always be first header if including system headers
#define LIBSMBIOS_SOURCE
#include "smbios/compat.h"

#include <iomanip>
#include <string.h>

#include "TokenImpl.h"

#include "smbios/ISmi.h"

#define TODO do { throw NotImplementedImpl(); } while(0)

using namespace std;

namespace smbios
{
    SmiTokenDA::SmiTokenDA( const smbios::ISmbiosItem &initItem, const calling_interface_token *initToken )
            : IToken(), ISmiToken(), IProtectedToken(), item(initItem.clone()), password("")
    {
        memcpy( const_cast<calling_interface_token *>(&token), initToken, sizeof(token) );

        size_t size;
        const u8 *ptr =  item->getBufferCopy(size) ; // MUST DELETE[]!
        memcpy( const_cast<calling_interface_structure*>(&structure), ptr, sizeof(structure) );
        delete [] const_cast<u8 *>(ptr); //const_cast to fix msvc++
    }

    // no dynamically allocated memory, yay!
    SmiTokenDA::~SmiTokenDA() throw()
    {}

    string SmiTokenDA::getTokenClass() const
    {
        return "TokenDA";
    }

    u32 SmiTokenDA::getValueFormat() const
    {
        return 0xFFFFFFFF;
    }

    bool SmiTokenDA::tryPassword(std::string pw) const
    {
        // can't really validate password without retrying operation
        password = pw;
        return true;
    }

    const ISmbiosItem &SmiTokenDA::getItemRef() const
    {
        return *item;
    }

    void SmiTokenDA::getSmiDetails(  u16 *cmdIOAddress, u8 *cmdIOCode, u8 *location ) const
    {
        if (cmdIOAddress)
            *cmdIOAddress = structure.cmdIOAddress;
        if (cmdIOCode)
            *cmdIOCode = structure.cmdIOCode;
        if (location)
            *location = token.location;
    }

    u32 SmiTokenDA::getType() const
    {
        return token.tokenId;
    }

    bool SmiTokenDA::isActive() const
    {
        bool ret = false;

        DCERR("reading token: " << static_cast<u32>(token.location) << "  compare with value: " << static_cast<u32>(token.value) << "  actual: " << smi::readNVStorage(token.location, 0, 0) << endl);
        if (token.value == smi::readNVStorage(token.location, 0, 0))
            ret = true;

        return ret;
    }

    static void executeWithPassword(smi::IDellCallingInterfaceSmi *ci, u8 arg, string password)
    {
        for(int i=0; i<2; i++)
        {
            try
            {
                ci->execute();
                break;
            }
            catch(const smi::SmiExecutedWithError &)
            {
                // on second time through, just pass exception upwards.
                if(i==1)
                    throw;

                //cout << "Caught error. Might be bad password. Trying password: " << password << endl;
                ci->setArg( arg, smi::getAuthenticationKey(password));
            }
        }
    }

    void SmiTokenDA::activate() const
    {
        DCERR("trying to activate token: " << static_cast<u32>(token.location) << "  with value: " << static_cast<u32>(token.value) << endl);
        smi::writeNVStorage(password, token.location, token.value, 0, 0);
    }

    bool SmiTokenDA::isString() const
    {
        return true;
    }

    bool SmiTokenDA::isBool() const
    {
        return true;
    }

    const string SmiTokenDA::getString(u8 *byteArray, unsigned int size ) const
    {
        std::auto_ptr<smi::IDellCallingInterfaceSmi> smi = smi::SmiFactory::getFactory()->makeNew(smi::SmiFactory::DELL_CALLING_INTERFACE_SMI);

        smi::IDellCallingInterfaceSmi *ci = dynamic_cast<smi::IDellCallingInterfaceSmi *>(smi.get());
        ci->setClass( 0x0 );  /* Read Non-Volatile Storage class code */
        ci->setSelect( 0x0 );  /* Read Non-Volatile Storage select code */
        ci->setArg( 0, token.location );
        ci->execute();

        // first word is data. ignore high bits.
        u16 word = static_cast<u16>(ci->getRes(1));

        if(byteArray && size >= 2)
        {
            memset(byteArray, 0, size);
            memcpy(byteArray, &word, sizeof(u16));
        }

        char ret[3]={0};
        memcpy(ret, &word, sizeof(u16));

        return ret; //automatically converted to std::string
    }

    void SmiTokenDA::setString( const u8 *byteArray, size_t size ) const
    {
        if( size < 2 )
            return;

        std::auto_ptr<smi::IDellCallingInterfaceSmi> smi = smi::SmiFactory::getFactory()->makeNew(smi::SmiFactory::DELL_CALLING_INTERFACE_SMI);

        smi::IDellCallingInterfaceSmi *ci = dynamic_cast<smi::IDellCallingInterfaceSmi *>(smi.get());
        ci->setClass( 0x1 );  /* Read Non-Volatile Storage class code */
        ci->setSelect( 0x0 );  /* Read Non-Volatile Storage select code */
        ci->setArg( 0, token.location );
        ci->setArg( 1, *reinterpret_cast<const u16 *>(byteArray) );
        executeWithPassword(ci, 2, password);
    }

    unsigned int SmiTokenDA::getStringLength() const
    {
        // pretend all SMI tokens are one word
        return 2;
    }

    std::ostream & SmiTokenDA::streamify( std::ostream & cout ) const
    {
        std::ios::fmtflags old_opts = cout.flags ();

        cout << hex << setfill('0');
        cout << "DMI type 0x"       << setw(2) << static_cast<int>(structure.type);
        cout << "  Handle 0x"       << setw(4) << static_cast<int>(structure.handle);
        cout << "  CmdIO Port 0x"  << setw(4) << static_cast<int>(structure.cmdIOAddress);
        cout << "  CmdIO Code 0x"  << setw(2) << static_cast<int>(structure.cmdIOCode);
        cout << "  Type 0x"         << setw(4) << static_cast<int>(getType());
        cout << "  Location 0x"     << setw(4) << static_cast<int>(token.location);
        cout << " value "           << setw(4) << static_cast<int>(token.value);

        cout.flags (old_opts);

        return cout;
    }

}
