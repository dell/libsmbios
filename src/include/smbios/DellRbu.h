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


#ifndef RBU_H
#define RBU_H

// compat header should always be first header, if system headers included
#include "smbios/compat.h"

#include <string>

// types.h should be first user-defined header.
#include "smbios/types.h"
#include "smbios/IFactory.h"
#include "smbios/IException.h"

// abi_prefix should be last header included before declarations
#include "smbios/config/abi_prefix.hpp"


namespace rbu
{
    DECLARE_EXCEPTION( RbuException );
    DECLARE_EXCEPTION_EX( RbuNotSupported, rbu, RbuException );
    DECLARE_EXCEPTION_EX( InvalidHdrFile, rbu, RbuException );
    DECLARE_EXCEPTION_EX( UnsupportedSystemForHdrFile, rbu, RbuException );
    DECLARE_EXCEPTION_EX( HdrFileIOError, rbu, RbuException );
    DECLARE_EXCEPTION_EX( RbuDriverIOError, rbu, RbuException );

    typedef enum { pt_any, pt_mono, pt_packet, pt_init } packet_type;
    // rbu_linux_v0: Linux 2.4
    // rbu_linux_v1: 2.6 dkms
    // rbu_linux_v2: 2.6.14+
    typedef enum { rbu_unsupported, rbu_linux_v0, rbu_linux_v1, rbu_linux_v2, rbu_solaris } driver_type;

    const int SYSID_OVERRIDE = 1;
    const int BIOSVER_OVERRIDE = 2;

    // forward decls
    class IRbuHdr;

    //!AbstractFactory that produces IRbuHdr objects.
    /** The RbuFactory class is based on the Factory design pattern.
    * The RbuFactory is the recommended method to create IRbuHdr
    * objects.
    *
    * The getSingleton() is the recommended method to call to create
    * tables. You need not delete the pointer returned by this method, it
    * will be delete by the factory when it is reset() or destructed.
    *
    * Most users of the factory need call nothing more than getFactory()
    * and then getSingleton() on the returned factory object.
    *
    * Advanced users can call setParameter() to set up internal factory
    * variables that control creation of tables.
    */
    class RbuFactory : public virtual factory::IFactory
    {
    public:
        //! Create a factory object that you can use to create IRbuHdr objects.
        /** Factory entry point: This is the method to call to get a handle
         * to the RbuFactory. The RbuFactory is the recommended method
         * to create IRbuHdr objects.
         *
         * The getSingleton() is the recommended method to call to create
         * tables. You need not delete the pointer returned by this method, it
         * will be deleted by the factory when it is reset() or destructed.
         *
         * \returns Singleton RbuFactory object pointer.
         */
        static RbuFactory *getFactory();
        virtual ~RbuFactory() throw();

        //! Create a new IRbuHdr object that the caller must delete. (NOT RECOMMENDED)
        /** The makeNew() method returns a pointer to a newly allocated
         * IRbuHdr object. The caller is responsible for deleting this
         * reference when it is finished with it. It is recommended that the
         * caller store the pointer in an std::auto_ptr<IRbuHdr>.
         *
         * The getSingleton() method is preferred over this method.
         *
         * \returns (IRbuHdr *) -- caller must delete
         */
        virtual IRbuHdr *makeNew(std::string filename) = 0;
    protected:
        //! Use getFactory() to get a factory.
        RbuFactory();
    };


    //!Interface definition for RBU HDR operations.
    class IRbuHdr
    {
    public:
        // CONSTRUCTORS, DESTRUCTOR, and ASSIGNMENT
        IRbuHdr();
        // Interface class: no default or copy constructor
        virtual ~IRbuHdr ();

        //! Used by operator << (std::ostream & cout, const IRbuHdr & ) to
        //output table information.
        /** Users normally would not need or want to call this API. The normal
         * operator<<() has been overloaded to call this function internally.
         */
        virtual std::ostream & streamify(std::ostream & cout ) const = 0;

        virtual std::string getBiosVersion() const = 0;
        virtual void getHdrVersion(unsigned int &major, unsigned int &minor) const = 0;
        virtual const u32 *getSystemIdList() const = 0;
        virtual void doUpdate() const = 0;
        virtual FILE *getFh() const = 0;

    private:
        explicit IRbuHdr(const IRbuHdr &); ///< not implemented (explicitly disallowed)
        void operator =( const IRbuHdr & ); ///< not implemented (explicitly disallowed)
    };

    std::ostream & operator << (std::ostream & cout, const IRbuHdr & item);

    //! Cancel BIOS Update on Dell systems
    /**
     */
    void cancelDellBiosUpdate();

    //! Check to see if a HDR file supports a specific System ID
    /**
     */
    bool checkSystemId(const IRbuHdr &hdr, u16 sysId );

    //! Update BIOS on Dell systems
    /**
     */
    void dellBiosUpdate(const IRbuHdr &hdr, packet_type force_type);

    //! Compare BIOS Versions
    /**
     */
    int compareBiosVersion(std::string ver1, std::string ver2);

}

// always should be last thing in header file
#include "smbios/config/abi_suffix.hpp"

#endif  /* RBU_H */
