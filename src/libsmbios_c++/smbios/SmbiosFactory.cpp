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

#include <vector>

#include "SmbiosImpl.h"

// message.h should be included last.
#include "smbios/message.h"

using namespace std;

namespace smbios
{
    SmbiosFactory::~SmbiosFactory() throw()
    {}
    SmbiosFactory::SmbiosFactory()
    {}

    SmbiosFactory *SmbiosFactory::getFactory()
    {
        // reinterpret_cast<...>(0) to ensure template parameter is correct
        // this is a workaround for VC6 which cannot use explicit member template
        // funciton initialization.
        return SmbiosFactoryImpl::getFactory(reinterpret_cast<SmbiosFactoryImpl *>(0));
    }

    ISmbiosTable      *SmbiosFactoryImpl::_tableInstance = 0;

    SmbiosFactoryImpl::~SmbiosFactoryImpl() throw()
    {
        if( _tableInstance )
        {
            delete _tableInstance;
            _tableInstance = 0;
        }
    }

    SmbiosFactoryImpl::SmbiosFactoryImpl()
    {
        mode = defaultMode;
        setParameter( "strictValidation", 0 );
        setParameter( "offset", 0 );
    }

    ISmbiosTable *SmbiosFactoryImpl::getSingleton()
    {
        if (! _tableInstance)
            _tableInstance = makeNew();

        return _tableInstance;
    }

    ISmbiosTable *SmbiosFactoryImpl::makeNew()
    {
        // stupid, ugly hack to supress (C4800) warning on msvc++
        bool strict = getParameterNum("strictValidation") ? 1 : 0;

        SmbiosTable *table = 0;

        std::vector<SmbiosStrategy *> strategies;

        if (mode == AutoDetectMode)
        {
#ifdef LIBSMBIOS_PLATFORM_LINUX
            strategies.push_back( new SmbiosLinuxEFIStrategy() );
#endif
            strategies.push_back( new SmbiosMemoryStrategy(getParameterNum("offset")) );
#if (defined(LIBSMBIOS_PLATFORM_WIN32) || defined(LIBSMBIOS_PLATFORM_WIN64))
            strategies.push_back( new SmbiosWinGetFirmwareTableStrategy() );
            strategies.push_back( new SmbiosWinWMIStrategy() );
#endif
        }
        else if (mode == UnitTestMode)
        {
            strategies.push_back( new SmbiosMemoryStrategy(getParameterNum("offset")) );
        }
        else
        {
        throw NotImplementedImpl(_("Unknown smbios factory mode requested"));
        }


        table = new SmbiosTable(
                        strategies,
                        strict
                    );

        table->initializeWorkaround();
        return table;
    }
}
