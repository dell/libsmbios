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


#ifndef IMEMORY_H
#define IMEMORY_H

// compat header should always be first header
#include "smbios/compat.h"

// types.h should be first user-defined header.
#include "smbios/types.h"

#include "smbios/IFactory.h"
#include "smbios/IException.h"

// abi_prefix should be last header included before declarations
#include "smbios/config/abi_prefix.hpp"

namespace memory
{

    //! Base class for all Memory operations
    /**
    */
    // Exception Classes
    DECLARE_EXCEPTION( MemoryException );
    DECLARE_EXCEPTION_EX( AccessError, memory, MemoryException );
    DECLARE_EXCEPTION_EX( OutOfBounds, memory, MemoryException );

    //forward declarations... defined 'for real' below...
    class IMemory;

    /** The MemoryFactory class is based on the Factory design pattern.
    * The MemoryFactory is the recommended method to create IMemory
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
    class MemoryFactory : public virtual factory::IFactory
    {
    public:
        //! Create a factory object that you can use to create IMemory objects.
        /** Factory entry point: This is the method to call to get a handle
         * to the MemoryFactory. The MemoryFactory is the recommended method
         * to create IMemory objects.
         *
         * The getSingleton() is the recommended method to call to create
         * tables. You need not delete the pointer returned by this method, it
         * will be deleted by the factory when it is reset() or destructed.
         *
         * \returns Singleton MemoryFactory object pointer.
         */
        static MemoryFactory *getFactory();
        virtual ~MemoryFactory() throw ();

        //! Recommended way to get an IMemory object.
        /** getSingleton() returns a pointer to a Singleton IMemory
         * object. The user need not delete the pointer returned by this method.
         * The singleton will be deleted when the factory is destructed or
         * the reset() method is called
         * \returns (IMemory *) -- Factory manages lifetime, do not delete.
         */
        virtual IMemory *getSingleton() = 0;

        //! Create a new IMemory object that the caller must delete. (NOT RECOMMENDED)
        /** The make() method returns a pointer to a newly allocated
         * IMemory object. The caller is responsible for deleting this
         * reference when it is finished with it. It is recommended that the
         * caller store the pointer in an std::auto_ptr<IMemory>.
         *
         * The getSingleton() method is preferred over this method.
         *
         * \returns (IMemory *) -- caller must delete
         */
        virtual IMemory *makeNew() = 0;

    protected:
        //! Use getFactory() to get a factory.
        MemoryFactory();
    };


    class IMemory
    {
    public:
        // CONSTRUCTORS, DESTRUCTOR, and ASSIGNMENT
        // Interface class: no default or copy constructor
        IMemory();
        virtual ~IMemory ();

        virtual u8 getByte(u64 offset) const = 0;
        virtual void putByte(u64 offset, u8 value) const = 0;
        virtual void fillBuffer(u8 *buffer, u64 offset, unsigned int length) const = 0;
        virtual int incReopenHint() = 0;
        virtual int decReopenHint() = 0;
    private:
        void operator =(const IMemory &); ///< not implemented (explicitly disallowed)
        IMemory( const IMemory & ); ///< not implemented (explicitly disallowed)
    };

}


// always should be last thing in header file
#include "smbios/config/abi_suffix.hpp"

#endif  /* IMEMORY_H */
