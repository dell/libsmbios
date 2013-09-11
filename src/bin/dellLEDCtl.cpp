/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:cindent:textwidth=0:
 *
 * Copyright (C) 2007 Dell Inc.
 *  by Ed H <eah1@yahoo.com>
 *  based on dellWirelessCtl by Michael E Brown <Michael_E_Brown@Dell.com>
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
#include <ctype.h>
//#define SHOW_STROBE_PERF
#if defined(SHOW_STROBE_PERF)
    #include <sys/time.h>
#endif
#include <time.h>
#include <string.h>

#include "smbios/ISmi.h"
#include "smbios/IToken.h"
#include "smbios/SystemInfo.h"
#include "getopts.h"

// always include last if included.
#include "smbios/message.h"

using namespace std;

#define LIST_DELL_LED_COLORS(_) \
_(Off) \
_(Ruby) \
_(Citrine) \
_(Amber) \
_(Peridot) \
_(Emerald) \
_(Jade) \
_(Topaz) \
_(Tanzanite) \
_(Aquamarine) \
_(Sapphire) \
_(Iolite) \
_(Amythest) \
_(Kunzite) \
_(Rhodolite) \
_(Coral) \
_(Diamond)

#define MK_DELL_LED_COLOR_ENUM(x) DELL_LED_COLOR_##x,
typedef enum { LIST_DELL_LED_COLORS(MK_DELL_LED_COLOR_ENUM) NUM_DELL_LED_COLORS } DELL_LED_COLOR;

#define MK_STRING(x) #x,
static const char *dell_led_color[] = { LIST_DELL_LED_COLORS(MK_STRING) };

struct options opts[] =
{
    { 1, "info", "Get LED info", "i", 0 },
    { 2, "zone1", "Set zone 1 color", "z1", 1 },
    { 3, "zone2", "Set zone 2 color", "z2", 1 },
    { 4, "zone3", "Set zone 3 color", "z3", 1 },
    { 5, "zone4", "Set zone 4 color", "z4", 1 },
    { 6, "level", "Set intensity level [0..7]", "l", 1 },
    { 7, "colors", "Show color list", "c", 0 },
    { 8, "boot", "Preserve colors across reboots", "b", 0 },
    { 9, "strobe", "Firetruck mode", "s", 0 },
    { 252, "password", "BIOS setup password", "p", 1 },
    { 249, "rawpassword", "Do not auto-convert password to scancodes", NULL, 0 },
    { 255, "version", "Display libsmbios version information", NULL, 0 },
    { 0, NULL, NULL, NULL, 0 }
};

#define DELL_LED_CTL_VERSION "0.1.0"

static void printLEDInfo()
{
    u32 args[4] = {0, 0, 0, 0};
    u32 res[4];

    // cbClass: 4
    // cbSelect: 7
    // name: Get LED Settings
    //
    // On return:
    // cbRES1     Return code
    //     0   Completed successfully
    //     -1  Completed with error
    //     -2  Function not supported
    // cbRES2   Current LED Settings
    //     byte 0      LED Color for Zone 1
    //     byte 1      LED Color for Zone 2
    //     byte 2      LED Color for Zone 3
    //     byte 3      bit [2:0] have the Intensity Level
    //     Bits 3-7  Reserved
    // cbRES3   CMOS LED Settings
    //     byte 0      LED Color for Zone 1
    //     byte 1      LED Color for Zone 2
    //     byte 2      LED Color for Zone 3
    //     byte 3      bit [2:0] have the Intensity Level
    //     bits 3-7  Reserved
    //
    // cbRES4   Zone4 LED Settings
    //     byte 0      Zone4 current led setting
    //     byte 1      Zone4 CMOS led setting
    //
    // Note : The Mapping of Zones like Zone 1 is Fans, Zone 2 is Speakers,etc
    // will be defined in the system specific BIOS Document.

    try
    {
        smi::doSimpleCallingInterfaceSmi(4, 7, args, res);

        cout << "LED Info:" << endl;

        u8 *p2 = (u8 *)&res[smi::cbRES2];
        u8 *p3 = (u8 *)&res[smi::cbRES3];
        u8 *p4 = (u8 *)&res[smi::cbRES4];

        cout << "\tCurrent zone 1: " << dell_led_color[p2[0]] << endl;
        cout << "\tCurrent zone 2: " << dell_led_color[p2[1]] << endl;
        cout << "\tCurrent zone 3: " << dell_led_color[p2[2]] << endl;
        cout << "\tCurrent zone 4: " << dell_led_color[p4[0]] << endl;
        cout << "\tCurrent intensity " << u32(p2[3] & 0x7) << endl;
        
        cout << "\tCMOS zone 1: " << dell_led_color[p3[0]] << endl;
        cout << "\tCMOS zone 2: " << dell_led_color[p3[1]] << endl;
        cout << "\tCMOS zone 3: " << dell_led_color[p3[2]] << endl;
        cout << "\tCMOS zone 4: " << dell_led_color[p4[1]] << endl;
        cout << "\tCMOS intensity " << u32(p3[3] & 0x7) << endl;
    }
    catch(smi::UnsupportedSmi)
    {
        cout << "LED Info not supported on this system." << endl;
        return;
    }

    // cbClass: 17
    // cbSelect: 12
    // Name: Touchpad (TP) & Media Board (MB) LED Control
    //
    // On entry:
    //     cbARG1, byte1       Subcommand:
    //     0 Get config
    //     1 Set config
    //     cbARG2, byte1    Control bits (if cbARG1==1):
    //     0 LED Inactive State:
    //         0=Off
    //         1=Low intensity
    //     1 TP LED Active State:
    //         0=No change (stay off or low)
    //         1=Turn on for 3 seconds
    //     2 MB LED Active State:
    //         0=No change (stay off or low)
    //         1=Turn on for 3 seconds
    //     3-7 Reserved (0)
    //
    // On return:
    //     cbRES1    Return code
    //     0   Completed successfully
    //     -1  Completed with error
    //     -2  Function not supported
    //     cbRES2    Error code (if cbRES1==-1)
    //     -1  Non-specific error
    //     cbARG3, byte1    Config bits:
    //     0 LED Inactive State:
    //         0=Off
    //         1=Low intensity
    //     1 TP LED Active State:
    //         0=No change (stay off or low)
    //         1=Turn on for 3 seconds
    //     2 MB LED Active State:
    //         0=No change (stay off or low)
    //         1=Turn on for 3 seconds
    //     3-7 Reserved (0)
    //
    // NOTE: The LED Inactive State bit controls both the Touchpad LED and the
    // Media Board LED. When this bit is clear (0), neither LED will be on when
    // their respective control is inactive. When this bit is set (1), both LEDs
    // will be on at low intensity when their respective control is inactive. The
    // control for the touchpad LED is the touchpad, and that control is active
    // when the touchpad is being touched. The control for the media board LED is
    // the media button, and that control is active when the media button is being
    // pressed. The responses to activity by the touchpad and media board LEDs are
    // controlled individually. Each of these LEDs can be set to remain in its
    // inactive state (off or low intensity) even when activity is detected, or to
    // turn on at normal intensity and stay on for 3 seconds when activity is
    // detected.

    try
    {
        memset( args, 0, sizeof(args) );

        u8 *p1 = (u8 *)&args[smi::cbARG1];

        // HACK HACK HACK I suspect this is really byte 0 (doc error?) - Ed
        p1[1] = 0;

        smi::doSimpleCallingInterfaceSmi(17, 12, args, res);

        cout << "Touchpad/Media Key LED Info:" << endl;

        u8 *p3 = (u8 *)&res[smi::cbRES3];

        cout << "\tInactive level:   " << ((p3[0] & 1) ? "Low" : "Off") << endl;
        cout << "\tTouchpad action:  " << ((p3[0] & 2) ? "Brighten" : "Unchanged") << endl;
        cout << "\tMedia key action: " << ((p3[0] & 4) ? "Brighten" : "Unchanged") << endl;
    }
    catch(smi::UnsupportedSmi)
    {
        cout << "Touchpad/Media Key LED Info not supported on this system." << endl;
        return;
    }
}

static inline bool zoneIsSet( DELL_LED_COLOR z )
{
    return( (z >= DELL_LED_COLOR_Off) && (z < NUM_DELL_LED_COLORS) );
}

static void setAllLEDs( int level, DELL_LED_COLOR zone[], bool preserve )
{
    u32 args[4] = {0, 0, 0, 0};
    u32 res[4];

    // cbClass: 4
    // cbClass: 6
    // name: Set LED Settings
    // On entry:
    // cbARG1:
    //     byte 0  LED Color for  Zone 1
    //     byte 1  LED Color for  Zone 2
    //     byte 2  LED Color for  Zone 3
    //     byte 3  bit [2:0] have the Intensity Level
    //     bits 3-7  Reserved
    // cbARG2:
    //     byte0 bit [0] - Make change Permanent Bit
    //
    // If bit 0 is 1, change the LED colors and save the setting to CMOS;
    // otherwise, change the LED colors but dont save to CMOS
    //
    // cbARG3:
    //     byte 0  LED Color for  Zone 4
    //
    // On return:
    // cbRES1      Return code
    //     0   Completed successfully
    //     -1  Completed with error
    //     -2  Function not supported
    //
    // Note : The Mapping of Zones like Zone 1 is Fans, Zone 2 is Speakers,etc
    // will be defined in the system specific BIOS Document.

    try
    {
        u8 *p1 = (u8 *)&args[smi::cbARG1];
        u8 *p2 = (u8 *)&args[smi::cbARG2];
        u8 *p3 = (u8 *)&args[smi::cbARG3];

        p1[0] = u8(zone[0]);
        p1[1] = u8(zone[1]);
        p1[2] = u8(zone[2]);
        p1[3] = level;

        p2[0] = preserve ? 1 : 0;
    
        p3[0] = u8(zone[3]);

        smi::doSimpleCallingInterfaceSmi(4, 6, args, res);
    }
    catch(smi::UnsupportedSmi)
    {
        cout << "Setting LED colors not supported on this system." << endl;
        return;
    }
}

static void setLEDs( int level, DELL_LED_COLOR zone[], bool preserve )
{
    u32 args[4] = {0, 0, 0, 0};
    u32 res[4];

    // first get current state (see printLEDInfo)...
    try
    {
        smi::doSimpleCallingInterfaceSmi(4, 7, args, res);

        u8 *p2 = (u8 *)&res[smi::cbRES2];
        //u8 *p3 = (u8 *)&res[smi::cbRES3];
        u8 *p4 = (u8 *)&res[smi::cbRES4];

        if (!zoneIsSet( zone[0] )) zone[0] = DELL_LED_COLOR(p2[0]);
        if (!zoneIsSet( zone[1] )) zone[1] = DELL_LED_COLOR(p2[1]);
        if (!zoneIsSet( zone[2] )) zone[2] = DELL_LED_COLOR(p2[2]);
        if (!zoneIsSet( zone[3] )) zone[3] = DELL_LED_COLOR(p4[0]);

        if ((level < 0) || (level > 7)) level = p2[3] & 0x7;
    }
    catch(smi::UnsupportedSmi)
    {
        cout << "LED Info not supported on this system." << endl;
        return;
    }

    // ...then set all the merged-in changes (see setAllLEDs)
    setAllLEDs( level, zone, preserve );
}

// just for fun...
static void strobeLEDs()
{
    int level;
    DELL_LED_COLOR zone[4];

    u32 args[4] = {0, 0, 0, 0};
    u32 res[4];

    // save the current state (see printLEDInfo)
    try
    {
        smi::doSimpleCallingInterfaceSmi(4, 7, args, res);

        u8 *p2 = (u8 *)&res[smi::cbRES2];
        //u8 *p3 = (u8 *)&res[smi::cbRES3];
        u8 *p4 = (u8 *)&res[smi::cbRES4];

        zone[0] = DELL_LED_COLOR(p2[0]);
        zone[1] = DELL_LED_COLOR(p2[1]);
        zone[2] = DELL_LED_COLOR(p2[2]);
        zone[3] = DELL_LED_COLOR(p4[0]);

        level = p2[3] & 0x7;
    }
    catch(smi::UnsupportedSmi)
    {
        cout << "LED Info not supported on this system." << endl;
        return;
    }

    // 3ms per change
    struct timespec nsleep = { 0, 3000000 };
    int cnt = 0;
#if defined(SHOW_STROBE_PERF)
    struct timeval start;
    gettimeofday( &start, NULL );
    nsleep.tv_nsec = 0;
#endif

    for (int z = DELL_LED_COLOR_Off + 1; z < NUM_DELL_LED_COLORS; ++z)
    {
        DELL_LED_COLOR t[4];
        t[0] = t[1] = t[2] = t[3] = DELL_LED_COLOR(z);
        for (int new_level = 0; new_level < 7; ++new_level)
        {
            setAllLEDs( new_level, t, false );
            nanosleep( &nsleep, NULL );
            ++cnt;
        }
        for (int new_level = 7; new_level > 0; --new_level)
        {
            setAllLEDs( new_level, t, false );
            nanosleep( &nsleep, NULL );
            ++cnt;
        }
    }
    for (int z = NUM_DELL_LED_COLORS - 2; z > DELL_LED_COLOR_Off + 1; --z)
    {
        DELL_LED_COLOR t[4];
        t[0] = t[1] = t[2] = t[3] = DELL_LED_COLOR(z);
        for (int new_level = 0; new_level < 7; ++new_level)
        {
            setAllLEDs( new_level, t, false );
            nanosleep( &nsleep, NULL );
            ++cnt;
        }
        for (int new_level = 7; new_level > 0; --new_level)
        {
            setAllLEDs( new_level, t, false );
            nanosleep( &nsleep, NULL );
            ++cnt;
        }
    }

#if defined(SHOW_STROBE_PERF)
    struct timeval end;
    gettimeofday( &end, NULL );
    float elapsed_millis = (end.tv_sec - start.tv_sec) * 1000.0f + (end.tv_usec - start.tv_usec) / 1000.0f;
    cout << elapsed_millis << "ms for " << cnt << " updates (" << elapsed_millis / cnt << "ms/update)" << endl;
#endif

    // restore everything
    setAllLEDs( level, zone, false );
}

int main (int argc, char **argv)
{
    int retval = 0;

    string password("");
    bool rawPassword = false;

    bool printInfo = false;
    bool setColors = false;
    bool strobe = false;
    bool preserve = false;

    int level = -1;
    DELL_LED_COLOR zone[4] =
    {
        NUM_DELL_LED_COLORS,
        NUM_DELL_LED_COLORS,
        NUM_DELL_LED_COLORS,
        NUM_DELL_LED_COLORS
    };

    try
    {
        char *args = 0;
        int c=0;
        while ( (c=getopts(argc, argv, opts, &args)) != 0 )
        {
            switch(c)
            {
            case 1:
                cout << "Libsmbios version      : " << SMBIOSGetLibraryVersionString() << endl;
                cout << "dellLEDCtl version: " << DELL_LED_CTL_VERSION << endl;
                printInfo = true;
                break;

            case 2: case 3: case 4: case 5:
                if (isdigit( args[0] ))
                {
                    zone[c - 2] = DELL_LED_COLOR(atoi( args ));
                    setColors = true;
                }
                else
                {
                    for (int i = 0; i < NUM_DELL_LED_COLORS; ++i)
                    {
                        if (strcasecmp( args, dell_led_color[i] ) == 0)
                        {
                            zone[c - 2] = DELL_LED_COLOR(i);
                            setColors = true;
                            break;
                        }
                    }
                }
                break;

            case 6:
                level = atoi( args );
                setColors = true;
                break;

            case 7:
                cout << "Color values:" << endl;
                for (int i = 0; i < NUM_DELL_LED_COLORS; ++i)
                {
                    cout << dell_led_color[i] << " = " << i << endl;
                }
                break;

            case 8:
                preserve = true;
                break;

            case 9:
                strobe = true;
                break;

            case 249:
                rawPassword = true;
                break;

            case 252:
                password = args;
                break;

            case 255:
                cout << "Libsmbios version:    " << SMBIOSGetLibraryVersionString() << endl;
                cout << "dellLEDCtl version: " << DELL_LED_CTL_VERSION << endl;
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
            printLEDInfo();
            cout << endl;
        }
    
        if (setColors)
        {
            setLEDs( level, zone, preserve );
        }
    
        if (strobe)
        {
            strobeLEDs();
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

// cbClass: 4
// cbSelect: 7
// name: Get LED Settings
//
// On return:
// cbRES1     Return code
//     0   Completed successfully
//     -1  Completed with error
//     -2  Function not supported
// cbRES2   Current LED Settings
// 	   byte 0      LED Color for Zone 1
// 	   byte 1      LED Color for Zone 2
// 	   byte 2      LED Color for Zone 3
// 	   byte 3      bit [2:0] have the Intensity Level
// 	   Bits 3-7  Reserved
// cbRES3   CMOS LED Settings
// 	   byte 0      LED Color for Zone 1
// 	   byte 1      LED Color for Zone 2
// 	   byte 2      LED Color for Zone 3
// 	   byte 3      bit [2:0] have the Intensity Level
// 	   bits 3-7  Reserved
//
// cbRES4   Zone4 LED Settings
// 	   byte 0      Zone4 current led setting
// 	   byte 1      Zone4 CMOS led setting
//
// Note : The Mapping of Zones like Zone 1 is Fans, Zone 2 is Speakers,etc
// will be defined in the system specific BIOS Document.

// cbClass: 4
// cbClass: 6
// name: Set LED Settings
// On entry:
// cbARG1:
// 	   byte 0  LED Color for  Zone 1
// 	   byte 1  LED Color for  Zone 2
// 	   byte 2  LED Color for  Zone 3
// 	   byte 3  bit [2:0] have the Intensity Level
// 	   bits 3-7  Reserved
// cbARG2:
// 	   byte0 bit [0] - Make change Permanent Bit
//
// If bit 0 is 1, change the LED colors and save the setting to CMOS;
// otherwise, change the LED colors but dont save to CMOS
//
// cbARG3:
// 	   byte 0  LED Color for  Zone 4
//
// On return:
// cbRES1      Return code
// 	   0   Completed successfully
// 	   -1  Completed with error
// 	   -2  Function not supported
//
// Note : The Mapping of Zones like Zone 1 is Fans, Zone 2 is Speakers,etc
// will be defined in the system specific BIOS Document.

// cbClass: 17
// cbSelect: 12
// Name: Touchpad (TP) & Media Board (MB) LED Control
//
// On entry:
//     cbARG1, byte1	   Subcommand:
// 	   0 Get config
// 	   1 Set config
//     cbARG2, byte1	   Control bits (if cbARG1==1):
// 	   0 LED Inactive State:
// 	       0=Off
// 	       1=Low intensity
// 	   1 TP LED Active State:
// 	       0=No change (stay off or low)
// 	       1=Turn on for 3 seconds
// 	   2 MB LED Active State:
// 	       0=No change (stay off or low)
// 	       1=Turn on for 3 seconds
// 	   3-7 Reserved (0)
//
// On return:
//     cbRES1	   Return code
// 	   0   Completed successfully
// 	   -1  Completed with error
// 	   -2  Function not supported
//     cbRES2	   Error code (if cbRES1==-1)
// 	   -1  Non-specific error
//     cbARG3, byte1	   Config bits:
// 	   0 LED Inactive State:
// 	       0=Off
// 	       1=Low intensity
// 	   1 TP LED Active State:
// 	       0=No change (stay off or low)
// 	       1=Turn on for 3 seconds
// 	   2 MB LED Active State:
// 	       0=No change (stay off or low)
// 	       1=Turn on for 3 seconds
// 	   3-7 Reserved (0)
//
// NOTE: The LED Inactive State bit controls both the Touchpad LED and the
// Media Board LED. When this bit is clear (0), neither LED will be on when
// their respective control is inactive. When this bit is set (1), both LEDs
// will be on at low intensity when their respective control is inactive. The
// control for the touchpad LED is the touchpad, and that control is active
// when the touchpad is being touched. The control for the media board LED is
// the media button, and that control is active when the media button is being
// pressed. The responses to activity by the touchpad and media board LEDs are
// controlled individually. Each of these LEDs can be set to remain in its
// inactive state (off or low intensity) even when activity is detected, or to
// turn on at normal intensity and stay on for 3 seconds when activity is
// detected.
