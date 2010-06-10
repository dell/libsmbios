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

#define LIBSMBIOS_C_SOURCE

// Include compat.h first, then system headers, then public, then private
#include "smbios_c/compat.h"

// system

// public
#include "smbios_c/obj/smbios.h"
#include "smbios_c/obj/token.h"
#include "smbios_c/types.h"

// private
#include "smbios_impl.h"

//const Workaround *PE1300_Workarounds[] = { &PE1300_InvalidCheckType, 0 };

#define PE1300   0x8E
#define PE0600   0x0134
#define PE0650   0x0141
#define PE1600   0x0135
#define PE1650   0x011B
#define IDS4235  0x8012
#define PE1655   0x0124
#define PE1750   0x014a
#define PE2600   0x0123
#define PE2650   0x0121
#define PE4600   0x0106
#define PE6600   0x0109

static bool system_affected(u16 *id_list, int num, u16 sysid)
{
    bool is_system_affected = false;
    for(int i=0; i<num; ++i)
        if (id_list[i] == sysid)
            is_system_affected = true;
    return is_system_affected;
}

// define
__hidden u16 get_id_byte_from_mem();

#define DELL_CHECK_FIXUP_BAD_HANDLE 0xd402
static void do_dell_check_type_fixup(struct smbios_table *table)
{
    u16 affected[] = {PE1300, PE0600, PE0650, PE1600, PE1650, IDS4235, PE1655, PE1750, PE2600, PE2650, PE4600, PE6600};
    int num_affected = sizeof(affected)/sizeof(affected[0]);
    struct smbios_struct *s = 0;
    struct indexed_io_access_structure *broken = 0;

    dbg_printf ("%s\n", __PRETTY_FUNCTION__);
    u16 sysid = get_id_byte_from_mem();
    if (!system_affected(affected, num_affected, sysid))
        goto out;

    s = smbios_table_get_next_struct_by_handle(table, 0, DELL_CHECK_FIXUP_BAD_HANDLE);
    if (!s)
        goto out;

    broken = (struct indexed_io_access_structure *)s;
    dbg_printf ("%s - cur checktype: %x  sysid: %x\n", __PRETTY_FUNCTION__, broken->checkType, sysid);

    if (broken->checkType == CHECK_TYPE_WORD_CHECKSUM_N)
        broken->checkType = CHECK_TYPE_WORD_CHECKSUM;

    // PE1300 has same breakage, but slighly different fix
    if (sysid == PE1300)
        broken->checkType = CHECK_TYPE_BYTE_CHECKSUM;

out:
    return;
}

__hidden void do_smbios_fixups(struct smbios_table *table)
{
    dbg_printf ("%s\n", __PRETTY_FUNCTION__);
    do_dell_check_type_fixup(table);
}

