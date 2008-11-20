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

#include <sstream>
#include <string.h>

#include "smbios/IMemory.h"
#include "SmbiosImpl.h"

// message.h should be included last.
#include "smbios/message.h"

using namespace smbiosLowlevel;
using namespace std;

namespace smbios
{

    // validate the smbios table entry point
    bool validateDMITableEntryPoint(
        const smbiosLowlevel::dmi_table_entry_point *tempTEP,
        bool strict,
        ParseExceptionImpl &parseException
        )
    {
        // This code checks for the following:
        //       entry point structure checksum : As per the specs
        //       anchor string : As per the specs
        //
        bool retval = true;

        u8 checksum = 0;
        const u8 *ptr = reinterpret_cast<const u8*>(tempTEP);
        // don't overrun tempTEP if BIOS is buggy... (note sizeof() test here)
        //      added especially to deal with buggy Intel BIOS.
        for( unsigned int i = 0; i < sizeof(*tempTEP); ++i )
        {
            // stupid stuff to avoid MVC++ .NET runtime exception check for cast to different size
            checksum = (checksum + ptr[i]) & 0xFF;
        }

        ostringstream oss;

        DCERR("_DMI_ anchor: " << tempTEP->anchor[0] << tempTEP->anchor[1] << tempTEP->anchor[2] << tempTEP->anchor[3] << tempTEP->anchor[4] << tempTEP->anchor[5] << endl);
        if(memcmp(tempTEP->anchor,"_DMI_",5)!=0) // Checking intermediate anchor string
        {
            oss << _("Intermediate anchor string does not match. anchor string: %(dmi_anchor)s") << endl;
            retval = false;  // validation failed
        }

        DCERR("_DMI_ checksum: " << (int)checksum << endl);
        if(checksum) // Checking entry point structure checksum
        {
            oss << _("Checksum check for table entry point should be zero. checksum: %(dmi_checksum)i ") << endl;
            retval = false;  // validation failed
        }

        parseException.setParameter("dmi_anchor", reinterpret_cast<const char *>(tempTEP->anchor));
        parseException.setParameter("dmi_checksum", static_cast<int>(checksum));

        return retval;
    }




    // validate the smbios table entry point
    bool validateSmbiosTableEntryPoint(
        const smbiosLowlevel::smbios_table_entry_point *tempTEP,
        bool strict,
        ParseExceptionImpl &parseException
        )
    {
        // This code checks for the following:
        //       entry point structure checksum : As per the specs
        //       smbios major version : As per the specs
        //       Intermediate anchor string : As per the specs
        //
        // This code does not check the following:
        //      intermediate checksum: the main checksum covers the
        //      entire area
        //          and should be sufficient, plus there is a
        //          possibility for
        //          BIOS bugs in this area.
        //
        //      minor version: according to the spec, this parser should
        //      work
        //          with any change in minor version. The spec says this
        //          parser
        //          will break if major version changes, so we check
        //          that.
        //

        bool retval = true;

        u8 checksum = 0;
        const u8 *ptr = reinterpret_cast<const u8*>(tempTEP);
        // don't overrun tempTEP if BIOS is buggy... (note sizeof() test here)
        //      added especially to deal with buggy Intel BIOS.
        for( unsigned int i = 0; (i < static_cast<unsigned int>(tempTEP->eps_length)) && (i < sizeof(*tempTEP)); ++i )
        {
            // stupid stuff to avoid MVC++ .NET runtime exception check for cast to different size
            checksum = (checksum + ptr[i]) & 0xFF;
        }

        ostringstream oss;
        oss << _("validation of table entry point failed") << endl;

        validateDMITableEntryPoint( &(tempTEP->dmi), strict, parseException );

        DCERR("strict table checking: " << strict << endl);


        DCERR("_SM_ checksum: " << (int)checksum << endl);

        if(checksum) // Checking entry point structure checksum
        {
            oss << _("Checksum check for table entry point should be zero. checksum: %(checksum)i ") << endl;
            retval = false;  // validation failed
        }

        DCERR("major_ver: " << (int)tempTEP->major_ver << endl);
        if(tempTEP->major_ver!=0x02)     // Checking smbios major version
        {
            oss << _("Major version of table entry point should be 2: %(major_version)i") << endl;
            retval = false;  // validation failed
        }

        // Entry Point Length field is at least 0x1f.
        DCERR("eps_length: " << (int)tempTEP->eps_length << endl);
        if(tempTEP->eps_length < 0x0f)
        {
            oss << _("Entry Point Length field is at least 0x1f : %(eps_length)i") << endl;
            retval = false;  // validation failed
        }

        parseException.setParameter("checksum", static_cast<int>(checksum));
        parseException.setParameter("major_version", static_cast<int>(tempTEP->major_ver));
        parseException.setParameter("eps_length", static_cast<int>(tempTEP->eps_length));
        parseException.setMessageString(oss.str());

        return retval;
    }





    bool SmbiosMemoryStrategy::getSmbiosTable(const u8 **smbiosBuffer, smbiosLowlevel::smbios_table_entry_point *table_header, bool strict)
    {
        bool ret = false;
        try
        {
            // allocates no mem
            DCERR("trying SmbiosMemoryStrategy" << endl);
            getSmbiosTableHeader(table_header, strict);

            // allocates mem, but frees on exception
            getSmbiosTableBuf(smbiosBuffer, *table_header);
            if(smbiosBuffer)
                    ret = true;
        }
        catch( MARK_UNUSED const exception &e)
        {
            DCERR("got Exception: " << e.what() << endl);
        }

        DCERR("  ret for SmbiosMemoryStrategy is: " << ret << endl);
        return ret;
    }

    void SmbiosMemoryStrategy::getSmbiosTableBuf(const u8 **smbiosBuffer, smbiosLowlevel::smbios_table_entry_point table_header)
    {
        memory::IMemory *mem = memory::MemoryFactory::getFactory()->getSingleton();

        // new throws exception, no need to test.
        u8 *newSmbiosBuffer = new u8[table_header.dmi.table_length];
        try
        {
            mem->fillBuffer( newSmbiosBuffer, table_header.dmi.table_address, table_header.dmi.table_length );

            //delete old one, if necessary
            if( 0 != *smbiosBuffer )
            {
                memset (const_cast<u8 *>(*smbiosBuffer), 0, sizeof (**smbiosBuffer));
                delete [] const_cast<u8 *>(*smbiosBuffer);
                *smbiosBuffer = 0;
            }
        }
        catch(...)
        {
            delete [] newSmbiosBuffer;
            newSmbiosBuffer = 0;
            throw;
        }

        *smbiosBuffer = reinterpret_cast<const u8 *>(newSmbiosBuffer);
    }

    // allocates no memory, constructs no objects.
    // can raise an exception
    void SmbiosMemoryStrategy::getSmbiosTableHeader(smbiosLowlevel::smbios_table_entry_point *table_header, bool strict)
    {
        memory::IMemory *mem = memory::MemoryFactory::getFactory()->getSingleton();

        unsigned long fp = E_BLOCK_START;
        if( offset )
            fp = offset;

        ParseExceptionImpl parseException;
        if( offset )
        {
            DCERR("SmbiosMemoryStrategy::getSmbiosTableHeader() using hardcoded offset: " << hex << offset << endl);
            parseException.setMessageString(_("SMBIOS Header not found at offset: %(offsetValue)i"));
            parseException.setParameter("offsetValue",offset);
        }
        else
        {
            DCERR("SmbiosMemoryStrategy::getSmbiosTableHeader() Memory scan for smbios table." << endl);
            parseException.setMessageString(_("SMBIOS Header not found in search."));
        }

        // tell the memory subsystem that it can optimize here and 
        // keep memory open while we scan rather than open/close/open/close/... 
        // for each fillBuffer() call
        // 
        // this would be safer if we used spiffy c++ raii technique here
        mem->decReopenHint();

        smbios_table_entry_point tempTEP;
        memset(&tempTEP, 0, sizeof(tempTEP));
        while ( (fp + sizeof(tempTEP)) < F_BLOCK_END)
        {
            mem->fillBuffer(
                reinterpret_cast<u8 *>(&tempTEP),
                fp,
                sizeof(tempTEP)
            );

            // search for promising looking headers
            // first, look for old-style DMI header
            if (memcmp (&tempTEP, "_DMI_", 5) == 0)
            {
                DCERR("Found _DMI_ anchor. Trying to parse legacy DMI structure." << endl);
                dmi_table_entry_point *dmiTEP = reinterpret_cast<dmi_table_entry_point *>(&tempTEP);
                memmove(&(tempTEP.dmi), &dmiTEP, sizeof(dmi_table_entry_point));
                // fake the rest of the smbios table entry point...
                tempTEP.major_ver=2;
                tempTEP.minor_ver=0;
                if(validateDMITableEntryPoint(dmiTEP, strict, parseException))
                {
                    DCERR("Found valid _DMI_ entry point at offset: " << fp << endl);
                    break;
                }
            }

            // then, look for new-style smbios header. This will always
            // occur before _DMI_ in memory
            if (offset || (memcmp (&tempTEP, "_SM_", 4) == 0))
            {
                // if we are passed a hardcoded offset (EFI?), it is possible
                // that machine doesnt have _SM_ anchor, but still has valid
                // table. Just try validating the table and skip _SM_ check.
                DCERR("Found _SM_ anchor or using hardcoded offset. Trying to parse Smbios Entry Point." << endl);
                if(validateSmbiosTableEntryPoint(&tempTEP, strict, parseException))
                {
                    DCERR("Found valid _SM_ entry point at offset: " << fp << endl);
                    break;
                }
            }

            // previous if() would have broken out if we have a valid
            // table header. if offset is set, then we are not supposed
            // to be scanning through memory. We didn't find a table,
            // so there is nothing to do but raise an exception.
            if (offset)
            {
                // dont need memory optimization anymore
                mem->incReopenHint();
                throw parseException; // previously set up.
            }

            fp += 16;
        }

        // dont need memory optimization anymore
        mem->incReopenHint();

        // bad stuff happened if we got to here and fp > 0xFFFFFL
        if ((fp + sizeof(tempTEP)) >= F_BLOCK_END)
            throw parseException; // previously set up.

        // found it. set offset for future reference (no need to search.)
        offset = fp;
        memcpy( const_cast<smbios_table_entry_point *>(table_header), &tempTEP, sizeof(*table_header) );
    }
}
