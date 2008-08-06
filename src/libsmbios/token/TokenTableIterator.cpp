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
#include "TokenImpl.h"

using namespace std;

namespace smbios
{
    ConstTokenTableIterator::ConstTokenTableIterator (const ITokenTable * initialTable, int typeToMatch)
                        : TokenTableIteratorBase( initialTable, typeToMatch ) 
    {}

    TokenTableIterator::TokenTableIterator (const ITokenTable *initialTable, int typeToMatch)
                          : TokenTableIteratorBase( initialTable, typeToMatch ) 
    {}


    TokenTableIteratorBase::TokenTableIteratorBase (const ITokenTable *initialTable, int typeToMatch)
            :matchType(typeToMatch), table(initialTable), current(-1)
    {
        if( table == 0 )
            current = -2;

        incrementIterator();
    }

    void TokenTableIteratorBase::reset()
    { 
        current=0; 
        incrementIterator(); 
    }
    
    bool TokenTableIteratorBase::eof()
    {
        return (current < 0);
    }

    IToken& TokenTableIterator::operator * () const 
    {
        return *(const_cast<TokenTableIterator *>(this)->dereference());
    }

    IToken* TokenTableIterator::operator -> () const 
    {
        return const_cast<TokenTableIterator *>(this)->dereference();
    }

    const IToken& ConstTokenTableIterator::operator * () const 
    {
        return *dereference();
    }

    const IToken*   ConstTokenTableIterator::operator -> () const 
    {
        return dereference();
    }


    const IToken * TokenTableIteratorBase::dereference () const
    {
        return const_cast<TokenTableIteratorBase *>(this)->dereference();
    }

    IToken * TokenTableIteratorBase::dereference ()
    {
        const TokenTable *CTTable = dynamic_cast<const TokenTable *>(table);
        if( current >= 0 && static_cast<unsigned int>(current) >= CTTable->tokenList.size() )
            current = -2;   // should _never_ get here.
        if( current > -1 )
        {
            return CTTable->tokenList[current] ;
        }
        throw DerefNullPointerImpl("tried to dereference non-existent token");
    }

    void TokenTableIteratorBase::incrementIterator()
    {
        if( current == -2 )
            return;

        const TokenTable *CTTable = dynamic_cast<const TokenTable *>(table);
        size_t size = CTTable->tokenList.size();
        do
        {
            ++current;
        }
        while(
            matchType != -1 &&
            current >= 0  &&
            static_cast<unsigned int>(current) < size &&
            CTTable->tokenList[current]->getType() != static_cast<u32>(matchType)
        );

        // don't overflow the size of the table.
        // be careful about signed vs unsigned comparisons.
        if( current >= 0 && static_cast<unsigned int>(current) >= size )
            current = -2;

        return;
    }

    //Prefix ++
    TokenTableIterator & TokenTableIterator::operator ++ ()
    {
        if( current > -1 )
            incrementIterator();
        return *this;
    }

    //Postfix ++
    const TokenTableIterator TokenTableIterator::operator ++ (int)
    {
        const TokenTableIterator oldValue = *this;
        ++(*this);
        return oldValue;
    }

    //Prefix ++
    ConstTokenTableIterator & ConstTokenTableIterator::operator ++ ()
    {
        if( current > -1 )
            incrementIterator();
        return *this;
    }

    //Postfix ++
    const ConstTokenTableIterator ConstTokenTableIterator::operator ++ (int)
    {
        const ConstTokenTableIterator oldValue = *this;
        ++(*this);
        return oldValue;
    }

}
