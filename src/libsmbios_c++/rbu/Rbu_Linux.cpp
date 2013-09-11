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

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <unistd.h>
#include <cstdio>

#include "RbuImpl.h"

// always include last if included.
#include "smbios/message.h"  // not needed outside of this lib. (mainly for gettext i18n)

using namespace std;

namespace rbu
{
const char *rbu_v0_type_file = "/proc/dell/rbu/image_type";
const char *rbu_v0_data_file = "/proc/dell/rbu/rbudata";
const char *rbu_v0_size_file = "/proc/dell/rbu/rbudatasize";
    
const char *rbu_v1_mono_data_file = "/sys/firmware/rbu/rbudata";
const char *rbu_v1_mono_size_file = "/sys/firmware/rbu/rbudatasize";
const char *rbu_v1_pkt_data_file = "/sys/firmware/rbu/packetdata";
const char *rbu_v1_pkt_size_file = "/sys/firmware/rbu/packetdatasize";

const char *rbu_v2_fw_data_file = "/sys/class/firmware/dell_rbu/data";
const char *rbu_v2_fw_load_file = "/sys/class/firmware/dell_rbu/loading";
const char *rbu_v2_img_type_file = "/sys/devices/platform/dell_rbu/image_type";
const char *rbu_v2_pkt_size_file = "/sys/devices/platform/dell_rbu/packet_size";

const int RBU_PACKET_SIZE = 4096;

    static size_t FWRITE(const void *ptr, size_t size, size_t nmemb, FILE *stream)
    {
        size_t written = fwrite(ptr, size, nmemb, stream); 
        // TODO: handle short write
        if (written < nmemb)
            throw RbuDriverIOErrorImpl("short write");
        return written;
    }

    driver_type getDriverType()
    {
        if (!access(rbu_v1_mono_data_file, F_OK))
            return rbu_linux_v1;
        else if (!access(rbu_v2_img_type_file, F_OK))
            return rbu_linux_v2;
        else if (!access(rbu_v0_data_file, F_OK))
            return rbu_linux_v0;
        else
            return rbu_unsupported;
    }

    static FILE * writePacket(const char *fn, const char *buffer, size_t bufSize, bool openclose)
    {
        static FILE *data_fh = 0;
        if(!data_fh)
            data_fh = fopen(fn, "wb");

        if (!data_fh)
            throw RbuDriverIOErrorImpl(strerror(errno));
    
        try
        {
            FWRITE(buffer, 1, bufSize, data_fh);
            if (ferror(data_fh))
                throw RbuDriverIOErrorImpl(strerror(errno));

            if(openclose)
            {
                fclose(data_fh);
                data_fh = 0;
            }
        }
        catch(...)
        {
            fclose(data_fh);
            throw;
        }

        fflush(NULL);
        return data_fh;
    }

    static void pktUpdateLoop(FILE *hdr_fh, const char *packetFilename, char *buffer, size_t bufSize, bool openclose)
    {
        cout << "Writing RBU data (" << bufSize << "bytes/dot): ";

        fseek(hdr_fh, 0, SEEK_END);
        size_t totalSizeBytes = ftell(hdr_fh);

        fseek(hdr_fh, 0, 0);
        // set up packet
        rbu_packet *pkt = reinterpret_cast<rbu_packet *>(buffer);
        createPacket(buffer, bufSize, totalSizeBytes);
    
        // TODO: password support.
        FILE *data_fh = writePacket(packetFilename, buffer, bufSize, openclose);
        cout << ".";
    
        while(!feof(hdr_fh))
        {
            ++pkt->pktNum;
            memset(&(pkt->pktData), 0, bufSize - sizeof(rbu_packet) + 1);
            size_t numBytes = fread(&(pkt->pktData), 1, bufSize - sizeof(rbu_packet) + 1, hdr_fh);
            UNREFERENCED_PARAMETER(numBytes);
            if (ferror(hdr_fh)) // should catch all errored short reads with this
                throw HdrFileIOErrorImpl(strerror(errno));
    
            checksumPacket(pkt, bufSize);
            writePacket(packetFilename, buffer, bufSize, openclose);
    
            cout << ".";
        }
        cout << endl;
    
        // close data file if it is still open
        if(data_fh)
        {
            fflush(NULL);
            fclose(data_fh);
        }
        cout << "Done writing packet data." << endl;
    }

    static void monoUpdateLoop(FILE *hdr_fh, FILE *data_fh)
    {
        fseek(hdr_fh, 0, 0);
        const int bufSize = 4096;
        char *buffer[bufSize];
        cout << "Writing RBU data (" << bufSize << "bytes/dot): ";
        while(!feof(hdr_fh))
        {
            memset(buffer, 0, bufSize);
            size_t readSz = fread(buffer, 1, bufSize, hdr_fh);
            if (ferror(hdr_fh))
                throw HdrFileIOErrorImpl(strerror(errno));
    
            FWRITE(buffer, 1, readSz, data_fh);
            if (ferror(data_fh))
                throw RbuDriverIOErrorImpl(strerror(errno));
            cout << "." << flush;
        }
        cout << endl;
    }


/****************************************
   RBU Linux v1 functions (older driver)
****************************************/
    static void setSize(const char *fn, size_t sz)
    {
        FILE *size_fh = fopen(fn, "wb");
        int saved_errno = 0;
        if (!size_fh)
            throw RbuDriverIOErrorImpl(strerror(errno));

        ostringstream ost("");
        ost << sz;
        cout << "writing (" << sz << ") to file: " << fn << endl;
        FWRITE(ost.str().c_str(), 1, ost.str().length(), size_fh);
        if (ferror(size_fh))
               saved_errno = errno;
        fclose(size_fh);
        size_fh = 0;

        if (saved_errno)
            throw RbuDriverIOErrorImpl(strerror(saved_errno));
    }

    static void doPacketUpdate_v1(FILE *hdr_fh) 
    {
        const size_t bufSize = RBU_PACKET_SIZE;
        char buffer[bufSize] = {0};
    
        // set packet size, reset mono handler
        setSize(rbu_v1_mono_size_file, 0);
        setSize(rbu_v1_pkt_size_file, bufSize);

        pktUpdateLoop(hdr_fh, rbu_v1_pkt_data_file, buffer, bufSize, true);
    }
    
    static void doMonoUpdate_v1(FILE *hdr_fh) 
    {
        cout << "Prep driver for data load." << endl;
    
        FILE *data_fh = fopen(rbu_v1_mono_data_file, "wb");
        if (!data_fh)
            throw RbuDriverIOErrorImpl(strerror(errno));
    
        fseek(hdr_fh, 0, SEEK_END);
        size_t totalSizeBytes = ftell(hdr_fh);

        // set mono data size, reset pkt handler
        setSize(rbu_v1_pkt_size_file, 0);
        setSize(rbu_v1_mono_size_file, totalSizeBytes);

        monoUpdateLoop(hdr_fh, data_fh);
        
        fclose(data_fh);
        data_fh = 0;
        fflush(NULL);
    
        cout << "BIOS staging is complete." << endl;
    }

/****************************************
   RBU Linux v0 functions (2.4)
****************************************/

    static void doPacketUpdate_v0(FILE *hdr_fh) 
    {
        const size_t bufSize = RBU_PACKET_SIZE;
        char buffer[bufSize] = {0};
    
        // set packet size, reset mono handler
        setSize(rbu_v0_size_file, 0);
        setSize(rbu_v0_size_file, bufSize);

        pktUpdateLoop(hdr_fh, rbu_v0_data_file, buffer, bufSize, false);
    }

    static void doMonoUpdate_v0(FILE *hdr_fh) 
    {
        cout << "Prep driver for data load." << endl;
    
        FILE *data_fh = fopen(rbu_v0_data_file, "wb");
        if (!data_fh)
            throw RbuDriverIOErrorImpl(strerror(errno));
    
        fseek(hdr_fh, 0, SEEK_END);
        size_t totalSizeBytes = ftell(hdr_fh);

        // set mono data size, reset pkt handler
        setSize(rbu_v0_size_file, 0);
        setSize(rbu_v0_size_file, totalSizeBytes);

        monoUpdateLoop(hdr_fh, data_fh);
        
        fclose(data_fh);
        data_fh = 0;
        fflush(NULL);
    
        cout << "BIOS staging is complete." << endl;
    }



/****************************************
   RBU Linux v2 functions (newer, included in 2.6.14+)
****************************************/

    static void setPacketType(packet_type type, const char *fn=rbu_v2_img_type_file)
    {
        FILE *type_fh = 0;
        type_fh = fopen(fn, "wb");
        if (!type_fh)
            throw RbuDriverIOErrorImpl(strerror(errno));
    
        switch(type)
        {
        case pt_mono:
            FWRITE("mono\0", 1, 5, type_fh);
            break;
        case pt_packet:
            FWRITE("packet\0", 1, 7, type_fh);
            break;
        case pt_any:  /*fall thru*/
        case pt_init:  /*fall thru*/
        default:
            // not really a packet type, but causes driver to free its memory
            FWRITE("init\0", 1, 5, type_fh);
            break;
        }
    
        if (ferror(type_fh))
            throw RbuDriverIOErrorImpl(strerror(errno));

        fclose(type_fh);
    }        // Step 5: set rbu cmos token
    
    
    static void waitForFile(const char *fn, time_t wait)
    {
        time_t start = time(NULL);
        while( access(fn, F_OK) && (time(NULL) - start < wait))
            /*nothing*/;
    }
    
    static void setLoadValue(char val)
    {
        FILE *load_fh = 0;
        int saved_errno = 0;
    
        waitForFile(rbu_v2_fw_load_file, 10);
    
        load_fh = fopen(rbu_v2_fw_load_file, "wb");
        if (!load_fh)
            throw RbuDriverIOErrorImpl(strerror(errno));
    
        FWRITE(&val, 1, 1, load_fh);
        if (ferror(load_fh))
            saved_errno = errno;

        fclose(load_fh);
        fflush(NULL);

        if (saved_errno)
            throw RbuDriverIOErrorImpl(strerror(saved_errno));
    }
    
    static void doPacketUpdate_v2(FILE *hdr_fh) 
    {
        const size_t bufSize = RBU_PACKET_SIZE;
        char buffer[bufSize] = {0};

        setSize(rbu_v2_pkt_size_file, bufSize);
        setLoadValue('1');
        pktUpdateLoop(hdr_fh, rbu_v2_fw_data_file, buffer, bufSize, false);
        setLoadValue('0');
    }
    
    static void doMonoUpdate_v2(FILE *hdr_fh) 
    {
        cout << "Prep driver for data load." << endl;
        setLoadValue('1');
    
        FILE *data_fh = fopen(rbu_v2_fw_data_file, "wb");
        if (!data_fh)
            throw RbuDriverIOErrorImpl(strerror(errno));
    
        monoUpdateLoop(hdr_fh, data_fh);
       
        fclose(data_fh);
        data_fh = 0;
        fflush(NULL);
    
        cout << "Notify driver data is finished." << endl;
        setLoadValue('0');
    }



/*****************************************************************************
******************************************************************************

main entry points for this module.

******************************************************************************
*****************************************************************************/
    
    void dellBiosUpdate(const IRbuHdr &hdr, packet_type force_type)
    {
        FILE *hdr_fh = hdr.getFh();
        fseek(hdr_fh, 0, 0);

        bool forced=false;
        
        // TODO: verify that it is a HDR file
        // TODO: checksum HDR file

        packet_type supported_pt = getSupportedPacketType();
        cout << "Supported RBU type for this system: (MONOLITHIC"
            << (supported_pt == pt_packet ? ", PACKET"   : "")
            << ")"
            << endl;
    
        if( force_type != pt_any )
        {
            supported_pt = force_type;
            forced = true;
        }
    
        driver_type dt = getDriverType();

        if (dt == rbu_linux_v2)
        {
            // initialize RBU driver.
            cout << "Using RBU v2 driver. Initializing Driver. " << endl;
            setPacketType(pt_init, rbu_v2_img_type_file);
        
            // set packet/mono type
            cout << "Setting RBU type in v2 driver to: " 
                << (supported_pt == pt_packet ? "PACKET" : "")
                << (supported_pt == pt_mono   ? "MONOLITHIC" : "")
                << (forced ? " (FORCED) ": "" )
                << endl;
            setPacketType(supported_pt, rbu_v2_img_type_file);
        
            switch(supported_pt)
            {
            case pt_packet:
                doPacketUpdate_v2(hdr_fh);
                break;
            case pt_mono:
                doMonoUpdate_v2(hdr_fh);
                break;
            default:
                break;
            }
        } 
        else if(dt == rbu_linux_v1)
        {
            cout << "Using RBU v1 method: " 
                << (supported_pt == pt_packet ? "PACKET" : "")
                << (supported_pt == pt_mono   ? "MONOLITHIC" : "")
                << (forced ? " (FORCED) ": "" )
                << endl;
 
            switch(supported_pt)
            {
            case pt_packet:
                doPacketUpdate_v1(hdr_fh);
                break;
            case pt_mono:
                doMonoUpdate_v1(hdr_fh);
                break;
            default:
                break;
            }
        }
        else if(dt == rbu_linux_v0)
        {
            cout << "Using RBU v0 driver. Initializing Driver. " << endl;
            setPacketType(pt_init, rbu_v0_type_file);

            cout << "Setting RBU type in v0 driver to: "
                << (supported_pt == pt_packet ? "PACKET" : "")
                << (supported_pt == pt_mono   ? "MONOLITHIC" : "")
                << (forced ? " (FORCED) ": "" )
                << endl;
            setPacketType(supported_pt, rbu_v0_type_file);

            switch(supported_pt)
            {
            case pt_packet:
                doPacketUpdate_v0(hdr_fh);
                break;
            case pt_mono:
                doMonoUpdate_v0(hdr_fh);
                break;
            default:
                break;
            }
        }
        else
        {
            throw RbuNotSupportedImpl("Could not open Dell RBU driver.");
        }
    
        cout << "Activate CMOS bit to notify BIOS that update is ready on next boot." << endl;
        activateRbuToken();
    
        cout << "Update staged sucessfully. BIOS update will occur on next reboot." << endl;
    }
    
    void cancelDellBiosUpdate()
    {
        // FOR LOAD CANCEL:
        // Step 1: always unset CMOS first
        cout << "Cancel BIOS CMOS notification bit." << endl;
        cancelRbuToken();

        driver_type dt = getDriverType();
        switch(dt)
        {
        case rbu_linux_v2:
            // Step 2: tell the dell_rbu driver to free its allocated memory
            cout << "Re-initialize driver for next user." << endl;
            setPacketType(pt_init, rbu_v2_img_type_file);

            // Step 3: make sure firmware class doesn't think we are loading
            cout << "Free kernel driver memory." << endl;
            setLoadValue('0');

            break;

        case rbu_linux_v1:
            // Step 2: Free monolithic, if present
            cout << "Re-initialize driver for next user." << endl;
            setSize(rbu_v1_mono_size_file, 0);
            setSize(rbu_v1_pkt_size_file, 0);
            fflush(NULL);
            break;

        case rbu_linux_v0:
            // Step 2: Free monolithic, if present
            cout << "Re-initialize driver for next user." << endl;
            setSize(rbu_v0_size_file, 0);
            setPacketType(pt_init, rbu_v0_type_file);
            fflush(NULL);
            break;


        default:
            cout << "Could not determine RBU driver present, skipping." << endl;
            break;
        }
    }
}

