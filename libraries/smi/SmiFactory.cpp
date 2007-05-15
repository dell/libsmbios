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
        virtual std::auto_ptr<ISmi> makeNew(u8 type); // use me
    protected:
        static ISmi *_cmosPtr;
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

    std::auto_ptr<ISmi> SmiFactoryImpl::makeNew( u8 type )
    {
        ISmi *ret = 0;
        SmiStrategy *strategyPtr = 0;

        if (mode == AutoDetectMode )
            strategyPtr = new SmiArchStrategy();

        else if (mode == UnitTestMode)
            strategyPtr = new SmiMockStrategy(getParameterString("smiFile"));


        // used for automatic setup of magic io stuff 
        u16 cmdIOAddress = 0;
        u8  cmdIOCode = 0;
        smbios::TokenTableFactory *ttFactory = 0;
        smbios::ITokenTable *tokenTable = 0;

        switch( type )
        {
        case DELL_CALLING_INTERFACE_SMI_RAW:
            ret = new DellCallingInterfaceSmiImpl(strategyPtr);
            break;

        case DELL_CALLING_INTERFACE_SMI:
            ret = new DellCallingInterfaceSmiImpl(strategyPtr);
            // automatically set up cmd io port/magic
            // step 1: get token table
            ttFactory = smbios::TokenTableFactory::getFactory() ;
            tokenTable = ttFactory->getSingleton();
            // step 2: iterate through token table
            for(
                smbios::ITokenTable::iterator token = tokenTable->begin();
                token != tokenTable->end();
                ++token )
            {
                // Step 3: go until we get to the first one that will dynamic cast 
                //         into a TokenDA
                try{
                    if( token->getTokenClass() != "TokenDA" )
                        continue;

                    // Step 4: then set cmd io stuff
                    dynamic_cast<smbios::ISmiToken *>(&*token)->getSmiDetails(&cmdIOAddress, &cmdIOCode, static_cast<u8*>(0));
                    ret->setCommandIOMagic( cmdIOAddress, cmdIOCode );
                    break;
                } catch(...){
                }
            }

            if( ! (cmdIOAddress && cmdIOCode))
                throw SmiExceptionImpl(_("Could not automatically setup up magic io"));

            break;
        default:
            throw InvalidSmiModeImpl(_("Unknown smi factory mode requested"));
            break;
        }

        if( ! ret )
            throw InvalidSmiModeImpl(_("Could not allocate SMI object"));

        std::auto_ptr<ISmi> foo(ret);
        return foo;
    }

}
