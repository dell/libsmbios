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

#include <stdio.h>        // fopen()

//#include <unistd.h>    // getopt()
#include <string.h>
#include <stdlib.h>

#include "smbios_c/obj/memory.h"
#include "smbios_c/memory.h"
#include "smbios_c/obj/smbios.h"
#include "smbios_c/system_info.h"

#include "getopts.h"

const char *sysstrDumpFile = "sysstr.dat";
const char *smbiosDumpFile = "smbios.dat";
const char *idByteDumpFile = "idbyte.dat";
void dump_smbios_table(const char *smbiosDumpFile);
void dumpMem( const char *fn, size_t offset, size_t len);

struct options opts[] =
    {
        {
            254, "memory_file", "Debug: Memory dump file to use instead of physical memory", "m", 1
        },
        { 255, "version", "Display libsmbios version information", "v", 0 },
        { 0, NULL, NULL, NULL, 0 }
    };



int
main (int argc, char **argv)
{
    int c=0;
    char *args = 0;
    while ( (c=getopts(argc, argv, opts, &args)) != 0 )
    {
        switch(c)
        {
        case 254:
            // This is for unit testing. You can specify a file that
            // contains a dump of memory to use instead of reading
            // directly from RAM.
            memory_obj_factory(MEMORY_UNIT_TEST_MODE | MEMORY_GET_SINGLETON, args);
            break;
        case 255:
            printf("Libsmbios:    %s\n", smbios_get_library_version_string());
            exit(0);
            break;
        default:
            break;
        }
        free(args);
    }

    printf("Libsmbios:    %s\n", smbios_get_library_version_string());

    // SMBIOS TABLE
    dump_smbios_table(smbiosDumpFile);

    // ID BYTE STRUCT
    // system string at 0xFE076 "Dell System"
    dumpMem(sysstrDumpFile, 0xFE076, strlen("Dell System"));
    // two byte structure at 0xFE840  (except diamond)
    dumpMem(idByteDumpFile, 0xFE840, 12);


    // CMOS
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


    printf("dumping table header.\n");
    int ret = fwrite(&tep, sizeof(tep), 1, fd);
    if (ret != 1)  // one item
        goto out_err_header;

    printf("dumping table.\n");
    ret = fwrite(my->table, tep.dmi.table_length, 1, fd);
    if (ret != 1)  // one item
        goto out_err_table;

    printf("dumped table.\n");
    printf("table length: %d\n", tep.dmi.table_length);

    goto out;
out_err_header:
    printf("error dumping header ret(%d) sizeof(tep): %zd\n", ret, sizeof(tep));
    goto out;

out_err_table:
    printf("error dumping table\n");
    goto out;

out:
    smbios_table_free(table);
    fclose(fd);
    return;
}

