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

#define LIBSMBIOS_SOURCE
#include "SmiImpl.h"
#include "FactoryImpl2.h"
#include "TokenImpl.h"

// message.h should be included last.
#include "smbios/message.h"

using namespace std;

namespace smi
{
    class SmiFactoryImpl : public factory::TFactory<SmiFactory>
    {
    public:
        SmiFactoryImpl() { setParameter("smiFile", ""); };
        virtual ~SmiFactoryImpl() throw() {};
        virtual std::auto_ptr<IDellCallingInterfaceSmi> makeNew(u8 type); // use me
    };

    //
    SmiFactory::~SmiFactory() throw()
    {}
    SmiFactory::SmiFactory()
    {}

    SmiFactory *SmiFactory::getFactory()
    {
        // reinterpret_cast<...>(0) to ensure template parameter is correct
        // this is a workaround for VC6 which cannot use explicit member template
        // function initialization.
        return SmiFactoryImpl::getFactory(reinterpret_cast<SmiFactoryImpl *>(0));
    }

    std::auto_ptr<IDellCallingInterfaceSmi> SmiFactoryImpl::makeNew( u8 type )
    {
        IDellCallingInterfaceSmi *ret = 0;
        SmiStrategy *strategyPtr = 0;

        if (mode == AutoDetectMode )
            strategyPtr = new SmiArchStrategy();

        else if (mode == UnitTestMode)
            strategyPtr = new SmiMockStrategy(getParameterString("smiFile"));


        // used for automatic setup of magic io stuff 
        u16 cmdIOAddress = 0;
        u8  cmdIOCode = 0;
        const smbios::ISmbiosTable *table = 0;

        switch( type )
        {
        case DELL_CALLING_INTERFACE_SMI_RAW:
            ret = new DellCallingInterfaceSmiImpl(strategyPtr, 0, 0);
            break;

        case DELL_CALLING_INTERFACE_SMI:
            // would be better to just get a 0xDA type from smbios table and get code/io
            try {
                table = smbios::SmbiosFactory::getFactory()->getSingleton();
                smbios::ISmbiosTable::const_iterator item = (*table)[0xDA];
                cmdIOAddress = getU16_FromItem(*item, 4);
                cmdIOCode = getU8_FromItem(*item, 6);
                ret = new DellCallingInterfaceSmiImpl(strategyPtr, cmdIOAddress, cmdIOCode);
            } catch(...) {
                delete strategyPtr;
                throw SmiExceptionImpl(_("Could not automatically setup up magic io"));
            }

            break;
        default:
            delete strategyPtr;
            throw InvalidSmiModeImpl(_("Unknown smi factory mode requested"));
            break;
        }

        if( ! ret )
        {
            delete strategyPtr;
            throw InvalidSmiModeImpl(_("Could not allocate SMI object"));
        }

        std::auto_ptr<IDellCallingInterfaceSmi> foo(ret);
        return foo;
    }

}
