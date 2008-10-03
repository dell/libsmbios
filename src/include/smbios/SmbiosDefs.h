// vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:
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


#ifndef SMBIOSDEFS_H
#define SMBIOSDEFS_H

// compat header should always be first header
#include "smbios/compat.h"


// abi_prefix should be last header included before declarations
#include "smbios/config/abi_prefix.hpp"

namespace smbios
{
    // List of constants to use for different table types
    enum {
        BIOS_Information  =  0 ,
        System_Information  =  1 ,
        Base_Board_Information  =  2 ,
        System_Enclosure_or_Chassis  =  3 ,
        Processor_Information  =  4 ,
        Memory_Controller_Information  =  5 ,
        Memory_Module_Information  =  6 ,
        Cache_Information  =  7 ,
        Port_Connector_Information  =  8 ,
        System_Slots  =  9 ,
        On_Board_Devices_Information  =  10 ,
        OEM_Strings  =  11 ,
        System_Configuration_Options  =  12 ,
        BIOS_Language_Information  =  13 ,
        Group_Associations  =  14 ,
        System_Event_Log  =  15 ,
        Physical_Memory_Array  =  16 ,
        Memory_Device  =  17 ,
        Memory_Error_Information_32_bit  =  18 ,
        Memory_Array_Mapped_Address  =  19 ,
        Memory_Device_Mapped_Address  =  20 ,
        Built_in_Pointing_Device  =  21 ,
        Portable_Battery  =  22 ,
        System_Reset  =  23 ,
        Hardware_Security  =  24 ,
        System_Power_Controls  =  25 ,
        Voltage_Probe  =  26 ,
        Cooling_Device  =  27 ,
        Temperature_Probe  =  28 ,
        Electrical__Current_Probe  =  29 ,
        Out_of_Band_Remote_Access  =  30 ,
        Boot_Integrity_Services_Entry_Point  =  31 ,
        System_Boot_Information  =  32 ,
        Memory_Error_Information_64_bit  =  33 ,
        Management_Device  =  34 ,
        Management_Device_Component  =  35 ,
        Management_Device_Threshold  =  36 ,
        Memory_Channel  =  37 ,
        IPMI_Device_Information  =  38 ,
        System_Power_Supply  =  39 ,
        Inactive  =  126 ,
        Revisions_and_IDs  =  208 ,
        Parallel_Port  =  209 ,
        Serial_Port  =  210 ,
        IR_Port  =  211 ,

        End_of_Table  =  127 ,

        // vendor proprietary
        Dell_Revisions_and_IDs = 0xD0,
        Dell_Indexed_Io = 0xD4
    };

}


// always should be last thing in header file
#include "smbios/config/abi_suffix.hpp"

#endif /* SMBIOSDEFS_H */
