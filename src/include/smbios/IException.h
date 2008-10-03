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


#ifndef IEXCEPTION_H
#define IEXCEPTION_H

// compat header should always be first header
#include "smbios/compat.h"

#include <string>
#include <exception>

// types.h should be first user-defined header.
#include "smbios/types.h"

// abi_prefix should be last header included before declarations
#include "smbios/config/abi_prefix.hpp"

#define DECLARE_EXCEPTION_EX( excName, namespace, superclass )  \
    class excName : public namespace::superclass \
    {\
    public:\
        virtual ~excName() throw() {};\
        excName() {};\
    }
#define DECLARE_EXCEPTION( excName )   DECLARE_EXCEPTION_EX( excName, smbios, IException )

namespace smbios
{
     //! Base class for all Abstract Exceptions.
     /**
     */
    class IException : public std::exception
    {
    public:
        // use default compiler-generated constructor/copy constructor/operator =
        // but need virtual destructor
        virtual ~IException() throw() {};
        IException() {};
    };

    // some standard exceptions

    /** Raised when some class does not implement part of the public interface
      Used mainly in classes where there are optional parts of the interface
      defined that require extra external functionality, such as XML, for example.
      */
    DECLARE_EXCEPTION( NotImplemented );

    /** Used in cases where something that "cannot happen" happens. Raised in
     * instances usually caused by some internal class state becoming
     * corrupted.
     */
    DECLARE_EXCEPTION( InternalError );

    /** Used in cases where operating system privleges prevent an action. */
    DECLARE_EXCEPTION( PermissionException );
}

// always should be last thing in header file
#include "smbios/config/abi_suffix.hpp"

#endif /* IEXCEPTION_H */
