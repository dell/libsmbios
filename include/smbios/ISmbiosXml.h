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


#ifndef ISMBIOSXML_H
#define ISMBIOSXML_H

// make this define so that the automatic linking stuff
// does its magic
#define LIBSMBIOS_NEED_SMBIOSXML

// compat header should always be first header
#include "smbios/compat.h"

#include "smbios/ISmbios.h"

// abi_prefix should be last header included before declarations
#include "smbios/config/abi_prefix.hpp"

namespace smbios
{
    class SmbiosXmlFactory: public virtual SmbiosFactory
    {
    public:
        static SmbiosFactory *getFactory();
        virtual ~SmbiosXmlFactory() throw();
    };

    template <class R> 
    R &getData(const ISmbiosItem &item, const std::string field, R &out)
    {
        getData_FromItem(field, &out, sizeof(R));
        return out;
    }

    u8 getU8_FromItem(const ISmbiosItem &item, std::string field);
    u16 getU16_FromItem(const ISmbiosItem &item, std::string field);
    u32 getU32_FromItem(const ISmbiosItem &item, std::string field);
    u64 getU64_FromItem(const ISmbiosItem &item, std::string field);
    const char *getString_FromItem(const ISmbiosItem &item, std::string field);
    void *getBits_FromItem(const ISmbiosItem &item, std::string field, std::string bitField, void *out);

    // helper... not really convenient to use directly
    void getData_FromItem(std::string field, void *out, size_t sz);

    // not implemented yet.
    std::ostream & toXmlString(const ISmbiosTable &, std::ostream &);
}

// always should be last thing in header file
#include "smbios/config/abi_suffix.hpp"

#endif /* ISMBIOSXML_H */

