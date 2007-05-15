// vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:
/*
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

#ifndef CMOSRWIMPL_H
#define CMOSRWIMPL_H

#include "smbios/IObserver.h"
#include "smbios/ICmosRW.h"
#include "ExceptionImpl.h"

namespace cmos
{
    // define our exceptions
    DEFINE_EXCEPTION_EX( InvalidCmosRWModeImpl, cmos, InvalidCmosRWMode );

    class Suppressable : public observer::IObservable
    {
    public:
        Suppressable();
        void suppressNotification(bool sup = true) const;
        void resumeNotification(bool doNotify = true) const;
        bool isNotifySuppressed() const;
        virtual ~Suppressable();

    protected:
        mutable bool suppressNotify;
    };

    class CmosRWFile : public ICmosRW, public Suppressable
    {
    public:
        // CONSTRUCTORS, DESTRUCTOR, and ASSIGNMENT
        explicit CmosRWFile (const std::string &File);
        CmosRWFile& operator = (const CmosRWFile & source);
        virtual ~CmosRWFile();

        u8 readByte( u32 indexPort, u32 dataPort, u32 offset ) const;
        void writeByte( u32 indexPort, u32 dataPort, u32 offset, u8 byte ) const;

    protected:
        std::string fileName;

    private:
        CmosRWFile (const CmosRWFile & source); //copy constructor
    };


    class CmosRWIo : public ICmosRW, public Suppressable
    {
    public:
        // CONSTRUCTORS, DESTRUCTOR, and ASSIGNMENT
        explicit CmosRWIo ();
        CmosRWIo& operator = (const CmosRWIo & source);
        virtual ~CmosRWIo();

        u8 readByte( u32 indexPort, u32 dataPort, u32 offset ) const;
        void writeByte( u32 indexPort, u32 dataPort, u32 offset, u8 byte ) const;
    private:
        CmosRWIo (const CmosRWIo & source); //copy constructor
    };

}


#endif  /* CMOSRWIMPL_H */
