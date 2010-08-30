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

#if defined(DEBUG_SYSINFO)
#define DEBUG_OUTPUT_ALL
#endif

// compat header should always be first header if including system headers
#define LIBSMBIOS_SOURCE
#include "smbios/compat.h"

#include <sstream>
#include <string.h>

#include "SmbiosImpl.h"

// message.h should be included last.
#include "smbios/message.h"

using namespace smbiosLowlevel;
using namespace std;

namespace smbios
{

    ISmbiosTable::ISmbiosTable()
    {}

    ISmbiosTable::~ISmbiosTable()
    {}

    // Regular CONSTRUCTOR   NOT ALLOWED!
    // This is marked "protected:" in the header (only to be used by derived classes)
    SmbiosTable::SmbiosTable ()
            : ISmbiosTable(), itemList(), initializing(true), strictValidationMode(false), workaround(0), smbiosBuffer (0)
    {
        memset (
            const_cast<smbios_table_entry_point *>(&table_header),
            0,
            sizeof (table_header)
        );
    }

    // Regular Constructor
    SmbiosTable::SmbiosTable(std::vector<SmbiosStrategy *> initStrategyList, bool strictValidation)
            :ISmbiosTable(), itemList(), initializing(true), strictValidationMode(strictValidation), workaround(0), smbiosBuffer (0), strategyList(initStrategyList)
    {
        // clearItemCache(); // works fine if we call here.
        reReadTable(); // may throw
    }

    // DESTRUCTOR
    SmbiosTable::~SmbiosTable ()
    {
        clearItemCache();

        if (0 != smbiosBuffer)
        {
            memset ( const_cast<u8 *>(smbiosBuffer), 0, sizeof (*smbiosBuffer));
            delete [] const_cast<u8 *>(smbiosBuffer);
            smbiosBuffer = 0;
        }

        memset (
            const_cast<smbios_table_entry_point *>(&table_header),
            0,
            sizeof (table_header)
        );

        std::vector< SmbiosStrategy *>::iterator strategy;
        for( strategy = strategyList.begin(); strategy != strategyList.end(); ++strategy )
        {
            delete *strategy;
        }
    }

    ISmbiosItem *SmbiosTable::getCachedItem( const void * itemPtr ) const
    {
        ISmbiosItem *ret = 0;
        if ((itemList.find (itemPtr) !=
                itemList.end ()))
        {
            if( 0 != itemList[itemPtr] )
            {
                ret = itemList[itemPtr];
            }
            else
            {
                throw InternalErrorImpl(_("No null pointers should ever leak into the itemList"));
            }
        }
        return ret;
    }

    void SmbiosTable::cacheItem( const void *ptr, ISmbiosItem &newitem ) const
    {
        // The following two lines are equivalent to:
        //table->itemList[current] = newitem;
        //  but are more efficient
        pair < const void *, ISmbiosItem * >myPair (ptr, &newitem);
#ifndef __SUNPRO_CC
        itemList.insert (itemList.begin (), myPair);
#endif
    }

    SmbiosTable::iterator SmbiosTable::begin ()
    {
        return SmbiosTable::iterator (this);
    }

    SmbiosTable::const_iterator SmbiosTable::begin () const
    {
        return SmbiosTable::const_iterator (this);
    }

    SmbiosTable::iterator SmbiosTable::end ()
    {
        return SmbiosTable::iterator ();
    }

    SmbiosTable::const_iterator SmbiosTable::end ()const
    {
        return SmbiosTable::const_iterator ();
    }

    SmbiosTable::iterator SmbiosTable::operator[] (const int type)
    {
        return SmbiosTable::iterator (this, type);
    }

    SmbiosTable::const_iterator SmbiosTable::operator[](const int type) const
    {
        return SmbiosTable::const_iterator (this, type);
    }

    SmbiosTable::iterator SmbiosTable::operator[] (const string &)
    {
        throw NotImplementedImpl(_("This is an enhanced function call that is not available in the base Smbios library. You must be using an enhanced library such as SmbiosXml to use this API"));
    }

    SmbiosTable::const_iterator SmbiosTable::operator[](const string &) const
    {
        throw NotImplementedImpl(_("This is an enhanced function call that is not available in the base Smbios library. You must be using an enhanced library such as SmbiosXml to use this API"));
    }

    void SmbiosTable::reReadTable()
    {
        bool gotTable = false;
        // make sure there are no cached objects.
        // MSVC++ crashes here if you remove the if(). No idea why, works on
        // GCC.
        if( ! initializing )
            clearItemCache();

        // go through our strategies one-by-one and ask them if they can 
        // fulfill the request.
        DCERR("calling strategy code to read table" << endl);
        std::vector< SmbiosStrategy *>::iterator strategy;
        for( strategy = strategyList.begin(); strategy != strategyList.end(); ++strategy )
        {
            try
            {
                DCERR("  strategy: 0x" << hex << (int)(*strategy) << endl);
                if( (*strategy)->getSmbiosTable(&smbiosBuffer, &table_header, getStrictValidationMode()) )
                {
                    DCERR("    RETURNED SUCCESS" << endl);
                    gotTable = true;
                    break;
                }
            }
            catch(...)
            {
                // nil
            }
        }
        DCERR("TABLE HEADER DUMP: " << endl << *this << endl);
        DCERR("TABLE BUFFER: 0x" << hex << (int)smbiosBuffer << endl);
        
        if (! gotTable)
        {
            // manually delete allocated stuff here because the destructor 
            // does not get called and this stuff will not be freed.
            std::vector< SmbiosStrategy *>::iterator strategy;
            for( strategy = strategyList.begin(); strategy != strategyList.end(); ++strategy )
            {
                delete *strategy;
            }
            throw InternalErrorImpl(_("Could not instantiate SMBIOS table."));
        }
    }

    void SmbiosTable::initializeWorkaround() const
    {
        // crap code to work around compiler bugs in:
        //   -- gcc 2.96
        //   -- msvc++ 6.0
        //
        const SmbiosWorkaroundTable *ptr = workaround.get();
        workaround.release();
        delete const_cast<SmbiosWorkaroundTable *>(ptr);
        std::auto_ptr<SmbiosWorkaroundTable> foo(
            SmbiosWorkaroundFactory::getFactory()->makeNew( this ));
        workaround = foo;

        // after initializing the workaround object, go through and delete
        // all existing items.
        //
        // have to do this because items are possibly created with wrong
        // type. Types higher than SmbiosTableMemory
        // call this through constructor, and this call happens before
        // we enter the constructor for the higher level type.
        //
        // For example, if we are supposed to initializing an Xml Table,
        // The items here _should_ be SmbiosItemAccess objects, but instead
        // they turn out to be regular SmbiosItem objects.
        clearItemCache();

        initializing = false;
    }

    void SmbiosTable::rawMode(bool m) const
    {
        initializing = m;
    }

    void SmbiosTable::clearItemCache() const
    {
        // Delete everything in itemList
        std::map < const void *, ISmbiosItem * >::iterator position;
        for (position = itemList.begin ();
                position != itemList.end (); ++position)
        {
            delete position->second;
        }
        // clear the item list.
        itemList.clear();
    }

    // Restrict the validation of table entry point
    void SmbiosTable::setStrictValidationMode(bool mode) const
    {
        strictValidationMode = mode;
    }

    bool SmbiosTable::getStrictValidationMode() const
    {
        return strictValidationMode;
    }

    // Get an Item
    ISmbiosItem &SmbiosTable::makeItem(
        const void *header) const
    {
        const smbios_structure_header *structure =
            reinterpret_cast<const smbios_structure_header *>(header);
        ISmbiosItem *item = new SmbiosItem( structure );
        if( ! initializing )
        {
            dynamic_cast<SmbiosItem*>(item)->fixup( workaround.get() );
        }
        return *item;
    }

    const ISmbiosItem & SmbiosTable::getSmbiosItem (const u8 *current) const
    {
        return const_cast<SmbiosTable *>(this)->getSmbiosItem(current);
    }

    ISmbiosItem & SmbiosTable::getSmbiosItem (const u8 *current)
    {
        if (0 == current)
        {
            throw ItemNotFoundImpl("Could not de-reference a null item");
        }

        ISmbiosItem *item = this->getCachedItem( current );
        if ( 0 != item )
            return *(item);

        ISmbiosItem &newitem = this->makeItem( current );

        this->cacheItem( current, newitem );

        return newitem;
    }

    const u8 *SmbiosTable::nextSmbiosStruct (const u8* current) const
    {
        const smbios_structure_header *currStruct =
            reinterpret_cast<const smbios_structure_header *>(current);
        const u8 *data = 0;

        //If we are called on an uninitialized smbiosBuffer, return 0;
        if (0 == smbiosBuffer || (currStruct && 0x7f == currStruct->type))
            goto out1;

        data = smbiosBuffer;

        // currStruct == 0, that means we return the first struct
        if (0 == currStruct)
            goto out1;

        // start out at the end of the currStruct structure.
        // The only things that sits between us and the next struct
        // are the strings for the currStruct structure.
        data = reinterpret_cast<const u8 *>(currStruct) + currStruct->length;

        // skip past strings at the end of the formatted structure,
        // go until we hit double NULL "\0"
        // add a check to make sure we don't walk off the buffer end
        // for broken BIOSen.
        // The (3) is to take into account the deref at the end "data[0] ||
        // data[1]", and to take into account the "data += 2" on the next line.
        while (((data - smbiosBuffer) < (table_header.dmi.table_length - 3)) && (*data || data[1]))
            data++;

        // ok, skip past the actual double null.
        data += 2;

        // add code specifically to work around crap bios implementations
        // that do not have the _required_ 0x7f end-of-table entry
        //   note: (4) == sizeof a std header.
        if ( (data - smbiosBuffer) > (table_header.dmi.table_length - 4))
        {
            // really should output some nasty message here... This is very
            // broken
            data = 0;
            goto out1;
        }

out1:
        return data;
    }


    int SmbiosTable::getNumberOfEntries () const
    {
        return table_header.dmi.table_num_structs;
    }

    smbiosLowlevel::smbios_table_entry_point SmbiosTable::getTableEPS() const
    {
        return table_header;
    }

    ostream & SmbiosTable::streamify(ostream & cout) const
    {
        cout << "\nSMBIOS table " << endl;
        cout << "\tversion    : ";
        cout << static_cast<int>(table_header.major_ver) << ".";
        cout << static_cast<int>(table_header.minor_ver) << endl;
        cout << hex ;
        cout << "\taddress    : " << table_header.dmi.table_address << endl;
        cout << dec;
        cout << "\tlength     : " << table_header.dmi.table_length << endl;
        cout << "\tnum structs: " << table_header.dmi.table_num_structs << endl;
        cout << endl;

        SmbiosTable::const_iterator position = begin();
        while (position != end())
        {
             cout << *position << endl;
            ++position;
        }
        return cout;
    }

    ostream & operator << (ostream & cout, const ISmbiosTable & table)
    {
        table.streamify( cout );
        return cout;
    }

}
