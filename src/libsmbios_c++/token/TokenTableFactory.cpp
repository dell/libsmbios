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
#include "TokenImpl.h"
#include "FactoryImpl2.h"

using namespace std;

namespace smbios
{
    class TokenTableFactoryImpl : public factory::TFactory<TokenTableFactory>
    {
    public:
        TokenTableFactoryImpl() {};
        virtual ~TokenTableFactoryImpl() throw();
        virtual ITokenTable *getSingleton( const smbios::ISmbiosTable *table = 0 );
        virtual ITokenTable *makeNew( const smbios::ISmbiosTable *table );
    protected:
        static ITokenTable *_tablePtr;
    };

    TokenTableFactory::TokenTableFactory()
    {}
    
    TokenTableFactory::~TokenTableFactory() throw()
    {}
    

    TokenTableFactory *TokenTableFactory::getFactory()
    {
        // reinterpret_cast<...>(0) to ensure template parameter is correct
        // this is a workaround for VC6 which cannot use explicit member template
        // function initialization.
        return TokenTableFactoryImpl::getFactory(reinterpret_cast<TokenTableFactoryImpl *>(0));
    }

    ITokenTable           *TokenTableFactoryImpl::_tablePtr = 0;

    TokenTableFactoryImpl::~TokenTableFactoryImpl() throw()
    {
        if( _tablePtr )
        {
            delete _tablePtr;
        }
        _tablePtr = 0;
    }

    ITokenTable *TokenTableFactoryImpl::getSingleton(const ISmbiosTable *table)
    {
        if( !table )
        {
            table = smbios::SmbiosFactory::getFactory()->getSingleton();
        }

        if (! _tablePtr)
            _tablePtr =  makeNew(table);

        return _tablePtr;
    }


    ITokenTable *TokenTableFactoryImpl::makeNew(const ISmbiosTable *table)
    {
        if ((mode == AutoDetectMode) || (mode == UnitTestMode))
        {
            return new TokenTable( *table );
        }
        else
        {
            throw InvalidTokenTableModeImpl();
        }
    }

}
