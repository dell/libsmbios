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
#include <errno.h>
#include <string.h>
#include <cstdio>
#include "CmosRWImpl.h"

using namespace std;

namespace cmos
{

    //
    // NON-MEMBER FUNCTIONS
    //
    void readByteArray ( const ICmosRW &cmos, u32 indexPort, u32 dataPort, u32 offset, u8 * target, u32 count)
    {
        for (u32 i = 0; i < count; ++i)
        {
            target[i] = cmos.readByte (indexPort, dataPort, offset + i);
        }
    }

    void writeByteArray ( const ICmosRW &cmos, u32 indexPort, u32 dataPort, u32 offset, const u8 * source, u32 count)
    {
        const Suppressable *s = dynamic_cast<const Suppressable *>(&cmos);
        if(s)
            s->suppressNotification();
        for (u32 i = 0; i < count; ++i)
        {
            cmos.writeByte (indexPort, dataPort, offset + i, source[i]);
        }
        if(s)
            s->resumeNotification();
    }



    //
    // Suppressable
    //
    // This class is used to supress ->notify() calls inside an Observable
    // class. This lets us do many operations that may cause spurious
    // notifications. This would also probably let us do some simple
    // transaction-like operations.
    //
    Suppressable::Suppressable()
            : suppressNotify(false)
    {}

    Suppressable::~Suppressable()
    {}

    void Suppressable::suppressNotification(bool sup) const
    {
        suppressNotify = sup;
    }

    void Suppressable::resumeNotification(bool doNotify) const
    {
        const observer::IObservable *o = dynamic_cast<const observer::IObservable *>(this);
        if(o && doNotify)
            o->notify();

        suppressNotify = false;
    }

    bool Suppressable::isNotifySuppressed() const
    {
        return suppressNotify;
    }

    //
    // ICmosRW functions
    //
    ICmosRW::ICmosRW()
    {}

    ICmosRW::~ICmosRW()
    {}

    //
    // CmosRWFile functions
    //

    // REGULAR CONSTRUCTOR
    CmosRWFile::CmosRWFile ( const string &File )
            :ICmosRW(), Suppressable(), fileName (File)
    {}

    // DESTRUCTOR
    CmosRWFile::~CmosRWFile()
    {}

    CmosRWIo::~CmosRWIo()
    {}

    // TODO: need to throw exception on problem with file
    //
    u8 CmosRWFile::readByte (u32 indexPort, u32 dataPort, u32 offset) const
    {
        u8 retval = 0xFF;
        u32 realOffset = indexPort * 256 + offset;
        (void) dataPort; // unused
        string errMessage("Could not open CMOS file(" + fileName + ") for reading: ");

        FILE *fh = fopen (fileName.c_str (), "rb");
        if( !fh )
            throw smbios::InternalErrorImpl(errMessage + strerror(errno));

        fseek (fh, static_cast<long>(realOffset), SEEK_SET);
        size_t numRecs = fread (&retval, sizeof (retval), 1, fh); // only used in unit tests, so isnt critical
        fclose (fh);
        if (numRecs != 1)
            throw std::exception(); // short read. there isnt really a good exception to throw here.

        return retval;
    }

    // TODO: need to throw exception on problem with file
    //
    void CmosRWFile::writeByte (u32 indexPort, u32 dataPort, u32 offset, u8 byte) const
    {
        //cout << "w(" << offset << ")";
        u32 realOffset = indexPort * 256 + offset;
        (void) dataPort; // unused
        string errMessage("Could not open CMOS file(" + fileName + ") for writing: ");

        FILE *fh = fopen (fileName.c_str (), "r+b");
        if( !fh )
            throw smbios::InternalErrorImpl(errMessage + strerror(errno));

        fseek (fh, static_cast<long>(realOffset), SEEK_SET);
        size_t recs = fwrite (&byte, sizeof (byte), 1, fh);
        fclose (fh);
        fflush(NULL);

        if (recs < 1)
            throw std::exception(); // short write. there isnt really a good exception to throw here.

        if(! isNotifySuppressed() )
        {
            // writers are responsible for only writing changed values
            // otherwise we get to see how fast our OS can do an
            // infinite loop. :-)
            notify();
        }
        return;
    }


}
