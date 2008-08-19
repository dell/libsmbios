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
    CmosTokenD6::CmosTokenD6( const smbios::ISmbiosItem &initItem, std::vector< CmosRWChecksumObserver > &initChecksumList)
            : CmosTokenD5(initItem, initChecksumList)
    {
        size_t size;
        const u8 *ptr =  item->getBufferCopy(size) ; // MUST DELETE[]!
        size = size < sizeof(structure)? size : sizeof(structure);
        memcpy( const_cast<dell_protected_value_2_structure*>(&structure), ptr, size );

        // have to copy into parents "structure"
        size = size < sizeof(CmosTokenD5::structure)? size : sizeof(CmosTokenD5::structure);
        memcpy(
            const_cast<dell_protected_value_1_structure*>(&(CmosTokenD5::structure)),
            ptr,
            sizeof(CmosTokenD5::structure) );

        delete [] const_cast<u8 *>(ptr); //const_cast to fix msvc++
    }

    string CmosTokenD6::getTokenClass() const
    {
        return "TokenD6";
    }

    void CmosTokenD6::addChecksumObserver() const
    {
        // run superclass to get observer for value check
        CmosTokenD5::addChecksumObserver();


        // add an observer for the checked range
        ostringstream ost;
        ost << *item;

        CmosRWChecksumObserver chk(
            ost.str(),
            cmos,
            structure.rangeCheckType,
            structure.indexPort,
            structure.dataPort,
            structure.rangeCheckStart,
            structure.rangeCheckEnd,
            structure.rangeCheckIndex  );

        checksumList.push_back( chk );
    }
}
