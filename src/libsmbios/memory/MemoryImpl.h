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


#ifndef SMBIOSIMPL_H
#define SMBIOSIMPL_H

#include "smbios/IMemory.h"
#include "FactoryImpl2.h"
#include "ExceptionImpl.h"

namespace memory
{
    DEFINE_EXCEPTION_EX( AccessErrorImpl, memory, AccessError );
    DEFINE_EXCEPTION_EX( OutOfBoundsImpl, memory, OutOfBounds );

    class MemoryFactoryImpl : public factory::TFactory<MemoryFactory>
    {
    public:
        MemoryFactoryImpl();
        virtual ~MemoryFactoryImpl() throw ();
        virtual IMemory *getSingleton();
        virtual IMemory *makeNew();
    protected:
        static IMemory *_mem_instance;
    };


    // for unit tests
    class MemoryFile : public IMemory
    {
    public:
        // CONSTRUCTORS, DESTRUCTOR, and ASSIGNMENT
        explicit MemoryFile (const std::string file);
        virtual ~MemoryFile ();

        virtual void fillBuffer(u8 *buffer, u64 offset, unsigned int length) const;
        virtual u8 getByte(u64 offset) const;
        virtual void putByte(u64 offset, u8 value) const;
        virtual int incReopenHint() {return ++reopenHint;};
        virtual int decReopenHint() {return --reopenHint;};

    private:
        const std::string filename;
        mutable FILE *fd;
        bool rw;
        int reopenHint;

        MemoryFile ();
        MemoryFile (const MemoryFile & source);
        MemoryFile& operator = (const MemoryFile & source);
    };

    // for real work
    class MemoryOsSpecific : public IMemory
    {
    public:
        // CONSTRUCTORS, DESTRUCTOR, and ASSIGNMENT
        explicit MemoryOsSpecific (const std::string file);
        virtual ~MemoryOsSpecific ();

        virtual void fillBuffer(u8 *buffer, u64 offset, unsigned int length) const;
        virtual u8 getByte(u64 offset) const;
        virtual void putByte(u64 offset, u8 value) const;
        virtual int incReopenHint();
        virtual int decReopenHint();

    private:
        void *osData;

        MemoryOsSpecific ();
        MemoryOsSpecific (const MemoryOsSpecific & source);
        MemoryOsSpecific& operator = (const MemoryOsSpecific & source);
    };
}


#endif /* SMBIOSIMPL_H */
