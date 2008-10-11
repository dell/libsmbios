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

#include <iomanip>
#include <string.h>

#include "SmbiosImpl.h"
// message.h should be included last.
#include "smbios/message.h"

using namespace smbiosLowlevel;
using namespace std;

namespace smbios
{
    ISmbiosItem::~ISmbiosItem()
    {}

    ISmbiosItem::ISmbiosItem()
    {}

    //
    // COPY CONSTRUCTOR
    //
    SmbiosItem::SmbiosItem (const SmbiosItem & source)
            : ISmbiosItem(), header (source.header), header_size(source.header_size)
    {
        // only one allocation here. If it fails, there is
        // nothing to rollback, so we are exception safe.
        // if we add another allocation, we need to restructure this.
        u8 *newSmbiosItem = new u8[ header_size ];
        memcpy (newSmbiosItem, source.header, header_size);
        header = reinterpret_cast<const smbios_structure_header *>(newSmbiosItem);

        // don't deref header if it isn't valid
        // the world is not right if this happens.
        if (!header)
        {
            InternalErrorImpl internalError;
            internalError.setMessageString(_("Not a valid header. header is zero."));
            throw internalError;
        }

    }

    //
    // REGULAR CONSTRUCTOR
    //
    SmbiosItem::SmbiosItem (const smbios_structure_header *init_header)
            : ISmbiosItem(), header(init_header), header_size(0)
    {
        // we need to copy all of our data out of the SmbiosTable
        // so that we have our own copy. This effectively lets
        // smbiosItem have a separate lifetime from it's containing
        // table.
        //
        // we do all of the stuff below, up to the "new", to figure
        // out the size of the item.

        // don't deref header if it isn't valid
        // the world is not right if this happens.
        if (!header)
        {
            InternalErrorImpl internalError;
            internalError.setMessageString(_("Not a valid header. header is zero."));
            throw internalError;
        }

        // hop over to the next struct using smbios parsing rules
        // see smbiostable code for more details
        const u8 *nextStruct = reinterpret_cast<const u8 *>(header)  + header->length ;

        // skip over the strings in the string table. It ends with a double null
        while (*nextStruct || nextStruct[1])
            nextStruct++;

        // skip over the actual double null
        nextStruct += 2;

        // now we are at the next struct, we know the size
        // of the struct we are supposed to be pointing at.
        header_size = nextStruct - reinterpret_cast<const u8 *>(header);

        // only one allocation here. If it fails, there is
        // nothing to rollback, so we are safe.
        // if we add another allocation, we need to restructure this.
        u8 *newSmbiosItem = new u8[header_size];
        memcpy (newSmbiosItem, header, header_size);
        header = reinterpret_cast<const smbios_structure_header *>(newSmbiosItem);
    }

    SmbiosItem::~SmbiosItem ()
    {
        // its ugly because microsoft vc++ 6 sucks so much.
        delete [] const_cast<u8 *>(reinterpret_cast<const u8 *>(header));
        header = 0;
    }

    // gcc workaround. overprotective git. #!#$J@*(&$%
    // This is only used to format informational output stuff.
    // it loses precision, so don't do it except for display
    static u32 force_u64_to_u32(u64 orig)
    {
        // only gives correct results for little endian (I think)
        // but shouldn't matter, as it is for information purposes only.
        // 
        // use union to fix gcc type-punned pointer warning
        union 
        {
            u64 orig;
            u32 recast;
        } temp;

        temp.orig = orig;
        return temp.recast;
    }

    // Lifetime of returned char* is the same as the SmbiosItem
    // FIXME: This function needs to make sure it doesn't run past the end of the table for
    // malformed strings.
    //
    // Invariant: will always return a valid const char * or throw an exception
    const char *SmbiosItem::getStringByStringNumber (u8 which) const
    {
        const char *string_pointer = reinterpret_cast<const char *>(header);

        //  either user is an idiot and should go read the spec,
        //  _or_ there is a '\0' in the table, indicating
        //  the string does not exist. :-(
        //  In either case, we throw an exception.
        if (!which)     //strings are numbered beginning with 1
        {
            throw StringUnavailableImpl(_("String does not exist."));
        }

        // start out at the end of the header. This is where
        // the first string starts
        string_pointer += header->length;

        for (; which > 1; which--)
        {
            string_pointer += strlen (string_pointer);
            string_pointer++;  // skip past '\0'

            // check that we don't overflow this item
            //  additionally, split test into temp vars outside if() to work
            //  around astyle formatting bug where it will break code.
            const u8 *cur_loc = reinterpret_cast<const u8 *>(string_pointer);
            const u8 *base_loc =  reinterpret_cast<const u8 *>(header);
            if( cur_loc >= base_loc + header_size)
            {
                ParseExceptionImpl parseException;
                parseException.setMessageString(_("Overflow while getting byte data at location: cur_loc >= base_loc + header_size\n cur_loc : %(cur_loc)i\n base_loc : %(base_loc)i\n header_size : %(header_size)i "));
                parseException.setParameter("cur_loc",    force_u64_to_u32(reinterpret_cast<u64>(cur_loc)));
                parseException.setParameter("base_loc",   force_u64_to_u32(reinterpret_cast<u64>(base_loc)));
                parseException.setParameter("header_size",static_cast<u32>(header_size));
                throw parseException;
            }

            // if it is still '\0', that means we are
            // at the end of this item and should stop.
            // user gave us a bad index
            if( ! *string_pointer )
            {
                throw StringUnavailableImpl(_("The string does not exist. Bad index caused this error"));
            }
        }

        return string_pointer;
    }

    std::auto_ptr<ISmbiosItem> SmbiosItem::clone()
    {
        return auto_ptr<ISmbiosItem>(new SmbiosItem (*this));
    }

    std::auto_ptr<const ISmbiosItem> SmbiosItem::clone() const
    {
        return auto_ptr<const ISmbiosItem>(new SmbiosItem (*this));
    }

    u8 SmbiosItem::getType () const
    {
        return header->type;
    }

    u8 SmbiosItem::getLength () const
    {
        return header->length;
    }

    u16 SmbiosItem::getHandle () const
    {
        return header->handle;
    }

    void checkItemBounds( size_t total_size, size_t length, size_t offset, size_t size)
    {
        DataOutOfBoundsImpl dataOutOfBounds;
        dataOutOfBounds.setParameter("offset",static_cast<int>(offset));
        dataOutOfBounds.setParameter("header_length",static_cast<int>(total_size));

        // tricky.  Need all three tests here in this order to avoid security hole
        if( offset > length )
        {
            dataOutOfBounds.setMessageString(_("Attempt to access data outside the length of header. offset : %(offset)i, header_length : %(header_length)i"));
            throw dataOutOfBounds;
        }

        if( offset + size < offset )
        {
            dataOutOfBounds.setMessageString(_("Attempt to access data outside the length of header. offset : %(offset)i, header_length : %(header_length)i"));
            throw dataOutOfBounds;
        }

        if( offset + size > length )
        {
            dataOutOfBounds.setMessageString(_("Attempt to access data outside the length of header. offset : %(offset)i, header_length : %(header_length)i"));
            throw dataOutOfBounds;
        }

        if( offset >= total_size ) // world gone mad check.
            // data is inside what the header says is
            // the length, but outside the range of the
            // buffer we are using to hold the header.
            // Impossible?
        {
            dataOutOfBounds.setMessageString(_("Attempt to access data outside header buffer. Impossible situation! offset : %(offset)i, header_length : %(header_length)i"));
            throw dataOutOfBounds;
        }

    }

    void SmbiosItem::getData(unsigned int offset, u8 *out, size_t size ) const
    {
        checkItemBounds( header_size, header->length, offset, size );
        memcpy(out, reinterpret_cast<const u8 *>(header)+offset, size);
    }

    const u8 *SmbiosItem::getBufferCopy(size_t &size) const
    {
        size = header_size;

        const u8 *newBuffer = new u8[ size ];
        memcpy (const_cast<u8 *>(newBuffer), header, size);
        return newBuffer;
    }

    size_t SmbiosItem::getBufferSize() const
    {
        return header_size;
    }

    void SmbiosItem::fixup( const SmbiosWorkaroundTable *workaround ) const
    {
        u8 *buffer = const_cast<u8 *>(reinterpret_cast<const u8 *>(header));
        workaround->fixupItem( this, buffer, header_size );
    }

    ostream & SmbiosItem::streamify (ostream & cout) const
    {
        if (header == 0)  // violates class invariant, should never happen
            cout << "operator << on an uninitialized SmbiosItem!";
        else
        {
            std::ios::fmtflags old_opts = cout.flags ();
            cout << "Handle 0x" << hex << setfill ('0') <<
            setw (4) << getHandle () << endl;
            cout << "\tDMI type 0x" << static_cast<int>(getType()) << dec <<
            ", " << static_cast<int>(getLength()) << " bytes." <<
            endl;
            cout.flags (old_opts);
        }
        return cout;
    }


    /**************************************************************************
     *  OUT OF LINE HELPERS
     *************************************************************************/

    u8 getItemType(const ISmbiosItem &item)
    {
        return getU8_FromItem(item, 0);
    }

    u8 getItemLength(const ISmbiosItem &item)
    {
        return getU8_FromItem(item, 1);
    }

    u16 getItemHandle(const ISmbiosItem &item)
    {
        return getU16_FromItem(item, 2);
    }

    u8 getU8_FromItem(const ISmbiosItem &item, unsigned int offset)
    {
        u8 retval = 0;
        item.getData(offset, reinterpret_cast<u8 *>(&retval), sizeof(retval));
        return retval;
    }

    u16 getU16_FromItem(const ISmbiosItem &item, unsigned int offset)
    {
        u16 retval = 0;
        item.getData(offset, reinterpret_cast<u8 *>(&retval), sizeof(retval));
        return retval;
    }

    u32 getU32_FromItem(const ISmbiosItem &item, unsigned int offset)
    {
        u32 retval = 0;
        item.getData(offset, reinterpret_cast<u8 *>(&retval), sizeof(retval));
        return retval;
    }

    u64 getU64_FromItem(const ISmbiosItem &item, unsigned int offset)
    {
        u64 retval = 0;
        item.getData(offset, reinterpret_cast<u8 *>(&retval), sizeof(retval));
        return retval;
    }

    const char *getString_FromItem(const ISmbiosItem &item, unsigned int offset)
    {
        u8 stringNum = 0;
        getData(item, offset, stringNum);
        return item.getStringByStringNumber(stringNum);
    }

    void *getBits_FromItem ( const ISmbiosItem &item, unsigned int offset, void *out, unsigned int lsb, unsigned int msb )
    {
        u64 bitfield = 0;

        //If msb is less/equal to lsb, they are only requesting a single bit
        if(msb <= lsb)
            msb=lsb;

        if(msb > 63)
        {
            DataOutOfBoundsImpl dataOutOfBounds;
            dataOutOfBounds.setParameter("lsb",lsb);
            dataOutOfBounds.setParameter("msb",msb);
            dataOutOfBounds.setMessageString(_("The total length of bit field is out of bounds. The largest accessible bit is 63. lsb: %(lsb)i , msb: %(msb)i"));
            throw  dataOutOfBounds;
        }

        // calculate length of bit field based on msb/lsb
        unsigned int fieldLen = ((msb+1)/8) + (((msb+1)%8)?1:0);

        // request data from item
        item.getData(offset, reinterpret_cast<u8 *>(&bitfield), fieldLen);

        // mask off everything but requested bits and shift down
        unsigned int bitlen = (msb-lsb) + 1;
        bitfield = (bitfield >> lsb) & ((1<<bitlen)-1);

        if(out)
            memcpy(out, &bitfield, ((bitlen)/8) + (((bitlen)%8)?1:0));

        return out;
    }

    bool isBitSet(const ISmbiosItem *itemPtr, unsigned int offset, unsigned int bitToTest)
    {
        bool retval = false;

        unsigned int byte = bitToTest / 8;
        u8 fieldValue = getU8_FromItem(*itemPtr, offset + byte );
        if (fieldValue & (1 << (bitToTest%8)))
            retval = true;

        return retval;
    }

    ostream & operator << (ostream & cout, const ISmbiosItem & item)
    {
        return item.streamify (cout);
    }
}
