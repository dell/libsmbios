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
#include "smbios/compat.h"

#include <string.h>

#include "SmbiosWorkaroundImpl.h"
#include "smbios/SmbiosDefs.h"
#include "SmbiosImpl.h"

#include "StdWorkarounds.h"

// message.h should be included last.
#include "smbios/message.h"

using namespace std;

// convenience function.
#define _XXX( expr ) do{ try {  expr } catch( const std::exception & ){} }while(0)

namespace smbios
{
    SmbiosWorkaroundTable   *SmbiosWorkaroundFactory::_tableInstance = 0;

    factory::TFactory<smbios::SmbiosWorkaroundFactory> *SmbiosWorkaroundFactory::getFactory()
    {
        // reinterpret_cast<...>(0) to ensure template parameter is correct
        // this is a workaround for VC6 which cannot use explicit member template
        // function initialization.
        return factory::TFactory<SmbiosWorkaroundFactory>::getFactory(reinterpret_cast<factory::TFactory<SmbiosWorkaroundFactory> *>(0));
    }

    SmbiosWorkaroundFactory::~SmbiosWorkaroundFactory() throw()
    {
        if( _tableInstance )
        {
            delete _tableInstance;
            _tableInstance = 0;
        }

    }

    SmbiosWorkaroundTable *SmbiosWorkaroundFactory::makeNew( const ISmbiosTable *table )
    {
        int systemId = 0;

        _XXX( systemId = getU8_FromItem( *(*table)[ Dell_Revisions_and_IDs ], 0x06 ); );
        if( 0xFE == systemId )
            _XXX(systemId = getU16_FromItem(*(*table)[ Dell_Revisions_and_IDs ], 0x08 ););

        const char * chBiosVersion = 0;

        std::string biosVersion = "";

        _XXX( chBiosVersion = getString_FromItem(*(*table)[ BIOS_Information ], 0x05 ); );

        if ( 0 != chBiosVersion )
            biosVersion = chBiosVersion;

        const Workaround **thisSystemWA = 0;
        for( int i=0; i < numSystemWorkarounds; ++i )
        {
            if( workaroundMasterList[i].system->systemId == systemId )
            {
                thisSystemWA = workaroundMasterList[i].workarounds;
                break;
            }
        }

        return new SmbiosWorkaroundTable( table, thisSystemWA );
    }

    SmbiosWorkaroundTable::SmbiosWorkaroundTable( const ISmbiosTable *, const Workaround **initWorkarounds )
            : systemId(0), biosVersion(""), workaroundsForThisSystem(initWorkarounds)
    {}

    SmbiosWorkaroundTable::~SmbiosWorkaroundTable()
    {}

    static bool compare( int size, const ISmbiosItem *item, int offset, datatron data )
    {
        bool retval = false;
        u8 *cmpPtr = new u8[size];
    
        try
        {
            item->getData(offset, cmpPtr, size);
            if(0 == memcmp(cmpPtr, data.data, size))
                    retval = true;
        }
        catch(...)
        {
            delete [] cmpPtr;
            cmpPtr = 0;
            throw;
        }

        delete [] cmpPtr;
        cmpPtr = 0;

        return retval;
    }

    static void fixupData( u8 *buf, size_t size, unsigned int offset, unsigned int len, datatron data )
    {
        InternalErrorImpl internalError;
        if( offset > size )
        {
            internalError.setMessageString(_("Data overflow. Offset requested is larger than data size. offset: %(offset)i, data size: %(size)i"));
            internalError.setParameter("offset",offset);
            internalError.setParameter("size",static_cast<int>(size));
            throw internalError;
        }

        memcpy(&(buf[offset]), data.data, len);
        //for(unsigned int i=0; i<len; i++)
            //buf[offset + i] = data.data[i];
    }

    static void doFixupItem( const Workaround *wa, const ISmbiosItem *item, u8 *buf, size_t bufSize )
    {
        int i = 0;  //loop counter. workaround MSVC++ braindamage.

        // check all of the symptoms. If any symptom does not match, bail.
        for( i=0; 0!=wa->symptoms[i].type; ++i )
        {
            bool ret = compare( wa->symptoms[i].fieldDataType, item, wa->symptoms[i].fieldOffset, wa->symptoms[i].data );
            if( ! ret )
                goto out;
        }

        // All symptoms present if we got here.
        //cout << "GOT HERE!" << flush;

        // apply all of the fixups.
        for( i=0; 0!=wa->fixups[i].type; ++i )
        {
            fixupData(buf, bufSize, wa->fixups[i].fieldOffset, wa->fixups[i].fieldDataType, wa->fixups[i].data );
        }

out:
        return;
    }

    void SmbiosWorkaroundTable::fixupItem( const ISmbiosItem *item, u8 *buffer, size_t bufSize ) const
    {
        int i = 0; //declare i up here to work around braindamaged MSVC++ for()
        // scoping violation.

        if( 0 == workaroundsForThisSystem )
            goto out;

        // workaroundsForThisSystem is a NULL-terminated array
        // of Workaround pointers.
        for( i=0; 0 != workaroundsForThisSystem[i]; ++i )
        {
            if( workaroundsForThisSystem[i]->symptoms->type  == item->getType() )
            {
                //cout << "-F-" << flush;
                doFixupItem( workaroundsForThisSystem[i], item, buffer, bufSize );
            }
        }

out:
        return;
    }

}
