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
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 */

#ifndef SMIIMPL_H
#define SMIIMPL_H

#include "smbios/ISmi.h"
#include "SmiLowLevel.h"
#include "ExceptionImpl.h"
#include <cstdio>

namespace smi
{
    // define our exceptions
    DEFINE_EXCEPTION_EX( SmiExceptionImpl, smi, SmiException);
    DEFINE_EXCEPTION_EX( InvalidSmiModeImpl, smi, InvalidSmiMode);
    DEFINE_EXCEPTION_EX( ParameterErrorImpl, smi, ParameterError);
    DEFINE_EXCEPTION_EX( UnsupportedSmiImpl, smi, UnsupportedSmi);
    DEFINE_EXCEPTION_EX( UnhandledSmiImpl,   smi, UnhandledSmi);
    DEFINE_EXCEPTION_EX( SmiExecutedWithErrorImpl, smi, SmiExecutedWithError);
    DEFINE_EXCEPTION_EX( PasswordVerificationFailedImpl, smi, PasswordVerificationFailed);
    DEFINE_EXCEPTION_EX( ConfigErrorImpl, smi, ConfigError);

    class SmiStrategy
    {
    public:
        SmiStrategy()
        {}
        ;
        virtual ~SmiStrategy()
        {}
        ;

        virtual void lock()
            = 0;
        virtual void setSize(int) = 0;
        virtual size_t getPhysicalBufferBaseAddress() = 0;
        virtual void addInputBuffer(u8 *buffer, size_t size) = 0;
        virtual void execute() = 0;
        virtual void getResultBuffer(u8 *buffer, size_t size) = 0;
        virtual void finish() = 0;
    };

    class SmiMockStrategy : public SmiStrategy
    {
    public:
        SmiMockStrategy(std::string initFilename) : fh(fopen (initFilename.c_str (), "w+b")), filename(initFilename)
        {}
        ;
        virtual ~SmiMockStrategy()
        {
            fclose (fh);
        };

        virtual void lock()
        {}
        ;
        virtual void setSize(int)
        {}
        ;
        virtual size_t getPhysicalBufferBaseAddress()
        {
            return 0xDEADBEEF;
        };
        virtual void addInputBuffer(u8 *buffer, size_t size)
        {
            size_t written = fwrite(buffer, 1, size, fh);
            if (written < size)
                throw std::exception();
        };
        virtual void execute()
        {
            fseek(fh,0,0);
        };
        virtual void getResultBuffer(u8 *buffer, size_t size)
        {
            size_t numbytes = fread(buffer,1,size,fh); // only used in unit tests, not critical
            if (numbytes != size)
            {
                throw SmiExceptionImpl("Short read from file.");
            }
        };
        virtual void finish()
        {}
        ;
    private:
        FILE *fh;
        std::string filename;
    };

    class SmiArchStrategy : public SmiStrategy
    {
    public:
        SmiArchStrategy();
        virtual ~SmiArchStrategy();

        virtual void lock()
            ;
        virtual void setSize(int);
        virtual size_t getPhysicalBufferBaseAddress();
        virtual void addInputBuffer(u8 *buffer, size_t size);
        virtual void execute();
        virtual void getResultBuffer(u8 *buffer, size_t size);
        virtual void finish();

    private:
        void *privateData;
    };


    class DellCallingInterfaceSmiImpl : virtual public IDellCallingInterfaceSmi
    {
    public:
        DellCallingInterfaceSmiImpl(SmiStrategy *, u16 address, u8 code );
        virtual ~DellCallingInterfaceSmiImpl();

        virtual void execute();
        virtual void setClass( u16 newClass );
        virtual void setSelect( u16 newSelect );
        virtual void setArg( u8 argNumber, u32 argValue );
        virtual u32  getRes( u8 resNumber ) const;
        virtual void setArgAsPhysicalAddress( u8 argNumber, u32 bufferOffset );
        virtual const u8 *getBufferPtr();
        virtual void setBufferSize(size_t newSize);
        virtual void setBufferContents(const u8 *, size_t size);

    protected:
        struct calling_interface_command_buffer smi_buf;
        bool argIsAddress[4];
        u32  argAddressOffset[4];
        struct callintf_cmd                     kernel_buf;
        u8                                      *buffer;
        size_t                                  bufferSize;
        std::auto_ptr<SmiStrategy>              smiStrategy;

    private:
        DellCallingInterfaceSmiImpl();
    };

}

#endif  /* SMIIMPL_H */
