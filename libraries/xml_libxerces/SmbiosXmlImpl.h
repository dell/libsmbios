// vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:cindent:
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


#ifndef SMBIOSXMLIMPL_H
#define SMBIOSXMLIMPL_H

// compat header should always be first header
#include "smbios/compat.h"

#include "XmlUtils.h"
#include "smbios/ISmbiosXml.h"
#include "SmbiosImpl.h"

namespace smbios
{
    class SmbiosTableXml : public virtual SmbiosTable
    {
    public:
        // ITERATORS!
        using  SmbiosTable::operator[];
        virtual iterator operator[]( const std::string & );
        virtual const_iterator operator[]( const std::string & ) const;

        virtual std::ostream & streamify(std::ostream & cout ) const;

        // CONSTRUCTORS/DESTRUCTORS
        virtual ~SmbiosTableXml();
        explicit SmbiosTableXml(std::vector<SmbiosStrategy *> initStrategyList, bool strictValidation = 0);

        void setXmlFilePath( std::string );
        int getTypeForString( const std::string )const;
        const std::string getStringForType( const int )const;

        const XML_NAMESPACE DOMDocument *getXmlDoc() const;

    protected:
        // Private, should never construct empty object.
        SmbiosTableXml ();


    protected:
        std::string xmlFile;

        // parser owns all XML entities. When it is deleted, everything
        // goes with it.
        XML_NAMESPACE DOMBuilder *parser;

        // The doc is owned by the parser. We do not have to clean it up
        // it is deleted when the parser is released. We keep a ref
        // here for speed purposes
        XML_NAMESPACE DOMDocument *doc;

        // indicates if XML subsystem has been initialized and should be
        // terminated.
        bool xmlInitialized;

    private:
        SmbiosTableXml (const SmbiosTableXml & source);
        SmbiosTableXml & operator = (const SmbiosTableXml & source);
    };
}

#endif /* SMBIOSXMLIMPL_H */

