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

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <unistd.h>
#include <cstdio>

#include "RbuImpl.h"

// always include last if included.
#include "smbios/message.h"  // not needed outside of this lib. (mainly for gettext i18n)

#include <fcntl.h>
#include <sys/mman.h>
#define XSVCIOC 		('Q' << 8)
#define XSVC_ALLOC_MEM 		(XSVCIOC | 130)

#ifdef _LP64
typedef struct _xsvc_mem_req {
	int		xsvc_mem_reqid;
	uint64_t	xsvc_mem_addr_lo;
	uint64_t	xsvc_mem_addr_hi;
	uint64_t	xsvc_mem_align;
	int		xsvc_mem_sgllen;
	size_t		xsvc_mem_size;
	void		*xsvc_sg_list;
} xsvc_mem_req;
typedef struct _xsvc_mloc {
	uint64_t	mloc_addr;
	size_t		mloc_size;
} xsvc_mloc;
#else
typedef struct _xsvc_mem_req {
	uint64_t	xsvc_mem_reqid;
	uint64_t	xsvc_mem_addr_lo;
	uint64_t	xsvc_mem_addr_hi;
	uint64_t	xsvc_mem_align;
	int		xsvc_mem_sgllen;
	uint32_t	xsvc_mem_size;
	uint32_t	xsvc_sg_list;
} __attribute__((packed)) xsvc_mem_req;
typedef struct _xsvc_mloc32 {
	uint64_t	mloc_addr;
	size_t		mloc_size;
} xsvc_mloc;
#endif

using namespace std;

namespace rbu
{

    driver_type getDriverType()
    {
	return rbu_solaris;
    }


    static void doSolarisUpdate(FILE *hdr_fh, int pt)
    {
	xsvc_mem_req mrq = { 0 };
	xsvc_mloc ml;
	size_t totalBytes;
	void *ptr;
	int fd, pgsize;

	pgsize = getpagesize();
	fseek(hdr_fh, 0, SEEK_END);
	totalBytes = ftell(hdr_fh);

	totalBytes = (totalBytes + 4095) & ~4095;
	fd = open("/dev/xsvc", O_RDWR);
	if (fd < 0)
	{
		cout << "failed to open xsvc" << endl;
		return;
	}
	/* Allocate a chunk of physical memory < 4Gb */
     	mrq.xsvc_mem_reqid = 0xDE11B105;
	mrq.xsvc_mem_size = totalBytes;
	mrq.xsvc_mem_addr_lo = 0;
	mrq.xsvc_mem_addr_hi = 0xFFFFFFFFL;
	mrq.xsvc_mem_sgllen = 1;
	mrq.xsvc_mem_align = 4096;
  	mrq.xsvc_sg_list = (intptr_t)&ml;
	if (ioctl(fd, XSVC_ALLOC_MEM, &mrq) < 0)
	{
		cout << "xsvc ioctl fails" << endl;
		return;
	}

	/* mmap physical memory and read BIOS header into buffer */
	ptr = mmap(0, totalBytes, PROT_WRITE, MAP_SHARED, fd, ml.mloc_addr);
	if (ptr == (void *)-1LL) {
		cout << "mmap fails" << endl;
		return;
	}
	fseek(hdr_fh, 0, SEEK_SET);
	fread(ptr, 1, totalBytes, hdr_fh);
    }

/*****************************************************************************
******************************************************************************

main entry points for this module.

******************************************************************************
*****************************************************************************/

    void dellBiosUpdate(const IRbuHdr &hdr, packet_type force_type)
    {
        FILE *hdr_fh = hdr.getFh();
        fseek(hdr_fh, 0, 0);

        bool forced=false;

        // TODO: verify that it is a HDR file
        // TODO: checksum HDR file

        packet_type supported_pt = getSupportedPacketType();
        cout << "Supported RBU type for this system: (MONOLITHIC"
            << (supported_pt == pt_packet ? ", PACKET"   : "")
            << ")"
            << endl;

        if( force_type != pt_any )
        {
            supported_pt = force_type;
            forced = true;
        }

        driver_type dt = getDriverType();

	if (dt == rbu_solaris)
	{
            cout << "Using RBU Solaris driver. Initializing Driver. " << endl;
	    doSolarisUpdate(hdr_fh, supported_pt);
	}
        else
        {
            throw RbuNotSupportedImpl("Could not open Dell RBU driver.");
        }

        cout << "Activate CMOS bit to notify BIOS that update is ready on next boot." << endl;
        activateRbuToken();

        cout << "Update staged sucessfully. BIOS update will occur on next reboot." << endl;
    }

    void cancelDellBiosUpdate()
    {
        // FOR LOAD CANCEL:
        // Step 1: always unset CMOS first
        cout << "Cancel BIOS CMOS notification bit." << endl;
        cancelRbuToken();

        driver_type dt = getDriverType();
        switch(dt)
        {
	case rbu_solaris:
            cout << "Re-initialize driver for next user." << endl;
            break;


        default:
            cout << "Could not determine RBU driver present, skipping." << endl;
            break;
        }
    }
}

