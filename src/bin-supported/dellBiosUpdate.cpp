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
#include "smbios/compat.h"

#include <iostream>
#include <string.h>

#include "smbios/DellRbu.h"
#include "smbios/SystemInfo.h"
#include "smbios/RbuLowLevel.h"
#include "getopts.h"

// always include last if included.
#include "smbios/message.h"  // not needed outside of this lib. (mainly for gettext i18n)

using namespace std;

struct options opts[] =
    {
        { 1, "hdr", "BIOS update file (.HDR file)", "f", 1 },
        { 2, "cancel", "Cancel a previously-scheduled BIOS update", "c", 0 },
        { 3, "force_packet", "Force update type to be packet-based", NULL, 0 },
        { 4, "force_mono", "Force update type to be monolithic-based", NULL, 0 },
        { 5, "auto_detect", "Auto-detect update type", NULL, 0 },
        { 6, "info", "Dump BIOS update .HDR file info. Does not perform BIOS update", "i", 0 },
        { 7, "override_sysid", "Attempt BIOS update even if .HDR file does not appear to match this system", NULL, 0 },
        { 8, "override_bios_version", "Attempt BIOS update even if .HDR file is older than current system BIOS", NULL, 0 },
        { 9, "update", "Schedule BIOS update", "u", 0 },
        { 10, "test", "Test this HDR file to see if it is appropriate for this system.", "t", 0 },
        { 11, "i_know_what_i_am_doing", "don't use this option... :-)", NULL, 0 },
        { 255, "version", "Display libsmbios version information", "v", 0 },
        { 0, NULL, NULL, NULL, 0 }
    };

class noHdrFile : public exception {};

namespace rbu{
extern driver_type getDriverType();
extern packet_type getSupportedPacketType();
}

int
main (int argc, char **argv)
{
    int retval = 0;
    rbu::IRbuHdr *hdr = 0;
    try
    {
        int c=0;
        char *args = 0;

        string fileName("");
        enum { none, test, update_bios, cancel_update, hdr_info } action = none;
        rbu::packet_type force_type = rbu::pt_any;
        int override = 0;
        bool i_know_what_i_am_doing=false;

        while ( (c=getopts(argc, argv, opts, &args)) != 0 )
        {
            switch(c)
            {
            case 1:
                fileName = args;
                break;
            case 2:
                action = cancel_update; 
                break;
            case 9:
                action = update_bios; 
                break;
            case 3:
                force_type = rbu::pt_packet;
                break;
            case 4:
                force_type = rbu::pt_mono;
                break;
            case 5:
                force_type = rbu::pt_any;  // auto-detect
                break;
            case 6:
                action = hdr_info;
                break;
            case 7:
                override |= rbu::SYSID_OVERRIDE;
                break;
            case 8:
                override |= rbu::BIOSVER_OVERRIDE;
                break;
            case 10:
                action = test;
                break;
            case 11:
                i_know_what_i_am_doing = true;
                break;
            case 255:
                cout << "Libsmbios version:    " << SMBIOSGetLibraryVersionString() << endl;
                exit(0);
                break;
            default:
                break;
            }
            free(args);
        }

        // Instantiate RBU HDR object if it is going to be needed.
        if (action == update_bios || action == test || action == hdr_info)
        {
            if( ! strlen(fileName.c_str()) )
                throw noHdrFile();

            hdr = rbu::RbuFactory::getFactory()->makeNew(fileName);
        }

        // validation for bios update
        if (action == update_bios || action == test)
        {

#if 0
            rbu::driver_type dt = rbu::getDriverType();
            rbu::packet_type supported_pt = rbu::getSupportedPacketType();
            if ((force_type == rbu::pt_packet || (force_type == rbu::pt_any && supported_pt == rbu::pt_packet)))
            {
                cout << endl;
                cout << "WARNING: packet updates are not fully tested yet." << endl;
                cout << "         We recommend that only monlithic updates be used at this point." << endl;
                cout << "         You should only use packet mode if you know what you are " << endl;
                cout << "         doing (for example, testing.)" << endl << endl;
                cout << "         Let me know if it does work so I can remove this blacklist entry." << endl;
                cout << endl;
                if(! i_know_what_i_am_doing)
                {
                    cout << "Forcing MONOLITHIC mode..." << endl << endl;
                    force_type = rbu::pt_mono;
                }
            }
#endif

            // check system id in HDR against this system ID
            //          --override_sysid_check to override
            if (!rbu::checkSystemId(*hdr, SMBIOSGetDellSystemId()))
            {
                cout << "WARNING: BIOS HDR file does not contain support for this system." << endl;
                cout << "         This may result in bad things happening!" << endl;
                if (override & rbu::SYSID_OVERRIDE) 
                {
                    cout << "         Override detected, continuing anyway..." << endl;
                }
                else
                {
                    cout << "         Exiting..." << endl;
                    retval = 1;
                    goto out;
                }
            }

            // TODO: check bios version, don't allow backrev
            //          --override_ver_check to override
            string hdrBiosVer = hdr->getBiosVersion();
            const char *tmpSysBiosVer = SMBIOSGetBiosVersion();
            string sysBiosVer(tmpSysBiosVer);
            SMBIOSFreeMemory(tmpSysBiosVer);
            int cmp = rbu::compareBiosVersion(sysBiosVer, hdrBiosVer);
            if ( cmp <= 0 )
            {
                cout << "WARNING: BIOS HDR file BIOS version appears to be less than or equal to current BIOS version." << endl;
                cout << "         This may result in bad things happening!" << endl;
                if (override & rbu::BIOSVER_OVERRIDE) 
                {
                    cout << "         Override detected, continuing anyway..." << endl;
                }
                else
                {
                    cout << "         Exiting..." << endl;
                    retval = 1;
                    goto out;
                }
            }
        }

        switch(action)
        {
        case update_bios:
            rbu::dellBiosUpdate(*hdr, force_type);
            break;
        case cancel_update:
            rbu::cancelDellBiosUpdate();
            break;
        case hdr_info:
            cout << *hdr << endl;
            break;
        case test:
            cout << "BIOS file matches this system and is newer." << endl;
            break;
        case none:
            cout << "Missing action. Please specify: Cancel (-c), Update (-u), or Info (-i)" << endl;
            break;
        }
    }
    catch( const noHdrFile & )
    {
        cerr << endl;
        cerr << "You must specify a BIOS .HDR file to use. See help for details." << endl;
        cerr << endl;
        retval = 1;
    }
    catch( const rbu::InvalidHdrFile &e)
    {
        cout << endl;
        cout << "HDR file is not valid: " << e.what() << endl;
        cout << endl;
        retval = 1;
    }
    catch( const rbu::HdrFileIOError &e )
    {
        cout << endl;
        cout << "IO Error reading HDR File: " << e.what() << endl;
        cout << endl;
        retval = 1;
    }
    catch( const exception &e )
    {
        cerr << endl;
        cerr << "An Error occurred. The Error message is: " << endl;
        cerr << "    " << e.what() << endl;
        cerr << endl;
        cerr << "Problem updating BIOS. Common problems are:" << endl;
        cerr << endl;
        cerr << "    -- Insufficient permissions to perform operation." << endl;
        cerr << "       Try running as a more privileged account." << endl;
        cerr << "          Linux  : run as 'root' user" << endl;
        cerr << "          Windows: run as 'administrator' user" << endl;
        cerr << endl;
        cerr << "    -- dell_rbu device driver not loaded." << endl;
        cerr << "       Try loading the dell_rbu driver" << endl;
        cerr << "          Linux  : modprobe dell_rbu" << endl;
        cerr << "          Windows: dell_rbu driver not yet available." << endl;
        cerr << endl;

        retval = 1;
    }
    catch ( ... )
    {
        cerr << endl;
        cerr << "An Unknown Error occurred. Aborting." << endl;
        cerr << endl;
        retval = 2;
    }

out:
    delete hdr;
    return retval;
}
