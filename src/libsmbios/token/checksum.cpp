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

#include "CmosRWImpl.h"
#include "TokenImpl.h"

// message.h should be included last.
#include "smbios/message.h"

using namespace std;
using namespace cmos;

namespace smbios
{
    static u8 byteChecksum(
        const ICmosRW *cmos, u32 start, u32 end,
        u32 indexPort, u32 dataPort );
    static u16 wordChecksum(
        const ICmosRW *cmos, u32 start, u32 end,
        u32 indexPort, u32 dataPort, bool complement );
    static u16 wordCrc(
        const ICmosRW *cmos, u32 start, u32 end,
        u32 indexPort, u32 dataPort );


    // REGULAR CONSTRUCTOR
    CmosRWChecksumObserver::CmosRWChecksumObserver(
        string initDesc,
        ICmosRW *initCmos,
        int initCheckType, u32 initIndexPort, u32 initDataPort,
        u32 initStart, u32 initEnd, u32 initChecksumLocation  )
            :
            IObserver(),
            description(initDesc),
            cmos(initCmos),
            checkType(initCheckType),
            indexPort(initIndexPort),
            dataPort(initDataPort),
            start(initStart),
            end(initEnd),
            checksumLocation(initChecksumLocation)
    {
        observer::IObservable *ob = dynamic_cast<observer::IObservable*>(cmos);
        if( ob )
            ob->attach(this);
    }

    // COPY CONSTRUCTOR
    CmosRWChecksumObserver::CmosRWChecksumObserver( const CmosRWChecksumObserver &source )
            :
            IObserver(),
            description(source.description),
            cmos(source.cmos),
            checkType(source.checkType),
            indexPort(source.indexPort),
            dataPort(source.dataPort),
            start(source.start),
            end(source.end),
            checksumLocation(source.checksumLocation)
    {
        observer::IObservable *ob = dynamic_cast<observer::IObservable*>(cmos);
        if( ob )
            ob->attach(this);
    }

    // DESTRUCTOR
    CmosRWChecksumObserver::~CmosRWChecksumObserver()
    {
        observer::IObservable *ob = dynamic_cast<observer::IObservable*>(cmos);
        if( ob )
            ob->detach(this);
    }

    void CmosRWChecksumObserver::update( const observer::IObservable *whatChanged, void *doUpdate )
    {
        const ICmosRW *cmos = dynamic_cast<const ICmosRW *>(whatChanged);

        const u8 *chksum = 0;
        u16 wordRetval = 0;

        // optimize for the common case
        u8 len = sizeof(wordRetval);
        chksum = reinterpret_cast<const u8 *>(&wordRetval);

        // set up for the case where we throw an exception
        ostringstream ost;
        ost << hex ;
        ost << description << endl;
        ost << _("Checksum check for CMOS value does not match.") << endl;
        InvalidChecksumImpl invalidChecksum;

        // All zeros indicates that this range is not checksummed
        // icky to keep vc.net /w4 happy
        if( !(start || end || checksumLocation))
            return;

        switch( checkType )
        {
        case CHECK_TYPE_BYTE_CHECKSUM:
            ost << _("SMBIOS-specified checksum type is Byte Checksum. Type %(byte_chksum_type)i") << endl;
            // works because we are little endian.
            wordRetval = byteChecksum(cmos, start, end, indexPort, dataPort);
            len = sizeof(u8);
            break;
        case CHECK_TYPE_WORD_CHECKSUM:
            ost << _("SMBIOS-specified checksum type is Word Checksum. Type %(word_chksum_type)i") << endl;
            wordRetval = wordChecksum(cmos, start, end, indexPort, dataPort, false);
            break;
        case CHECK_TYPE_WORD_CHECKSUM_N:
            ost << _("SMBIOS-specified checksum type is One's Complement Word Checksum. Type %(word_chksum_n_type)i") << endl;
            wordRetval = wordChecksum(cmos, start, end, indexPort, dataPort, true);
            break;
        case CHECK_TYPE_WORD_CRC:
            ost << _("SMBIOS-specified checksum type is Word CRC. Type %(word_crc_type)i") << endl;
            wordRetval = wordCrc(cmos, start, end, indexPort, dataPort);
            break;
        default:
            ostringstream chkost;
            chkost << hex;
            chkost << _("Unknown checksum type encountered: ");
            chkost << static_cast<int>(checkType);
            throw smbios::Exception<smbios::IException>( chkost.str() );
        }

        u32 actualChksum = 0;
        u32 calculatedChksum = 0;
        for( int i=0; i<len; ++i )
        {
            u8 byte = cmos->readByte(indexPort, dataPort, checksumLocation+i);
            actualChksum = (actualChksum << 8) | byte;
            calculatedChksum = calculatedChksum |  (chksum[i] << (8*i));
        }

        // only write new checksum if it doesn't match what is already there
        if (actualChksum == calculatedChksum)
            goto out;

        // if NULL parameter passed, or if parameter not null and evaluates to TRUE
        if( !doUpdate || *static_cast<bool*>(doUpdate) )
        {
            const cmos::Suppressable *o = dynamic_cast<const cmos::Suppressable *>(cmos);
            o->suppressNotification(true);
            for( int i=0; i<len; ++i )
            {
                cmos->writeByte(
                    indexPort, dataPort,
                    checksumLocation+i, chksum[len -i -1]);
            }
            o->resumeNotification(true);
        }
        else
        {
            ost << _("Checking alternate checksum algorithm results.") << endl
            << _("Calculated (Type %(word_chksum_type)i) word checksum is: %(calc_word)i") << endl
            << _("Calculated (Type %(byte_chksum_type)i) byte checksum is: %(calc_byte)i") << endl
            << _("Calculated (Type %(word_crc_type)i) word crc is: %(calc_crc)i") << endl
            << _("Calculated (Type %(word_chksum_n_type)i) 1's complement word checksum is: %(calc_word_n)i") << endl
            << _("Actual data value is: %(actual)i") << endl
            << _("Calculated data value is: %(calc)i") << endl
            << _("Start: %(start)i") << endl
            << _("End: %(end)i") << endl
            << _("Checksum Loc: %(checksumLocation)i") << endl
            << _("Index Port: %(index)i") << endl
            << _("Data Port: %(data)i") << endl;

            invalidChecksum.setParameter( "byte_chksum_type", CHECK_TYPE_BYTE_CHECKSUM );
            invalidChecksum.setParameter( "word_chksum_type", CHECK_TYPE_WORD_CHECKSUM );
            invalidChecksum.setParameter( "word_chksum_n_type", CHECK_TYPE_WORD_CHECKSUM_N );
            invalidChecksum.setParameter( "word_crc_type", CHECK_TYPE_WORD_CRC );
            invalidChecksum.setParameter("calc_byte", byteChecksum(cmos, start, end, indexPort, dataPort));
            invalidChecksum.setParameter("calc_word", wordChecksum(cmos, start, end, indexPort, dataPort, false));
            invalidChecksum.setParameter("calc_word_n", wordChecksum(cmos, start, end, indexPort, dataPort, true));
            invalidChecksum.setParameter("calc_crc", wordCrc(cmos, start, end, indexPort, dataPort));
            invalidChecksum.setParameter("actual", actualChksum);
            invalidChecksum.setParameter("calc", calculatedChksum);
            invalidChecksum.setParameter("start", start);
            invalidChecksum.setParameter("end", end);
            invalidChecksum.setParameter("checksumLocation", checksumLocation);
            invalidChecksum.setParameter("index", indexPort);
            invalidChecksum.setParameter("data", dataPort);

            invalidChecksum.setMessageString( ost.str() );
            throw invalidChecksum;
        }
out:
        return;
    }

    /*******************
    * Checksum functions
    *******************/


    static u8 byteChecksum(
        const ICmosRW *cmos, u32 start, u32 end,
        u32 indexPort, u32 dataPort )
    {
        u8 running_checksum=0;

        for( u32 i = start; i <= end; i++)
        {
            // not += to keep vc.net /w4 happy
            running_checksum = running_checksum + cmos->readByte( indexPort, dataPort, i );
        }

        return static_cast<u8>(running_checksum);
    }


    static u16 wordChecksum(
        const ICmosRW *cmos, u32 start, u32 end,
        u32 indexPort, u32 dataPort, bool complement )
    {
        u16 running_checksum=0;

        for( u32 i = start; i <= end; i++)
        {
            // not += to keep vc.net /w4 happy
            running_checksum = running_checksum + cmos->readByte( indexPort, dataPort, i );
        }

        if( complement )
            running_checksum = (~running_checksum) + 1;
        return running_checksum;
    }


    static u16 wordCrc(
        const ICmosRW *cmos, u32 start, u32 end,
        u32 indexPort, u32 dataPort )
    {
        u16 running_crc=0;

        for( u32 i = start; i <= end; i++)
        {
            running_crc ^= cmos->readByte( indexPort, dataPort, i );

            for( int j=0; j<7; j++ )
            {
                u16 temp = running_crc & 0x0001;
                running_crc >>= 1;
                if( temp != 0 )
                {
                    running_crc |= 0x8000;
                    running_crc ^= 0xA001;
                }
            }
        }

        return running_crc;
    }

}

