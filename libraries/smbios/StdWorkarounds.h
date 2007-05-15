// vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:cindent:
/*
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


#ifndef STDWORKAROUNDS_H_
#define STDWORKAROUNDS_H_

// This file will eventually be generated automatically from an XML file.
// for now, hand edit.
//
// This file contains a list of all the BIOS cock-ups that we can work around in
// software.

namespace smbios
{
    const WorkaroundSmbiosItem InvalidCheckTypeSymptoms[] =
        {
            // Item Type,  Offset,  Data Type, Data to check(*)
            { 0xD4, 0x08, TYPE_U8 , { {0x03, 0} } },
            //                         ^-- wrong checksum type of 0x3
            //               ^------------ checkType is a single u8
            //        ^------------------- offset 0x8 is the checkType field.
            //
            { 0xD4, 0x02, TYPE_U16, { {0x02, 0xd4, 0} } },
            //                        ^--- Check the handle as well, only
            //                              the third 0xD4 entry is flawed.
            //                              Third 0xD4 is handle 0xD402
            //                              note little endian notation.
            //                ^----------- The Handle is a U16
            //       ^-------------------- Check the handle at offset 0x2
            //
            { 0, 0, 0, { {0} } },
            //
            // * -- Data to check is a _union_. initializer syntax for unions
            //      sucks. You can only initialize the first member of a union
            //      in a statically allocated variable.
            //      I have made the first member a (u8 data[8]) so that we can
            //      (messily) initialize any possible combination.
        };

    const WorkaroundSmbiosItem InvalidCheckTypeFixup[] =
        {
            // Item Type, Offset, Data Type, Data to Change
            { 0xD4, 0x08, TYPE_U8, {{0x00}} },  //caution here. see datatron def.
            { 0, 0, 0, {{0}} },
        };

    // PE1300 has same bug, but fixup is different
    const WorkaroundSmbiosItem PE1300_InvalidCheckTypeFixup[] =
        {
            // Item Type, Offset, Data Type, Data to Change
            { 0xD4, 0x08, TYPE_U8, {{0x01}} },  //caution here. see datatron def.
            { 0, 0, 0, {{0}} },
        };

    const Workaround InvalidCheckType =
        { "Invalid Checksum Type", InvalidCheckTypeSymptoms, InvalidCheckTypeFixup };
    // PE1300 has same symptoms as above, but different fix.
    const Workaround PE1300_InvalidCheckType =
        { "PE1300 Invalid Checksum Type", InvalidCheckTypeSymptoms, PE1300_InvalidCheckTypeFixup };

    // System Definitions...

    const SystemAffected PE0600 = { 0x0134, "A00", "A05" };  // TODO: code to check BIOS
    const SystemAffected PE0650 = { 0x0141, "A00", "A00" };  // TODO: code to check BIOS
    const SystemAffected PE1300 = { 0x8E,   "A00", "A--" };  // TODO: code to check BIOS
    const SystemAffected PE1600 = { 0x0135, "A00", "A00" };  // TODO: code to check BIOS
    const SystemAffected PE1650 = { 0x011B, "A00", "A09" };  // TODO: code to check BIOS
    const SystemAffected IDS4235 = { 0x8012, "A--", "A--" };  // TODO: code to check BIOS
    const SystemAffected PE1655 = { 0x0124, "A00", "A00" };  // TODO: code to check BIOS
    const SystemAffected PE1750 = { 0x014a, "A00", "A00" };  // TODO: code to check BIOS
    const SystemAffected PE2600 = { 0x0123, "A00", "A03" };  // TODO: code to check BIOS
    const SystemAffected PE2650 = { 0x0121, "A00", "A10" };  // TODO: code to check BIOS
    const SystemAffected PE4600 = { 0x0106, "A00", "A08" };  // TODO: code to check BIOS
    const SystemAffected PE6600 = { 0x0109, "A00", "A08" };  // TODO: code to check BIOS // what about 6650?

    // Workaround lists for each system listed.
    const Workaround *PE0600_Workarounds[] = { &InvalidCheckType, 0 };
    const Workaround *PE0650_Workarounds[] = { &InvalidCheckType, 0 };
    const Workaround *PE1300_Workarounds[] = { &PE1300_InvalidCheckType, 0 };
    const Workaround *PE1600_Workarounds[] = { &InvalidCheckType, 0 };
    const Workaround *PE1650_Workarounds[] = { &InvalidCheckType, 0 };
    const Workaround *IDS4235_Workarounds[] = { &InvalidCheckType, 0 };
    const Workaround *PE1655_Workarounds[] = { &InvalidCheckType, 0 };
    const Workaround *PE1750_Workarounds[] = { &InvalidCheckType, 0 };
    const Workaround *PE2600_Workarounds[] = { &InvalidCheckType, 0 };
    const Workaround *PE2650_Workarounds[] = { &InvalidCheckType, 0 };
    const Workaround *PE4600_Workarounds[] = { &InvalidCheckType, 0 };
    const Workaround *PE6600_Workarounds[] = { &InvalidCheckType, 0 };

    // now, put everything together.
    //   not NULL terminated, see numSystemWorkarounds below...
    const SystemWorkaround  workaroundMasterList[] =
        {
            { &PE0600, PE0600_Workarounds },
            { &PE0650, PE0650_Workarounds },
            { &PE1300, PE1300_Workarounds },  // Unit tested
            { &PE1600, PE1600_Workarounds },
            { &PE1650, PE1650_Workarounds },  // Unit tested
            { &IDS4235, IDS4235_Workarounds },  // Unit tested
            { &PE1655, PE1655_Workarounds },  // Unit tested
            { &PE1750, PE1750_Workarounds },
            { &PE2600, PE2600_Workarounds },
            { &PE2650, PE2650_Workarounds },
            { &PE4600, PE4600_Workarounds },
            { &PE6600, PE6600_Workarounds },  // Unit tested
        };

    // works because list is statically allocated.
    const int numSystemWorkarounds = (sizeof(workaroundMasterList) / sizeof(workaroundMasterList[0]));

}

#endif /* STDWORKAROUNDS_H_ */
