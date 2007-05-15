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

// compat header should always be first header if including system headers
#define LIBSMBIOS_SOURCE
#include "smbios/compat.h"

#include "SmiImpl.h"

using namespace std;

/*
 * This will eventually be implemented by using the windows dcdbas
 * driver to make smi calls. For now, just error them all out.
 */

#define NOT_IMPLEMENTED { throw smbios::NotImplementedImpl("SMI calls are not yet implemented on Windows."); }


namespace smi
{
    SmiArchStrategy::SmiArchStrategy() {}
    SmiArchStrategy::~SmiArchStrategy() {}
    void SmiArchStrategy::lock() NOT_IMPLEMENTED
    size_t SmiArchStrategy::getPhysicalBufferBaseAddress() NOT_IMPLEMENTED 
    void SmiArchStrategy::setSize(int) NOT_IMPLEMENTED
    void SmiArchStrategy::addInputBuffer(u8 *, size_t) NOT_IMPLEMENTED
    void SmiArchStrategy::getResultBuffer(u8 *, size_t) NOT_IMPLEMENTED
    void SmiArchStrategy::execute() NOT_IMPLEMENTED
    void SmiArchStrategy::finish() NOT_IMPLEMENTED
}

