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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>        // fopen()
#include <string.h>
#include <stdlib.h>
#include <locale.h>
#include <libintl.h>

#include "smbios_c/obj/cmos.h"
#include "smbios_c/obj/memory.h"
#include "smbios_c/memory.h"
#include "smbios_c/obj/smbios.h"
#include "smbios_c/smbios.h"
#include "smbios_c/obj/token.h"
#include "smbios_c/system_info.h"

#include "getopts.h"

#define _(String) gettext(String)
#define gettext_noop(String) String
#define N_(String) gettext_noop (String)

const char *sysstrDumpFile = "sysstr.dat";
const char *smbiosDumpFile = "smbios.dat";
const char *idByteDumpFile = "idbyte.dat";
const char *cmosDumpFile = "cmos.dat";

void dump_smbios_table(const char *smbiosDumpFile);
void dumpMem( const char *fn, size_t offset, size_t len);
void dumpCmos(const struct smbios_struct *structure, void *userdata);

struct options opts[] =
    {
        { 253, "cmos_file", N_("Debug: CMOS dump file to use instead of physical cmos"), "c", 1 },
        { 254, "memory_file", N_("Debug: Memory dump file to use instead of physical memory"), "m", 1 },
        { 255, "version", N_("Display libsmbios version information"), "v", 0 },
        { 0, NULL, NULL, NULL, 0 }
    };

int
main (int argc, char **argv)
{
    int c=0;
    char *args = 0;

    setlocale(LC_ALL, "");
    bindtextdomain(GETTEXT_PACKAGE, LIBSMBIOS_LOCALEDIR);
    textdomain(GETTEXT_PACKAGE);

    while ( (c=getopts(argc, argv, opts, &args)) != 0 )
    {
        switch(c)
        {
        case 253:
            cmos_obj_factory(CMOS_UNIT_TEST_MODE | CMOS_GET_SINGLETON, args);
            break;
        case 254:
            // This is for unit testing. You can specify a file that
            // contains a dump of memory to use instead of reading
            // directly from RAM.
            memory_obj_factory(MEMORY_UNIT_TEST_MODE | MEMORY_GET_SINGLETON, args);
            break;
        case 255:
            printf("%s\n", smbios_get_library_version_string());
            exit(0);
            break;
        default:
            break;
        }
        free(args);
    }

    printf(_("Libsmbios:    %s\n"), smbios_get_library_version_string());

    // CMOS
    smbios_walk(dumpCmos, (void*)cmosDumpFile);

    // ID BYTE STRUCT
    // system string at 0xFE076 "Dell System"
    dumpMem(sysstrDumpFile, 0xFE076, strlen("Dell System"));
    // two byte structure at 0xFE840  (except diamond)
    dumpMem(idByteDumpFile, 0xFE840, 12);
}

void dumpCmosIndexPort(const char *fn, u32 indexPort, u32 dataPort)
{
    struct cmos_access_obj *cmos=0;
    struct cmos_access_obj *dump=0;

    printf(_("Dumping CMOS index(%d) data(%d)\n"), indexPort, dataPort);

    // ensure file exists
    FILE *fd = fopen(cmosDumpFile, "a+");
    if (!fd)
    {
        printf( _("error opening dump file \"%s\": %m\n"), cmosDumpFile);
        return;
    }
    fclose(fd);

    cmos = cmos_obj_factory(CMOS_GET_SINGLETON);
    dump = cmos_obj_factory(CMOS_GET_NEW | CMOS_UNIT_TEST_MODE, fn);

    for (int i=0; i<256; i++)
    {
        u8 byte;
        int ret=cmos_obj_read_byte(cmos, &byte, indexPort, dataPort, i);
        if (ret!=0)
            continue;

        ret = cmos_obj_write_byte(dump, byte, indexPort, dataPort, i);
        if (ret!=0)
            continue;
    }

    cmos_obj_free(dump);
    cmos_obj_free(cmos);
}

void dumpCmos(const struct smbios_struct *structure, void *userdata)
{
    u32 indexPort = 0, dataPort = 0;
    struct indexed_io_access_structure *d4 = 0;
    struct dell_protected_value_1_structure *d5 = 0;
    struct dell_protected_value_2_structure *d6 = 0;
    switch (smbios_struct_get_type(structure))
    {
        case 0xD4: 
            d4 = (struct indexed_io_access_structure *)structure;
            indexPort = d4->indexPort;
            dataPort = d4->dataPort;
            break;

        case 0xD5:
            d5 = (struct dell_protected_value_1_structure *)structure;
            indexPort = d5->indexPort;
            dataPort = d5->dataPort;
            break;

        case 0xD6:
            d6 = (struct dell_protected_value_2_structure *)structure;
            indexPort = d6->indexPort;
            dataPort = d6->dataPort;
            break;

        default:
            goto out;
    }

    dumpCmosIndexPort((const char *)userdata, indexPort, dataPort);
out:
    return;
}

void dumpMem( const char *fn, size_t offset, size_t len)
{
    FILE *fd = NULL;
    u8 *buf = NULL;
    int ret;

    fd = fopen( fn, "w+" );
    if (!fd)
    {
        printf( _("error opening dump file \"%s\": %m\n"), fn);
        return;
    }
    buf = calloc(1, len);
    if (!buf)
    {
        printf( _("could not allocate memory: %m\n"));
        goto err;
    }
    ret = memory_read(buf, offset, len);
    if (ret < 0)
    {
        printf( _("could not read from memory: %m\n"));
        goto err;
    }
    int recs = fwrite(buf, len, 1, fd);
    if (recs != 1)
        printf( _("could not write to \"%s\": %m\n"), fn);
err:
    if (buf)
        free(buf);
    fclose(fd);
}

