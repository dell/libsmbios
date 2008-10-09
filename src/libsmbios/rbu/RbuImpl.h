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
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 */

#ifndef RBUIMPL_H
#define RBUIMPL_H

#if defined(DEBUG_SYSINFO)
#define DEBUG_OUTPUT_ALL
#endif

// compat header should always be first header if including system headers
#include "smbios/compat.h"

#include "smbios/DellRbu.h"
#include "FactoryImpl2.h"
#include "ExceptionImpl.h"
#include "smbios/RbuLowLevel.h"

namespace rbu
{
    DEFINE_EXCEPTION_EX( RbuNotSupportedImpl, rbu, RbuNotSupported );
    DEFINE_EXCEPTION_EX( InvalidHdrFileImpl, rbu, InvalidHdrFile );
    DEFINE_EXCEPTION_EX( UnsupportedSystemForHdrFileImpl, rbu, UnsupportedSystemForHdrFile );
    DEFINE_EXCEPTION_EX( HdrFileIOErrorImpl, rbu, HdrFileIOError );
    DEFINE_EXCEPTION_EX( RbuDriverIOErrorImpl, rbu, RbuDriverIOError );

    class RbuFactoryImpl : public factory::TFactory<RbuFactory>
    {
    public:
        RbuFactoryImpl();
        virtual ~RbuFactoryImpl() throw();
        virtual IRbuHdr *makeNew( std::string filename );
    };


    class RbuHdr : public virtual IRbuHdr
    {
    public:
        // CONSTRUCTORS, DESTRUCTOR, and ASSIGNMENT
        explicit RbuHdr(std::string filename);

        // CONSTRUCTORS, DESTRUCTOR, and ASSIGNMENT
        virtual ~RbuHdr ();

        virtual std::ostream & streamify(std::ostream & cout ) const;

        virtual std::string getBiosVersion() const;
        virtual void getHdrVersion(unsigned int &major, unsigned int &minor) const;
        virtual const u32 *getSystemIdList() const;
        virtual void doUpdate() const;
        virtual FILE *getFh() const;

    protected:
        // No-arg constructor not legal for this class for regular users
        RbuHdr ();

    private:
        RbuHdr (const RbuHdr &source);
        FILE *hdrFh;
        struct rbu_hdr_file_header header;
        u32 sysIdList[NUM_SYS_ID_IN_HDR + 1]; // zero terminated array of system ids.
    };

    packet_type getSupportedPacketType(void);
    void activateRbuToken();
    void cancelRbuToken();
    void checksumPacket(rbu_packet *pkt, size_t size);
    void createPacket(char *buffer, size_t bufSize, size_t imageSize);
}

#endif /* RBUIMPL_H */
