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


#ifndef ICMOSRW_H
#define ICMOSRW_H

// compat header should always be first header
#include "smbios/compat.h"

// types.h should be first user-defined header.
#include "smbios/types.h"

#include "smbios/IFactory.h"
#include "smbios/IException.h"

// abi_prefix should be last header included before declarations
#include "smbios/config/abi_prefix.hpp"

namespace cmos
{
    //! Abstract base class for the cmos read write operations
    /**
    */
    // Exceptions
    DECLARE_EXCEPTION( CmosException );
    DECLARE_EXCEPTION_EX( InvalidCmosRWMode, cmos, CmosException );

    // forward declarations.
    class ICmosRW;

    class CmosRWFactory : public virtual factory::IFactory
    {
    public:
        static CmosRWFactory *getFactory();
        virtual ~CmosRWFactory() throw();
        virtual ICmosRW *getSingleton( ) = 0; // returns singleton
        virtual ICmosRW *makeNew( ) = 0; // not for use
    protected:
        CmosRWFactory();
    };

    class ICmosRW
    {
    public:
        explicit ICmosRW();
        virtual u8 readByte( u32 indexPort, u32 dataPort, u32 offset ) const = 0;
        virtual void writeByte( u32 indexPort, u32 dataPort, u32 offset, u8 byte ) const = 0;
        virtual ~ICmosRW();
    private:
        ICmosRW( const ICmosRW &source ); ///< NOT ALLOWED. No copy constructor
        void operator = (const ICmosRW &source ); ///< NOT ALLOWED. No overloaded assignment
    };

    /** Read or write an array of bytes to CMOS.
     *
     * This function is set up as a non-member helper function. It successively reads/writes \param count bytes from/to CMOS.
     *
     * Note that the byte array passed as a parameter need not be zero-terminated.
     *
     * \param cmos Pass in the ICmosRW object to operate on
     * \param indexPort The IO port to write the offset to
     * \param dataPort The IO port to read/write data after setting the index via indexPort
     * \param offset The offset within CMOS. CMOS is typically multiple 256byte pages.
     * \param target Byte array to store the results. Call must already have allocated.
     * \param count The number of bytes to read/write.
     */
    void readByteArray( const ICmosRW &cmos, u32 indexPort, u32 dataPort, u32 offset, u8* target, u32 count);
    /** Read or write an array of bytes to CMOS.
     *
     * This function is set up as a non-member helper function. It successively reads/writes \param count bytes from/to CMOS.
     *
     * Note that the byte array passed as a parameter need not be zero-terminated.
     *
     * \param cmos Pass in the ICmosRW object to operate on
     * \param indexPort The IO port to write the offset to
     * \param dataPort The IO port to read/write data after setting the index via indexPort
     * \param offset The offset within CMOS. CMOS is typically multiple 256byte pages.
     * \param source byte array to store the CMOS contents. must be allocated by caller.
     * \param count The number of bytes to read/write.
     * */
    void writeByteArray( const ICmosRW &cmos, u32 indexPort, u32 dataPort, u32 offset, const u8* source, u32 count);

}

// always should be last thing in header file
#include "smbios/config/abi_suffix.hpp"

#endif  /* ICMOSRW_H */
