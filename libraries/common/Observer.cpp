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

#define LIBSMBIOS_SOURCE
#include "smbios/IObserver.h"
#include "smbios/IFactory.h"

using namespace std;

namespace factory
{
    IFactory::~IFactory() {}
    IFactory::IFactory() {}
}

namespace observer
{

    IObserver::IObserver()
    {}

    IObserver::~IObserver()
    {}

    IObservable::IObservable()
    {}

    IObservable::~IObservable()
    {}

    void IObservable::attach( IObserver *o ) const
    {
        observers.push_back(o);
    }

    void IObservable::detach( IObserver *o ) const
    {
        observers.remove(o);
    }

    void IObservable::notify(void *param) const
    {
        list< IObserver * >::iterator iter;
        for( iter = observers.begin(); iter != observers.end(); ++iter )
        {
            (*iter)->update(this, param);
        }
    }



}
