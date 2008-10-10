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
#include "smbios/compat.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <stdlib.h>

#include "smbios/ISmbios.h"
#include "smbios/IToken.h"
#include "smbios/IMemory.h"  // only needed if you want to use fake input (memdump.dat)
#include "smbios/SystemInfo.h"
#include "getopts.h"

// always include last if included.
#include "smbios/message.h"  // not needed outside of this lib. (mainly for gettext i18n)

using namespace std;

struct options opts[] =
    {
        { 255, "version", "Display libsmbios version information", "v", 0 },
        { 0, NULL, NULL, NULL, 0 }
    };

#if defined(_MSC_VER)
#pragma pack(push,1)
#endif

/*
    PROBE CUSTOMIZATION

    Customizes either type 26 (voltage), 28 (temperature), or 29 (electrical current)
   
   Probe-related values communicated through SMBIOS interfaces are always normalized values.  Using the example defined in 3.12.2, for instance, a raw current reading of 50 (decimal) corresponds to 25 degrees C — 50 (raw value) * .2 (Resolution) + 15 (Minimum Value).  To set an upper threshold of 60 degrees C, the management software would set the associated token to the raw value of 225 — ( 60 (desired value) - 15 (Minimum Value) ) / .2 (Resolution).
 -- tokens are either a token (in range 0xF000 - 0xFFFE) or 0x0000 if not supported
 -- each token represents an unsigned normalized value in the std structures resolution 
       with 0 as the minimum value
   */
struct probe_customization
{
// offset     // type == 0xDC
/* 0x00 */    u8	     type;
/* 0x01 */    u8	     length;
/* 0x02 */    u16	     handle;
/* 0x04 */    u16      present_reading_token;
/* 0x06 */    u16      reference_reading_token;
/* 0x08 */    u16      present_status_token;  // 0x03 == ok, 0x04 == Non-critical, 0x05 == critical
/* 0x0A */    u16      upper_non_critical_threshold_token;
/* 0x0C */    u16      lower_non_critical_threshold_token;
/* 0x0E */    u16      upper_critical_threshold_token;
/* 0x10 */    u16      lower_critical_threshold_token;
/* 0x12 */    u16      re_enable_alerts_token;
/* 0x14 */    u16      reserved;
}
    LIBSMBIOS_PACKED_ATTR;


/*
   COOLING CUSTOMIZATION

This Dell-specific structure type is used to identify system-specific tokens associated with runtime attributes of a cooling device, or fan.  Each standard cooling device structure, in a Dell-developed BIOS, includes the handle of its associated customization structure — see 2.24 Cooling Device (Type 1Bh) on page 55 for details.

The customization allows system-specific token values to be associated with fixed fan attributes without forcing a specific token value for a particular attribute.  This approach is used to provide a level of customization required, based on the ever-growing number of fans in our systems.  Any tokenized attribute that is not supported by the system is encoded as 0000h.

       */
struct cooling_customization
{
}
    LIBSMBIOS_PACKED_ATTR;

/*
   VOLTAGE PROBE

00h Type    BYTE 26     Voltage Probe indicator
01h Length  BYTE 14h    Length of the structure, at least 14h.
02h Handle  WORD Varies The handle, or instance number, associated with the structure.
04h Description BYTE STRING The number of the string that contains additional descriptive information about the probe or its location.

05h Location and Status BYTE Bit-field Defines the probe’s physical location and status of the voltage monitored by this voltage probe.  See 2.23.2.

06h Maximum Value WORD Varies The maximum voltage level readable by this probe, in millivolts. If the value is unknown, the field is set to 0x8000.

08h Minimum Value WORD Varies The minimum voltage level readable by this probe, in millivolts. If the value is unknown, the field is set to 0x8000.

0Ah Resolution WORD Varies The resolution for the probe’s reading, in tenths of millivolts. If the value is unknown, the field is set to 0x8000.

0Ch Tolerance WORD Varies The tolerance for reading from this probe, in plus/minus millivolts. If the value is unknown, the field is set to 0x8000.

0Eh Accuracy WORD Varies The accuracy for reading from this probe, in plus/minus 1/100th of a percent.  If the value is unknown, the field is set to 0x8000.

10h Probe Customization WORD Varies Contains the handle of the associated Probe Customization structure, see 3.12 Probe Customization — Type DCh on page 175.  If no runtime status is available for the probe, this field is set to 0 by the BIOS.

12h Reserved WORD 0000h Set to 0 and available for future definition via this specification.

14h Nominal Value WORD Varies The nominal value for the probe’s reading in millivolts.  If the value is unknown, the field is set to 0x8000.  This field is present in the structure only if the structure’s Length is larger than 14h.
   */


    // works for voltage/temperature/current
struct probe_structure
{
/* 0x00 */  u8  type;  /* type == 0x1A */
/* 0x01 */  u8  length;
/* 0x02 */  u16 handle;
/* 0x04 */  u8  description; /* STRING */
/* 0x05 */  u8  location_and_status;  /* BITFIELD */
/* 0x06 */  s16 maximum_value; /* millivolts  unknown = 0x8000 */
/* 0x08 */  s16 minimum_value; /* millivolts  unknown = 0x8000 */
/* 0x0A */  u16 resolution; /* millivolts  unknown = 0x8000 */
/* 0x0C */  u16 tolerance;  /* +/- millivolts  unknown = 0x8000 */
/* 0x0E */  u16 accuracy; /* +/- 1/100th of a percent  uknown = 0x8000 */
/* 0x10 */  u16 probe_customization; 
/* 0x12 */  u16 reserved; 
/* 0x14 */  s16 nominal_value; /* millivolts */
} 
    LIBSMBIOS_PACKED_ATTR;


#if defined(_MSC_VER)
#pragma pack(pop)
#endif

const smbios::ISmbiosItem *getItemByHandle( u16 handle )
{
    smbios::SmbiosFactory *smbiosFactory = smbios::SmbiosFactory::getFactory();
    const smbios::ISmbiosTable *table = smbiosFactory->getSingleton();
    smbios::ISmbiosTable::const_iterator itemIter;

    const smbios::ISmbiosItem *ret = 0;
    itemIter = table->begin();
    while( itemIter != table->end() )
    {
        if( itemIter->getHandle() == handle )
            ret = &(*itemIter);
        ++itemIter;
    }
    return ret;
}

void displayVoltageInfo(const smbios::ISmbiosItem &item)
{
    cout << "  Description: " << getString_FromItem(item, 0x04) << endl;
}

void displayCurrentInfo(const smbios::ISmbiosItem &item)
{
    cout << "  Description: " << getString_FromItem(item, 0x04) << endl;
}

void displayCoolingDeviceInfo(const smbios::ISmbiosItem &item)
{
    cout << item << endl;
}

void displayTemperatureInfo(const smbios::ISmbiosItem &item)
{
    cout << "  Description: " << getString_FromItem(item, 0x04) << endl;
    cout << "  Length: " << static_cast<int>(item.getLength()) << endl;

/* 0x06 */  s16 maximum_value = getU16_FromItem(item, 0x06);
/* 0x08 */  s16 minimum_value = getU16_FromItem(item, 0x08);
/* 0x0A */  u16 resolution    = getU16_FromItem(item, 0x0A);
/* 0x0C */  u16 tolerance     = getU16_FromItem(item, 0x0C);
/* 0x0E */  u16 accuracy      = getU16_FromItem(item, 0x0E);
/* 0x10 */  u16 probe_customization = getU16_FromItem(item, 0x10);
/* 0x14 */  u16 nominal_value = 0;//getU16_FromItem(item, 0x14);

    cout << "  Maximum value: " << maximum_value << endl;
    cout << "  Minimum value: " << minimum_value << endl;
    cout << "  Resolution: " << resolution << endl;
    cout << "  Tolerance: " << tolerance << endl;
    cout << "  Accuracy: " << accuracy << endl;
    cout << "  Nominal: " << nominal_value << endl;
    cout << hex;
    cout << "  Probe Customization Structure: " << probe_customization << endl;

    const smbios::ISmbiosItem *cust = getItemByHandle( probe_customization );
    if( cust )
    {
        cout << "    Got customization struct." << endl;

/* 0x04 */    u16      present_reading_token   = getU16_FromItem(*cust, 0x04);
/* 0x06 */    u16      reference_reading_token = getU16_FromItem(*cust, 0x06);

                        // 0x03 == ok, 0x04 == Non-critical, 0x05 == critical
/* 0x08 */    u16      present_status_token    = getU16_FromItem(*cust, 0x08);  
/* 0x0A */    u16      upper_non_critical_threshold_token = getU16_FromItem(*cust, 0x0A);
/* 0x0C */    u16      lower_non_critical_threshold_token = getU16_FromItem(*cust, 0x0C);
/* 0x0E */    u16      upper_critical_threshold_token     = getU16_FromItem(*cust, 0x0E);
/* 0x10 */    u16      lower_critical_threshold_token     = getU16_FromItem(*cust, 0x10);
/* 0x12 */    u16      re_enable_alerts_token             = getU16_FromItem(*cust, 0x12);

        cout << "    present_reading_token: " << present_reading_token << endl;
        cout << "    reference_reading_token: " << reference_reading_token << endl;
        cout << "    present_status_token: " << present_status_token << endl;
        cout << "    upper_non_critical_threshold_token: " << upper_non_critical_threshold_token << endl;
        cout << "    lower_non_critical_threshold_token: " << lower_non_critical_threshold_token << endl;
        cout << "    upper_critical_threshold_token: " << upper_critical_threshold_token << endl;
        cout << "    lower_critical_threshold_token: " << lower_critical_threshold_token << endl;
        cout << "    re_enable_alerts_token: " << re_enable_alerts_token << endl;

        smbios::TokenTableFactory *ttFactory = smbios::TokenTableFactory::getFactory() ;
        smbios::ITokenTable *tokenTable = ttFactory->getSingleton();
        u16 res = 0;
        u16 current_value = 0;
        if(present_reading_token)
        {
            (*tokenTable)[ present_reading_token ]->getString(reinterpret_cast<u8 *>(&res), sizeof(res));
            cout << "      Current value: " << static_cast<int>(res) << endl;
            current_value = res;
        }
        if(reference_reading_token)
        {
            (*tokenTable)[ reference_reading_token ]->getString(reinterpret_cast<u8 *>(&res), sizeof(res));
            cout << "      Reference value: " << static_cast<int>(res) << endl;
        }
        if(present_status_token)
        {
            (*tokenTable)[ present_status_token ]->getString(reinterpret_cast<u8 *>(&res), sizeof(res));
            cout << "      Status value: " << static_cast<int>(res) << endl;
        }
        if(upper_non_critical_threshold_token)
        {
            (*tokenTable)[ upper_non_critical_threshold_token ]->getString(reinterpret_cast<u8 *>(&res), sizeof(res));
            cout << "      upper non-crit thresh value: " << static_cast<int>(res) << endl;
        }
        if(lower_non_critical_threshold_token)
        {
            (*tokenTable)[ lower_non_critical_threshold_token ]->getString(reinterpret_cast<u8 *>(&res), sizeof(res));
            cout << "      lower non-crit thresh value: " << static_cast<int>(res) << endl;
        }
        if(upper_critical_threshold_token)
        {
            (*tokenTable)[ upper_critical_threshold_token ]->getString(reinterpret_cast<u8 *>(&res), sizeof(res));
            cout << "      upper crit thresh value: " << static_cast<int>(res) << endl;
        }
        if(lower_critical_threshold_token)
        {
            (*tokenTable)[ lower_critical_threshold_token ]->getString(reinterpret_cast<u8 *>(&res), sizeof(res));
            cout << "      lower crit thresh value: " << static_cast<int>(res) << endl;
        }

        cout << "  TEMP: ";
        // resolution = 1/1000 deg C
        // minimum_value = 1/10 deg C
        cout << static_cast<float>(current_value * static_cast<float>(resolution) / 1000.0 + minimum_value / 10.0) << " degrees C" << endl;

    }
    cout << dec;
    cout << endl;
}


int
main (int argc, char **argv)
{
    int retval = 0;
    try
    {
        int c=0;
        char *args = 0;

        while ( (c=getopts(argc, argv, opts, &args)) != 0 )
        {
            switch(c)
            {
            case 255:
                cout << "Libsmbios version:    " << SMBIOSGetLibraryVersionString() << endl;
                exit(0);
                break;
            default:
                break;
            }
            free(args);
        }

        smbios::SmbiosFactory *smbiosFactory = smbios::SmbiosFactory::getFactory();
        const smbios::ISmbiosTable *table = smbiosFactory->getSingleton();
        smbios::ISmbiosTable::const_iterator itemIter;

        cout << endl;
        cout << "NOTE: This is a proof-of-concept demonstration only!" << endl;
        cout << "      This program is meant to demonstrate the correct way to obtain" << endl;
        cout << "      sensor data on Dell systems using the SMI interface. It is not" << endl;
        cout << "      meant to be a final, working program to use. We hope to leverage" << endl;
        cout << "      this program to add support to lmsensors." << endl;
        cout << endl;

        cout << "Voltage Probes:" << endl;
        itemIter = (*table)[0x1A];
        while( itemIter != table->end() )
        {
            displayVoltageInfo(*itemIter);
            ++itemIter;
        }
        cout << endl;

        cout << "Temperature Probes:" << endl;
        itemIter = (*table)[0x1C];
        while( itemIter != table->end() )
        {
            displayTemperatureInfo(*itemIter);
            ++itemIter;
        }
        cout << endl;

        cout << "Current Probes:" << endl;
        itemIter = (*table)[0x1D];
        while( itemIter != table->end() )
        {
            displayCurrentInfo(*itemIter);
            ++itemIter;
        }
        cout << endl;

        cout << "Cooling Device:" << endl;
        itemIter = (*table)[0x1B];
        while( itemIter != table->end() )
        {
            displayCoolingDeviceInfo(*itemIter);
            ++itemIter;
        }
        cout << endl;
    }
    catch( const exception &e )
    {
        cerr << endl;
        cerr << "An Error occurred. The Error message is: " << endl;
        cerr << "    " << e.what() << endl;
        cerr << endl;
        cerr << "Problem reading or writing tag. Common problems are:" << endl;
        cerr << endl;
        cerr << "    -- Insufficient permissions to perform operation." << endl;
        cerr << "       Try running as a more privileged account." << endl;
        cerr << "          Linux  : run as 'root' user" << endl;
        cerr << "          Windows: run as 'administrator' user" << endl;
        cerr << endl;
        cerr << "    -- dcdbas device driver not loaded." << endl;
        cerr << "       Try loading the dcdbas driver" << endl;
        cerr << "          Linux  : modprobe dcdbas" << endl;
        cerr << "          Windows: dcdbas driver not yet available." << endl;
        cerr << endl;

        retval = 1;
    }
    catch ( ... )
    {
        cerr << endl;
        cerr << "An Unknown Error occurred. Aborting." << endl;
        cerr << endl;
        retval = 2;
    }

    return retval;
}
