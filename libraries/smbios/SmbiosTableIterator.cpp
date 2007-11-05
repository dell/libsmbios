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
#include "SmbiosImpl.h"

// message.h should be included last.
#include "smbios/message.h"

using namespace smbiosLowlevel;
using namespace std;

namespace smbios
{
    const ISmbiosItem & SmbiosTableIteratorBase::dereference () const
    {
        if (0 == current)
        {
            throw ParameterExceptionImpl (_("Programmer error: attempt to dereference a Null iterator."));
        }

        return table->getSmbiosItem(current);
    }

    void SmbiosTableIteratorBase::incrementIterator ()
    {
        const SmbiosTable *t = dynamic_cast<const SmbiosTable *>(table);

        while ((0 != t))
        {
            current = t->nextSmbiosStruct (current);
            if ((-1 == matchType) || (0 == current)
                    || ( reinterpret_cast<const smbios_structure_header *>(current)->type == matchType))
                break;
        }
    }

}
