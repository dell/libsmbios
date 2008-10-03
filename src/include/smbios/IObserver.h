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


#ifndef IOBSERVER_H
#define IOBSERVER_H

// compat header should always be first header
#include "smbios/compat.h"

#include <list>

// abi_prefix should be last header included before declarations
#include "smbios/config/abi_prefix.hpp"

namespace observer
{
    // forward declarations
    class IObservable;

    class IObserver
    {
    public:
        virtual ~IObserver();
        virtual void update( const IObservable *whatChanged, void *param = 0 ) = 0;
    protected:
        IObserver();
    };

    class IObservable
    {
    public:
        virtual ~IObservable();

        virtual void attach( IObserver * ) const;
        virtual void detach( IObserver * ) const;
        virtual void notify( void *param = 0 ) const;
    protected:
        IObservable();

    private:
        mutable std::list<IObserver *> observers;
    };

}

// always should be last thing in header file
#include "smbios/config/abi_suffix.hpp"

#endif /* IOBSERVER_H */
