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
#include "CmosRWImpl.h"
#include "FactoryImpl2.h"

using namespace std;

namespace cmos
{
    class CmosRWFactoryImpl : public factory::TFactory<CmosRWFactory>
    {
    public:
        CmosRWFactoryImpl() { setParameter("cmosMapFile", ""); };
        virtual ~CmosRWFactoryImpl() throw ();
        virtual ICmosRW *getSingleton( ); // returns singleton
        virtual ICmosRW *makeNew( ); // not for use
    protected:
        static ICmosRW *_cmosPtr;
    };

    //
    CmosRWFactory::~CmosRWFactory() throw()
    {}
    CmosRWFactory::CmosRWFactory()
    {}

    ICmosRW           *CmosRWFactoryImpl::_cmosPtr  = 0;

    CmosRWFactoryImpl::~CmosRWFactoryImpl() throw()
    {
        if( _cmosPtr )
        {
            delete _cmosPtr;
        }
        _cmosPtr = 0;
    }

    CmosRWFactory *CmosRWFactory::getFactory()
    {
        // reinterpret_cast<...>(0) to ensure template parameter is correct
        // this is a workaround for VC6 which cannot use explicit member template
        // funciton initialization.
        return CmosRWFactoryImpl::getFactory(reinterpret_cast<CmosRWFactoryImpl *>(0));
    }

    ICmosRW *CmosRWFactoryImpl::getSingleton()
    {
        if (_cmosPtr == 0)
            _cmosPtr = makeNew();

        return _cmosPtr;
    }

    ICmosRW *CmosRWFactoryImpl::makeNew()
    {
        ICmosRW *ret=0;
        if (mode == UnitTestMode)
        {
            ret = new CmosRWFile( getParameterString("cmosMapFile") );
        }
        else if ( mode == AutoDetectMode )
        {
            ret = new CmosRWIo( );
        }
        else
        {
            throw InvalidCmosRWModeImpl("CmosRW Factory has been set to an invalid mode.");
        }
        return ret;
    }

}
