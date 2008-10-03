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

//

#ifndef IFACTORY_H_INCLUDED
#define IFACTORY_H_INCLUDED

// compat header should always be first header
#include "smbios/compat.h"

#include <string>

// types.h should be first user-defined header.
#include "smbios/types.h"

// abi_prefix should be last header included before declarations
#include "smbios/config/abi_prefix.hpp"

namespace factory
{
    //! Base class for all Abstract Factories.
    /**
     */
    class IFactory
    {
    public:
        virtual ~IFactory();

        // Set Methods
        virtual void setParameter( const std::string name, const std::string value) = 0;
        virtual void setParameter( const std::string name, const u32 value) = 0;
        virtual void setMode( const int mode ) = 0;
        virtual void reset() = 0;

        // CONST Methods
        virtual std::string getParameterString( const std::string name ) const = 0;
        virtual u32 getParameterNum( const std::string name ) const = 0;
        virtual int getMode( ) const = 0;

        // DATA Definitions
        // standard modes.
        enum { AutoDetectMode, UnitTestMode };
        enum { defaultMode = AutoDetectMode };
    protected:
        IFactory();
    };

}

// always should be last thing in header file
#include "smbios/config/abi_suffix.hpp"

#endif /* IFACTORY_H_INCLUDED */
