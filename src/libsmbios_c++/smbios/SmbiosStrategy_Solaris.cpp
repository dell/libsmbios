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
#include <cstdio>

#include "smbios/IMemory.h"
#include "SmbiosImpl.h"

// message.h should be included last.
#include "smbios/message.h"

using namespace smbiosLowlevel;
using namespace std;

#if 1
#define EFIVARS_FILE_le266 "/proc/efi/systab"
#define EFIVARS_FILE_gt266 "/sys/firmware/efi/systab"
#else
// for debugging
#define EFIVARS_FILE_le266 "/home/michael_e_brown/cc/libsmbios_proj/libsmbios/foo.txt"
#define EFIVARS_FILE_gt266 "/home/michael_e_brown/cc/libsmbios_proj/libsmbios/foo.txt"
#endif

namespace smbios
{
    // allocates no memory, constructs no objects.
    // can raise an exception
    void SmbiosLinuxEFIStrategy::getSmbiosTableHeader(smbiosLowlevel::smbios_table_entry_point *table_header, bool strict)
    {
        ParseExceptionImpl parseException;
            parseException.setMessageString(_("EFI support not found"));

        FILE *fh = NULL;
        if ( (fh=fopen(EFIVARS_FILE_le266, "r")) == NULL  &&
             (fh=fopen(EFIVARS_FILE_gt266, "r")) == NULL)
            throw(parseException);

        DCERR("Found EFI systab. Reading offset..." << endl);

        // read lines
        char line[256] = {0,};
        while(NULL != fgets(line, sizeof(line)-1, fh))
        {
            char *varName=line;
            char *varValue=line;
            varValue = strchr(line, '=');
            if(!varValue)
                continue;

            *(varValue++) = '\0';
            if (0 == strcmp(varName, "SMBIOS"))
            {
                // offset is in parent class and locks down header
                // to only search at specified offset
                offset = strtol(varValue, NULL, 0);
                DCERR("Found SMBIOS address: " << hex << offset << "." << endl);
            }
        }
        fclose(fh);

        if(offset)
            SmbiosMemoryStrategy::getSmbiosTableHeader(table_header, strict);
        else
            throw(parseException);

        DCERR("Parsed SMBIOS table." << endl);
    }
}
