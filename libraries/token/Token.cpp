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

#include "TokenImpl.h"

using namespace std;

namespace smbios
{
    IToken::IToken()
    {}

    IProtectedToken::IProtectedToken()
    {}

    ICmosToken::ICmosToken()
    {}

    ISmiToken::ISmiToken()
    {}

    // no dynamically allocated memory, yay!
    IToken::~IToken()
    {}

    std::ostream & operator << (std::ostream & cout, const IToken & item)
    {
        return item.streamify(cout);
    }

    void activateToken(int tokenNum, string password)
    {
        smbios::TokenTableFactory *ttFactory = smbios::TokenTableFactory::getFactory() ;
        smbios::ITokenTable *tokenTable = ttFactory->getSingleton();
        try
        {
            smbios::IToken *token = &(*((*tokenTable)[ tokenNum ]));
            dynamic_cast< smbios::IProtectedToken * >(token)->tryPassword(password);
        } catch (...) { /* not an error if password fails */ }

        (*tokenTable)[ tokenNum ]->activate();
    }   
    
    bool isTokenActive(int token)
    {   
        smbios::TokenTableFactory *ttFactory = smbios::TokenTableFactory::getFactory() ;
        smbios::ITokenTable *tokenTable = ttFactory->getSingleton();
        return (*tokenTable)[ token ]->isActive();
    } 
}
