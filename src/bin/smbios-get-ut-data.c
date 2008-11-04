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
            printf(_("Libsmbios:    %s\n"), smbios_get_library_version_string());
            exit(0);
            break;
        default:
            break;
        }
        free(args);
    }

    printf(_("Libsmbios:    %s\n"), smbios_get_library_version_string());

    // SMBIOS TABLE
    dump_smbios_table(smbiosDumpFile);

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
    FILE *fd = fopen( fn, "w+" );
    u8 *buf = calloc(1, len);
    memory_read(buf, offset, len);
    int recs = fwrite(buf, len, 1, fd);
    if (recs != 1)
        ; // nada
    free(buf);
    fclose(fd);
}

#if defined(_MSC_VER)
#pragma pack(push,1)
#endif
struct dmi_table_entry_point
{
    u8 anchor[5];
    u8 checksum;
    u16 table_length;
    u32 table_address;
    u16 table_num_structs;
    u8 smbios_bcd_revision;
}
LIBSMBIOS_C_PACKED_ATTR;

struct smbios_table_entry_point
{
    u8 anchor[4];
    u8 checksum;
    u8 eps_length;
    u8 major_ver;
    u8 minor_ver;
    u16 max_struct_size;
    u8 revision;
    u8 formatted_area[5];
    struct dmi_table_entry_point dmi;
    u8 padding_for_Intel_BIOS_bugs;
} LIBSMBIOS_C_PACKED_ATTR;
#if defined(_MSC_VER)
#pragma pack(pop)
#endif


struct my_smbios_table
{
    int initialized;
    struct smbios_table_entry_point tep;
    struct table *table;
    int last_errno;
    char *errstring;
};

void dump_smbios_table(const char *smbiosDumpFile)
{
    FILE *fd = fopen( smbiosDumpFile, "w+" );
    struct smbios_table *table = smbios_table_factory(SMBIOS_GET_SINGLETON | SMBIOS_NO_FIXUPS);
    struct smbios_table_entry_point tep;
    struct my_smbios_table *my = (struct my_smbios_table *)table;
    memcpy(&tep, & (my->tep), sizeof(my->tep));
    tep.dmi.table_address = 0xE0000UL + sizeof(tep);

    // fixup checksum
    tep.checksum = 0;
    tep.dmi.checksum = 0;

    u8 checksum = 0;
    const u8 *ptr = (const u8*)(&(tep.dmi));
    for( unsigned int i = 0; i < sizeof(tep.dmi); ++i )
        checksum = checksum + ptr[i];
    tep.dmi.checksum = ~checksum + 1;

    checksum = 0;
    ptr = (const u8*)(&tep);
    for( unsigned int i = 0; (i < (unsigned int)(tep.eps_length)) && (i < sizeof(tep)); ++i )
        checksum = checksum + ptr[i];
    tep.checksum = ~checksum + 1;


    printf( _("dumping table header.\n")) ;
    int ret = fwrite(&tep, sizeof(tep), 1, fd);
    if (ret != 1)  // one item
        goto out_err_header;

    printf( _("dumping table.\n")) ;
    ret = fwrite(my->table, tep.dmi.table_length, 1, fd);
    if (ret != 1)  // one item
        goto out_err_table;

    printf(_("dumped table.\n"));
    printf(_("table length: %d\n"), tep.dmi.table_length);

    goto out;
out_err_header:
    printf( _("error dumping header ret(%d) sizeof(tep): %zd\n"), ret, sizeof(tep));
    goto out;

out_err_table:
    printf( _("error dumping table\n"));
    goto out;

out:
    smbios_table_free(table);
    fclose(fd);
    return;
}

