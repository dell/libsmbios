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

#include "smbios/ISmi.h"
#include "smbios/IToken.h"
#include "smbios/SystemInfo.h"
#include "smbios/version.h"
#include "getopts.h"

// always include last if included.
#include "smbios/message.h"

using namespace std;

struct options opts[] =
{
    { 1, "info", "Get wireless info", "i", 0 },
    { 2, "wlan", "Set WLAN radio runtime status", NULL, 1 },
    { 3, "bt", "Set Bluetooth radio runtime status", NULL, 1 },
    { 4, "wwan", "Set WWAN radio runtime status", NULL, 1 },
    { 5, "sw_wlan", "Enable hardware switch to control WLAN radio", NULL, 1 },
    { 6, "sw_bt", "Enable hardware switch to control Bluetooth radio", NULL, 1 },
    { 7, "sw_wwan", "Enable hardware switch to control WWAN radio", NULL, 1 },
    { 8, "sw_locator", "Enable hardware WiFi locator switch", NULL, 1 },
    { 9, "st_wlan", "WLAN status: 0 = enabled. 1 = disabled. 2 = not present. 3 = unsupported. 4 = unknown.", NULL, 0 },
    { 10, "st_bt", "Return 0 if bluetooth is enabled. 1 if disabled. 2 if unknown.", NULL, 0 },
    { 11, "st_wwan", "Return 0 if wwan is enabled. 1 if disabled. 2 if unknown.", NULL, 0 },
    { 12, "boot", "Set BIOS boot-time setting as well as runtime setting. Must be first param.", NULL, 0 },
    { 13, "boot_only", "Set BIOS boot-time setting only. Must be first param.", NULL, 0 },
    { 252, "password", "BIOS setup password", "p", 1 },
    { 249, "rawpassword", "Do not auto-convert password to scancodes", NULL, 0 },
    { 255, "version", "Display libsmbios version information", NULL, 0 },
    { 0, NULL, NULL, NULL, 0 }
};

#define DELL_WIRELESS_CTL_VERSION "0.3.0"

void printWirelessInfo()
{
    // cbClass 17
    // cbSelect 11
    // WiFi Control
    // Entry/return values grouped by the value of cbARG1, byte0 which indicates
    // the function to perform:
    // 0x0 = Return Wireless Info
    //  cbRES1      Standard return codes (0, -1, -2)

    u32 args[4] = {0, 0, 0, 0};
    u32 res[4] = {0,};
    try
    {
        smi::doSimpleCallingInterfaceSmi(17, 11, args, res);

        cout << "Wireless Info:" << endl;

        //  cbRES2      Info bit flags:
        //      0 Hardware switch supported (1)
        //      16 Hardware (HW) switch is On (1)
        if (res[smi::cbRES2] & (1 << 0))
        {
            cout << "\tHardware switch supported" << endl;
            cout << "\tHardware switch is " << ((res[smi::cbRES2] & (1 << 16)) ? "On":"Off") << endl;
        } else {
            cout << "\tHardware switch not supported" << endl;
        }
       
        //      1 WiFi locator supported (1)
        if (res[smi::cbRES2] & (1 << 1))
            cout << "\tWiFi Locator supported" << endl;
        else
            cout << "\tWiFi Locator not supported" << endl;

         //      5 Wireless KBD supported (1)
        if (res[smi::cbRES2] & (1 << 5))
            cout << "\tWireless Keyboard supported" << endl;
        else 
            cout << "\tWireless Keyboard not supported" << endl;

        //      6-7 Reserved (0)
        //      11-15 Reserved (0)
        //      20-31 Reserved (0) 
    
        //  cbRES3      NVRAM size in bytes
        cout << "\tNVRAM Size: " << dec << res[smi::cbRES3] << " bytes" << endl;
    
        //  cbRES4, byte 0  NVRAM format version number
        cout << "\tNVRAM format version: " << dec << (res[smi::cbRES4] & 0xFF) << endl;
   
        cout << endl;
        smi::wirelessRadioStatus(smi::WLAN_RADIO_NUM, cout, res[smi::cbRES2]);

        cout << endl;
        smi::wirelessRadioStatus(smi::BLUETOOTH_RADIO_NUM, cout, res[smi::cbRES2]);

        cout << endl;
        smi::wirelessRadioStatus(smi::WWAN_RADIO_NUM, cout, res[smi::cbRES2]);

    }
    catch(smi::UnsupportedSmi)
    {
        cout << "Wireless Info not supported on this system." << endl;
        return;
    }
}

void switchStatus()
{
    u32 args[4] = {0x2, 0, 0, 0};
    u32 res[4] = {0,};
    try
    {
        smi::doSimpleCallingInterfaceSmi(17, 11, args, res);

        cout << "Wireless Switch Configuration" << endl;
        // 0x2 = Wireless Switch Configuration
        //  cbARG1, byte1   Subcommand:
        //      0 Get config
        //      1 Set config
        //      2 Set WiFi locator enable/disable
        //  cbARG1,byte2    Switch settings (if byte 1==1):
        //      0 WLAN switch control (1)
        //      1 BT switch control (1)
        //      2 WWAN switch control (1)
        //      3-7 Reserved (0)
        //  cbARG1, byte2   Enable bits (if byte 1==2):
        //      0 Enable WiFi locator (1)
        //  cbRES1      Standard return codes (0, -1, -2)
        //  cbRES2      QuickSet radio disable bit map:
        //      0 WLAN controlled by switch (1)
        cout << "\tWLAN switch control " << ((res[smi::cbRES2] & (1<<0)) ? "on":"off") << endl;

        //      1 BT controlled by switch (1)
        cout << "\tBluetooth switch control " << ((res[smi::cbRES2] & (1<<1)) ? "on":"off") << endl;

        //      2 WWAN controlled by switch (1)
        cout << "\tWWAN switch control " << ((res[smi::cbRES2] & (1<<2)) ? "on":"off") << endl;

        //      3-6 Reserved (0)
        //      7 Wireless switch config locked (1)
        cout << "\tswitch config " << ((res[smi::cbRES2] & (1<<7)) ? "locked":"not locked") << endl;

        //      8 WiFi locator enabled (1)
        cout << "\tWiFi locator " << ((res[smi::cbRES2] & (1<<8)) ? "enabled":"disabled") << endl;

        //      9-14 Reserved (0)
        //      15 WiFi locator setting locked (1)
        cout << "\tWiFi Locator config " << ((res[smi::cbRES2] & (1<<15)) ? "locked":"not locked") << endl;

        //      16-31 Reserved (0)
    }
    catch (smi::UnsupportedSmi)
    {
        cout << "Wireless Switch Configuration not available on this system." << endl;
    }
}

// helper macro to save typing...
#define tokenStatus(foo) do {   \
    cout << "\t" << #foo << ": ";   \
    try {   \
        cout << smbios::isTokenActive( smi:: foo);   \
    } catch(smbios::DerefNullPointer) { \
        cout << "(NOT SUPPORTED)";  \
    }   \
    cout << endl;   \
    } while(0)

void wirelessTokenStatus()
{
    cout << "BIOS Boot-time settings (new interface):" << endl;

    tokenStatus(Wireless_Switch_Cellular_Control_Enable);
    tokenStatus(Wireless_Switch_Cellular_Control_Disable);
    tokenStatus(Wireless_Switch_Bluetooth_Control_Enable);
    tokenStatus(Wireless_Switch_Bluetooth_Control_Disable);
    tokenStatus(Wireless_Switch_Wireless_LAN_Control_Enable);
    tokenStatus(Wireless_Switch_Wireless_LAN_Control_Disable);
    tokenStatus(WiFi_Locator_Enable);
    tokenStatus(WiFi_Locator_Disable);
    tokenStatus(Cellular_Radio_Enable);
    tokenStatus(Cellular_Radio_Disable);
    tokenStatus(Bluetooth_Devices_Enable);
    tokenStatus(Bluetooth_Devices_Disable);
    tokenStatus(Wireless_LAN_Enable);
    tokenStatus(Wireless_LAN_Disable);

    cout << endl;
    cout << "BIOS Boot-time settings (old interface):" << endl;
    tokenStatus(Radio_Transmission_Enable);
    tokenStatus(Radio_Transmission_Disable);
    tokenStatus(Wireless_Device_Disable);
    tokenStatus(Wireless_Device_App_Control);
    tokenStatus(Wireless_Device_App_Or_Hotkey_Control);
}

int
main (int argc, char **argv)
{
    int retval = 0;

    string password("");
    bool rawPassword = false;

    bool enable = true;
    bool boot = false;
    bool runtime = true;

    bool printInfo = false, printSwitchStatus = false, printTokens = false;

    try
    {
        char *args = 0;
        int c=0;
        while ( (c=getopts(argc, argv, opts, &args)) != 0 )
        {
            switch(c)
            {
            case 1:
                cout << "Libsmbios version      : " << LIBSMBIOS_RELEASE_VERSION << endl;
                cout << "dellWirelessCtl version: " << DELL_WIRELESS_CTL_VERSION << endl;
                printInfo = true;
                printSwitchStatus = true;
                printTokens = true;
                break;

            case 2:
                enable = strtol(args, 0, 0);
                try {
                    smi::wirelessRadioControl(enable, boot, runtime, smi::Wireless_LAN_Enable, smi::Wireless_LAN_Disable, smi::WLAN_RADIO_NUM, password);
                } 
                catch (smi::ConfigError &) {
                    cerr << "Warning: The current boot-time setting for WLAN is 'disabled'. Cannot set runtime settings when device is disabled."<< endl;
                }
                catch (...){ // smi::UnsupportedSmi, smbios::DerefNullPointer
                    // this means new interface not supported. try old interface
                    if(boot)
                    {
                        cerr << "Runtime interface not present. Using old boot-only interface." << endl;
                        if (enable)
                            smbios::activateToken(smi::Radio_Transmission_Enable, password);
                        else
                            smbios::activateToken(smi::Radio_Transmission_Disable, password);

                        cerr << "Complete."  << endl; 
                    }
                    else
                    {
                        cerr << "Runtime interface not present. Cannot perform the requested change." << endl;
                    }
                }
                if (runtime) printInfo = true;
                if (boot) printTokens = true;
                break;
            case 3:
                enable = strtol(args, 0, 0);
                try {
                    smi::wirelessRadioControl(enable, boot, runtime, smi::Bluetooth_Devices_Enable, smi::Bluetooth_Devices_Disable, smi::BLUETOOTH_RADIO_NUM, password);
                } catch (smi::ConfigError) {
                    cerr << "Warning: boot time config for Bluetooth is disabled. Setting runtime will have no effect."<< endl;
                }
                if (runtime) printInfo = true;
                if (boot) printTokens = true;
                break;
            case 4:
                enable = strtol(args, 0, 0);
                try {
                    smi::wirelessRadioControl(enable, boot, runtime, smi::Cellular_Radio_Enable, smi::Cellular_Radio_Disable, smi::WWAN_RADIO_NUM, password);
                } catch (smi::ConfigError) {
                    cerr << "Warning: boot time config for WWAN (Cellular) is disabled. Setting runtime will have no effect."<< endl;
                }
                if (runtime) printInfo = true;
                if (boot) printTokens = true;
                break;

            case 5:
                enable = strtol(args, 0, 0);
                smi::wirelessSwitchControl(enable, boot, runtime, smi::Wireless_Switch_Wireless_LAN_Control_Enable, smi::Wireless_Switch_Wireless_LAN_Control_Disable, smi::WLAN_SWITCH_CTL, password);
                if (runtime) printSwitchStatus = true;
                if (boot) printTokens = true;
                break;
            case 6:
                enable = strtol(args, 0, 0);
                smi::wirelessSwitchControl(enable, boot, runtime, smi::Wireless_Switch_Bluetooth_Control_Enable, smi::Wireless_Switch_Bluetooth_Control_Disable, smi::BLUETOOTH_SWITCH_CTL, password);
                if (runtime) printSwitchStatus = true;
                if (boot) printTokens = true;
                break;
            case 7:
                enable = strtol(args, 0, 0);
                smi::wirelessSwitchControl(enable, boot, runtime, smi::Wireless_Switch_Cellular_Control_Enable, smi::Wireless_Switch_Cellular_Control_Disable, smi::WWAN_SWITCH_CTL, password);
                if (runtime) printSwitchStatus = true;
                if (boot) printTokens = true;
                break;
            case 8:
                enable = strtol(args, 0, 0);
                smi::wirelessSwitchControl(enable, boot, runtime, smi::WiFi_Locator_Enable, smi::WiFi_Locator_Disable, smi::LOCATOR_SWITCH_CTL, password);
                if (runtime) printSwitchStatus = true;
                if (boot) printTokens = true;
                break;

            case 9:
                retval = smi::wirelessRadioStatus(smi::WLAN_RADIO_NUM);
                break;
            case 10:
                retval = smi::wirelessRadioStatus(smi::BLUETOOTH_RADIO_NUM);
                break;
            case 11:
                retval = smi::wirelessRadioStatus(smi::WWAN_RADIO_NUM);
                break;

            case 12:
                boot = true;
                break;
            case 13:
                boot = true;
                runtime = false;
                break;

            case 249:
                rawPassword = true;
                break;

            case 252:
                password = args;
                break;

            case 255:
                cout << "Libsmbios version:    " << LIBSMBIOS_RELEASE_VERSION << endl;
                cout << "dellWirelessCtl version: " << DELL_WIRELESS_CTL_VERSION << endl;
                exit(0);
                break;

            default:
                break;
            }
            free(args);
        }

        if ((!rawPassword) && (1 == SMBIOSGetSmiPasswordCoding()) && strlen(password.c_str())>0)
        {
            cerr << endl;
            cerr << "BIOS Password encoding has been detected as SCAN CODE format." << endl;
            cerr << "Automatically changing password from ASCII coding to en_US scancode format." << endl;
            cerr << "Use the --rawpassword option to disable this, for example, if you have " << endl;
            cerr << "another language keyboard, then manually convert the ASCII password to" << endl;
            cerr << "scan code format." << endl;
            cerr << endl;

            char *codedPass = new char[strlen(password.c_str())+1];
            memset(codedPass, 0, strlen(password.c_str())+1);
            SMBIOSMapAsciiTo_en_US_ScanCode(codedPass, password.c_str(), strlen(password.c_str()));
            password = codedPass;
            delete []codedPass;
        }


        if (printInfo)
        {
            printWirelessInfo();
            cout << endl;
        }
    
        if (printSwitchStatus)
        {
            switchStatus();
            cout << endl;
        }
        if (printTokens)
        {
            wirelessTokenStatus();
        }
    }
    catch( const exception &e )
    {
        cerr << "An Error occurred. The Error message is: " << endl << e.what() << endl;
        retval = 1;
    }
    catch ( ... )
    {
        cerr << "An Unknown Error occurred. Aborting." << endl;
        retval = 2;
    }

    return retval;
}

// cbClass 17
// cbSelect 11
// WiFi Control
// Entry/return values grouped by the value of cbARG1, byte0 which indicates the function to perform:
// 0x0 = Return Wireless Info
//  cbRES1      Standard return codes (0, -1, -2)
//  cbRES2      Info bit flags:
//      0 Hardware switch supported (1)
//      1 WiFi locator supported (1)
//      2 WLAN supported (1)
//      3 Bluetooth (BT) supported (1)
//      4 WWAN supported (1)
//      5 Wireless KBD supported (1)
//      6-7 Reserved (0)
//      8 WLAN installed (1)
//      9 BT installed (1)
//      10 WWAN installed (1)
//      11-15 Reserved (0)
//      16 Hardware (HW) switch is On (1)
//      17 WLAN disabled (1)
//      18 BT disabled (1)
//      19 WWAN disabled (1) 
//      20-31 Reserved (0) //  
//  cbRES3      NVRAM size in bytes
//  cbRES4, byte 0  NVRAM format version number

// 0x1 = Set QuickSet Radio Disable Flag
//  cbARG1, byte1    Radio ID value:
//      0 Radio Status
//      1 WLAN ID
//      2 BT ID
//      3 WWAN ID
//  cbARG1, byte2   Flag bits:
//      0 QuickSet disables radio (1)
//      1-7 Reserved (0)
//  cbRES1      Standard return codes (0, -1, -2)
//  cbRES2      QuickSet (QS) radio disable bit map:
//      0 QS disables WLAN
//      1 QS disables BT
//      2 QS disables WWAN
//      3-31 Reserved (0)

// 0x2 = Wireless Switch Configuration
//  cbARG1, byte1   Subcommand:
//      0 Get config
//      1 Set config
//      2 Set WiFi locator enable/disable
//  cbARG1,byte2    Switch settings (if byte 1==1):
//      0 WLAN switch control (1)
//      1 BT switch control (1)
//      2 WWAN switch control (1)
//      3-7 Reserved (0)
//  cbARG1, byte2   Enable bits (if byte 1==2):
//      0 Enable WiFi locator (1)
//  cbRES1      Standard return codes (0, -1, -2)
//  cbRES2      QuickSet radio disable bit map:
//      0 WLAN controlled by switch (1)
//      1 BT controlled by switch (1)
//      2 WWAN controlled by switch (1)
//      3-6 Reserved (0)
//      7 Wireless switch config locked (1)
//      8 WiFi locator enabled (1)
//      9-14 Reserved (0)
//      15 WiFi locator setting locked (1)
//      16-31 Reserved (0)

// 0x10 = Read Local Config Data (LCD)
//  cbARG1, byte1   NVRAM index low byte
//  cbARG1, byte2   NVRAM index high byte
//  cbRES1      Standard return codes (0, -1, -2)
//  cbRES2      4 bytes read from LCD[index]
//  cbRES3      4 bytes read from LCD[index+4]
//  cbRES4      4 bytes read from LCD[index+8]

// 0x11 = Write Local Config Data (LCD)
//  cbARG1, byte1   NVRAM index low byte
//  cbARG1, byte2   NVRAM index high byte
//  cbARG2      4 bytes to write at LCD[index]
//  cbARG3      4 bytes to write at LCD[index+4]
//  cbARG4      4 bytes to write at LCD[index+8]
//  cbRES1      Standard return codes (0, -1, -2)

// 0x12 = Populate Local Config Data from NVRAM
//  cbRES1      Standard return codes (0, -1, -2)

// 0x13 = Commit Local Config Data to NVRAM
//  cbRES1      Standard return codes (0, -1, -2)

