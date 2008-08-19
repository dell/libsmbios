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

#include <sstream>

#include "TokenImpl.h"

using namespace std;

// for functions that are not implemented at this level in the
// inheritance heirarchy at all.
#define NOT_IMPLEMENTED do { throw NotImplementedImpl(); } while(0)

// Token numbering:
//   -- all tokens are 16-bit, this means we can store token number in 32-bit
//      value and use topmost bits as a 'type' code.

namespace smbios
{
    ITokenTable::ITokenTable()
    {}

    //
    //
    TokenTable::TokenTable( const smbios::ISmbiosTable & table)
    {
        addD4Structures(table);
        addD5Structures(table);
        addD6Structures(table);
        addDAStructures(table);
    }

    void TokenTable::addD4Structures(const smbios::ISmbiosTable & table)
    {
        // usually have around 4 checksums or so. estimate here to speed up stuff later.
        checksumList.reserve(4);

        // Pull out all of the 0xD4 entries and create Tokens out of them
        // iterate over each 0xD4 token (Dell_Indexed_Io)
        for(
            smbios::ISmbiosTable::const_iterator item = table[DellIndexedIoTokenType];
            item != table.end();
            ++item)
        {
            // get a copy of the buffer
            size_t size = 0;
            const u8 *ptr =  item->getBufferCopy(size) ; // MUST DELETE[]!
            try
            {
                addChecksumObserverForD4Struct( item, ptr, size );
                getD4TokensFromStruct( item, ptr, size );

            }
            // loathe C++. Why in the heck is there no finally()?
            // oh yeah, that would be Stroustrup again. :-(
            catch ( const std::exception & )
            {
                // make sure this is always in sync with below
                delete [] const_cast<u8*>(ptr);
                ptr = 0;
                throw;
            }
            // make sure this is always in sync with the above.
            delete [] const_cast<u8*>(ptr);
            ptr = 0;
        }
    }

    void TokenTable::addChecksumObserverForD4Struct(const smbios::ISmbiosTable::const_iterator &item, const u8 *ptr, size_t size)
    {
        ostringstream ost;
        ost << *item;
        (void) size; // avoid unused param warning

        //  cast buffer to 'indexed_io_access' struct to make
        //  life easier.
        const indexed_io_access_structure *io_struct =
            reinterpret_cast<const indexed_io_access_structure *>(ptr);

        // no need to delete, it is a singleton.
        //cout << "Adding checksum observer" << endl;
        cmos::ICmosRW *cmos = cmos::CmosRWFactory::getFactory()->getSingleton();
        CmosRWChecksumObserver chk(
            ost.str(),
            cmos,
            io_struct->checkType,
            io_struct->indexPort,
            io_struct->dataPort,
            io_struct->checkedRangeStartIndex,
            io_struct->checkedRangeEndIndex,
            io_struct->checkValueIndex  );
        checksumList.push_back( chk );
    }

    void TokenTable::getD4TokensFromStruct(const smbios::ISmbiosTable::const_iterator &item, const u8 *ptr, size_t size)
    {
        //  cast buffer to 'indexed_io_access' struct to make
        //  life easier.
        const indexed_io_access_structure *io_struct =
            reinterpret_cast<const indexed_io_access_structure *>(ptr);

        // now iterate over each token in the structure
        //   here, the last item in the struct is "first_token", which
        //   is really just a pointer to the first in an array.
        int count = 0;
        const indexed_io_token *iterToken;
        iterToken = &(io_struct->first_token);
        for(
            int i = 0;
            (iterToken[i].tokenId != TokenTypeEOT);
            ++i
        )
        {
            // work arounds for astyle bug (temp vars instead of doing
            // this all in if()):
            const u8 *cur_loc = reinterpret_cast<const u8 *>(&iterToken[i]);
            const u8 *struct_base_loc = reinterpret_cast<const u8 *>(io_struct);
            if ( cur_loc >= struct_base_loc + size )
            {
#if LIBSMBIOS_VERBOSE_BIOS_BUG_COMPLAINTS
                cout << "FATAL: iterToken > size.  Missing EOT, has the world gone mad?" << endl;
#endif
                break;
            }
            //instantiate the token
            CmosTokenD4 *tk = new CmosTokenD4( (*item), &iterToken[i] );
            tokenList.push_back( tk ); // pushes a copy
            ++ count;
        }
    }

    void TokenTable::addD5Structures(const smbios::ISmbiosTable & table)
    {
        // Pull out all of the 0xD5 entries and create Tokens out of them
        // iterate over each 0xD5 token (protected value 1)
        for(
            smbios::ISmbiosTable::const_iterator item = table[DellProtectedAreaType1];
            item != table.end();
            ++item)
        {
            //instantiate the token
            CmosTokenD5 *tk = new CmosTokenD5( (*item), checksumList );
            tokenList.push_back( tk ); // pushes a copy
        }
    }

    void TokenTable::addD6Structures(const smbios::ISmbiosTable & table)
    {
        // Pull out all of the 0xD6 entries and create Tokens out of them
        // iterate over each 0xD6 token (protected value 1)
        for(
            smbios::ISmbiosTable::const_iterator item = table[DellProtectedAreaType2];
            item != table.end();
            ++item)
        {
            //instantiate the token
            CmosTokenD6 *tk = new CmosTokenD6( (*item), checksumList );
            tokenList.push_back( tk ); // pushes a copy
        }
    }

    void TokenTable::addDAStructures(const smbios::ISmbiosTable & table)
    {
        // Pull out all of the 0xDA entries and create Tokens out of them
        // iterate over each 0xDA token (Dell_Indexed_Io)
        for(
            smbios::ISmbiosTable::const_iterator item = table[DellCallingInterface];
            item != table.end();
            ++item)
        {
            // get a copy of the buffer
            size_t size = 0;
            const u8 *ptr =  item->getBufferCopy(size) ; // MUST DELETE[]!
            try
            {
                getDATokensFromStruct( item, ptr, size );

            }
            // loathe C++. Why in the heck is there no finally()?
            // oh yeah, that would be Stroustrup again. :-(
            catch ( const std::exception & )
            {
                // make sure this is always in sync with below
                delete [] const_cast<u8*>(ptr);
                ptr = 0;
                throw;
            }
            // make sure this is always in sync with the above.
            delete [] const_cast<u8*>(ptr);
            ptr = 0;
        }
    }

    void TokenTable::getDATokensFromStruct(const smbios::ISmbiosTable::const_iterator &item, const u8 *ptr, size_t size)
    {
        //  cast buffer to 'calling_interface_structure' struct to make
        //  life easier.
        const calling_interface_structure *structure =
            reinterpret_cast<const calling_interface_structure *>(ptr);

        // now iterate over each token in the structure
        //   here, the last item in the struct is "first_token", which
        //   is really just a pointer to the first in an array.
        int count = 0;
        const calling_interface_token *iterToken;
        iterToken = &(structure->first_token);
        for(
            int i = 0;
            (iterToken[i].tokenId != TokenTypeEOT);
            ++i
        )
        {
            // work arounds for astyle bug (temp vars instead of doing
            // this all in if()):
            const u8 *cur_loc = reinterpret_cast<const u8 *>(&iterToken[i]);
            const u8 *struct_base_loc = reinterpret_cast<const u8 *>(structure);
            if ( cur_loc + sizeof(calling_interface_token) >= struct_base_loc + size )
            {
#if LIBSMBIOS_VERBOSE_BIOS_BUG_COMPLAINTS
                // BIOS violates spec.
                if(i>0)
                    cout << endl << "FATAL: iterToken > size.  Missing EOT, has the world gone mad?" << endl << *item << endl;
#endif

                break;
            }
            //instantiate the token
            //cout
            //    << "TokenId: " << hex << (int)iterToken[i].tokenId << "  "
            //    << "location: " << hex << (int)iterToken[i].location << "  "
            //    << "value: " << hex << (int)iterToken[i].value << endl;

            SmiTokenDA *tk = new SmiTokenDA( (*item), &iterToken[i] );
            tokenList.push_back( tk ); // pushes a copy
            ++ count;
        }
    }

    TokenTable::~TokenTable( )
    {
        std::vector< IToken *>::iterator token;
        for( token = tokenList.begin(); token != tokenList.end(); ++token )
        {
            delete *token;
        }
    }

    ITokenTable::~ITokenTable( )
    {}

    TokenTable::iterator TokenTable::begin ()
    {
        return TokenTable::iterator (this);
    }

    TokenTable::const_iterator TokenTable::begin () const
    {
        return TokenTable::const_iterator (this);
    }

    TokenTable::iterator TokenTable::end ()
    {
        return TokenTable::iterator ();
    }

    TokenTable::const_iterator TokenTable::end ()const
    {
        return TokenTable::const_iterator ();
    }

    TokenTable::iterator TokenTable::operator[] (const int type)
    {
        return TokenTable::iterator (this, type);
    }

    TokenTable::const_iterator TokenTable::operator[](const int type) const
    {
        // this == const TokenTable();
        return TokenTable::const_iterator (this, type);
    }

    TokenTable::iterator TokenTable::operator[] (const string &)
    {
        NOT_IMPLEMENTED;
    }

    TokenTable::const_iterator TokenTable::operator[](const string &) const
    {
        NOT_IMPLEMENTED;
    }


    ostream & TokenTable::streamify(ostream & cout) const
    {
        cout << "Token Table";
        return cout;
    }

    std::ostream & operator << (std::ostream & cout, const ITokenTable & table)
    {
        return table.streamify(cout);
    }
}
