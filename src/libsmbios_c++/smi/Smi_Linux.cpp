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

#include <sys/file.h>
#include <string.h>
#include <cstdio>

#include "SmiImpl.h"

using namespace std;

#define SMI_DATA_FILE       "/sys/devices/platform/dcdbas/smi_data"
#define SMI_PHYS_ADDR_FILE  "/sys/devices/platform/dcdbas/smi_data_buf_phys_addr"
#define SMI_DO_REQUEST_FILE "/sys/devices/platform/dcdbas/smi_request"
#define SMI_BUF_SIZE_FILE   "/sys/devices/platform/dcdbas/smi_data_buf_size"

//#define SMI_DATA_FILE       "/tmp/smi_data"
//#define SMI_PHYS_ADDR_FILE  "/tmp/smi_data_buf_phys_addr"
//#define SMI_DO_REQUEST_FILE "/tmp/smi_request"

struct smiLinuxPrivateData
{
    FILE *fh_data;
    FILE *fh_doReq;
};

namespace smi
{
    static size_t FWRITE(const void *ptr, size_t size, size_t nmemb, FILE *stream)
    {
        size_t written = fwrite(ptr, size, nmemb, stream); 
        // TODO: handle short write
        if (written < nmemb)
            throw smbios::InternalErrorImpl("Short write to file handle");
        return written;
    }

    SmiArchStrategy::SmiArchStrategy()
    {
        privateData = new smiLinuxPrivateData;
        memset(privateData, 0, sizeof(smiLinuxPrivateData));
    }

    SmiArchStrategy::~SmiArchStrategy()
    {
        smiLinuxPrivateData *tmpPrivPtr = reinterpret_cast<smiLinuxPrivateData *>(privateData);

        if(tmpPrivPtr->fh_data)
            fclose(tmpPrivPtr->fh_data);

        if(tmpPrivPtr->fh_doReq)
            fclose(tmpPrivPtr->fh_doReq);

        delete tmpPrivPtr;
        privateData = 0;
    }

    void SmiArchStrategy::lock()
    {
        smiLinuxPrivateData *tmpPrivPtr = reinterpret_cast<smiLinuxPrivateData *>(privateData);


        tmpPrivPtr->fh_data = fopen(SMI_DATA_FILE, "r+b");
        if( ! tmpPrivPtr->fh_data )
            throw smbios::InternalErrorImpl("Could not open file " SMI_DATA_FILE ". Check that dcdbas driver is properly loaded.");

        tmpPrivPtr->fh_doReq = fopen(SMI_DO_REQUEST_FILE, "wb");
        if( ! tmpPrivPtr->fh_doReq)
            throw smbios::InternalErrorImpl("Could not open file " SMI_DO_REQUEST_FILE ". Check that dcdbas driver is properly loaded.");

        flock( fileno(tmpPrivPtr->fh_data), LOCK_EX );

        fseek(tmpPrivPtr->fh_doReq, 0L, 0);
        FWRITE("0", 1, 1, tmpPrivPtr->fh_doReq);
        fseek(tmpPrivPtr->fh_doReq, 0L, 0);
    }

    size_t SmiArchStrategy::getPhysicalBufferBaseAddress()
    {
        const int bufSize=63;
        char tmpBuf[bufSize+1] = {0,};
        size_t retval = 0;

        fflush(NULL);

        FILE *fh = fopen(SMI_PHYS_ADDR_FILE, "rb");
        if( ! fh )
            throw smbios::InternalErrorImpl("Could not open file " SMI_PHYS_ADDR_FILE ". Check that dcdbas driver is properly loaded.");

        fseek(fh, 0L, 0);
        size_t numBytes = fread(tmpBuf, 1, bufSize, fh);
        fclose(fh);
        fh=0;
        if (!numBytes) // dont care how many bytes as long as we get at least 1.
            throw smbios::InternalErrorImpl("Short read from physical address file. Driver problem?");

        retval = strtoll(tmpBuf, NULL, 16);

        return retval;
    }

    void SmiArchStrategy::setSize(int newSize)
    {
        const int bufSize=63;
        char tmpBuf[bufSize+1] = {0,};

        fflush(NULL);

        FILE *fh = fopen(SMI_BUF_SIZE_FILE, "w+b");
        if( ! fh )
            throw smbios::InternalErrorImpl("Could not open file " SMI_BUF_SIZE_FILE ". Check that dcdbas driver is properly loaded.");

        snprintf(tmpBuf, bufSize, "%d", newSize);
        FWRITE(tmpBuf, 1, bufSize, fh);
        fclose(fh);

        fflush(NULL);
        fh=0;
    }

    void SmiArchStrategy::addInputBuffer(u8 *buffer, size_t size)
    {
        smiLinuxPrivateData *tmpPrivPtr = reinterpret_cast<smiLinuxPrivateData *>(privateData);
        FWRITE(buffer,  1,  size,  tmpPrivPtr->fh_data);
    }

    void SmiArchStrategy::getResultBuffer(u8 *buffer, size_t size)
    {
        smiLinuxPrivateData *tmpPrivPtr = reinterpret_cast<smiLinuxPrivateData *>(privateData);
        fflush(NULL);
        int numbytes = fread(buffer,  1,  size,  tmpPrivPtr->fh_data);
        if (!numbytes)
            throw smbios::InternalErrorImpl("Short read from file handle");
    }


    void SmiArchStrategy::execute()
    {
        smiLinuxPrivateData *tmpPrivPtr = reinterpret_cast<smiLinuxPrivateData *>(privateData);
        fflush(NULL);
        FWRITE("1", 1, 1, tmpPrivPtr->fh_doReq);
        fflush(NULL);
        fseek(tmpPrivPtr->fh_data, 0L, 0);
    }

    void SmiArchStrategy::finish()
    {
        smiLinuxPrivateData *tmpPrivPtr = reinterpret_cast<smiLinuxPrivateData *>(privateData);
        flock( fileno(tmpPrivPtr->fh_data), LOCK_UN );
        fclose(tmpPrivPtr->fh_doReq);
        fclose(tmpPrivPtr->fh_data);

        tmpPrivPtr->fh_doReq=0;
        tmpPrivPtr->fh_data=0;
    }
}

