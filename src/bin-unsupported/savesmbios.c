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

#include "smbios_c/obj/memory.h"
#include "smbios_c/obj/smbios.h"

const char *smbiosDumpFile = "smbios.dat";
void dump_smbios_table(struct smbios_table *table, FILE *fd );

int
main (int argc, char **argv)
{
    (void)argc;
    (void)argv;

    FILE *smbiosDumpFD;
    smbiosDumpFD = fopen( smbiosDumpFile, "w+" );

    struct smbios_table *table = smbios_table_factory(SMBIOS_GET_SINGLETON);
    dump_smbios_table(table, smbiosDumpFD);
    smbios_table_free(table);

    fclose(smbiosDumpFD);
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

void dump_smbios_table(struct smbios_table *table, FILE *fd )
{
    struct smbios_table_entry_point tep;
    struct my_smbios_table *my = (struct my_smbios_table *)table;
    memcpy(&tep, & (my->tep), sizeof(my->tep));
    tep.dmi.table_address = 0xF0000UL + sizeof(tep);
    fwrite(&tep, sizeof(tep), 1, fd);
    fwrite(& (my->table), sizeof(tep.dmi.table_length), 1, fd);
}

