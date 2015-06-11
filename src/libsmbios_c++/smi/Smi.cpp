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

#if defined(DEBUG_SMI)
#define DEBUG_OUTPUT_ALL
#endif

// compat header should always be first header if including system headers
#define LIBSMBIOS_SOURCE
#include "smbios/compat.h"

//#include <iostream>
#include <string.h>

#include "SmiImpl.h"
#include "smbios/ISmbios.h"
#include "smbios/IToken.h"
#include "TokenLowLevel.h"

using namespace std;

/* work around broken VC6 compiler */
#define SIZEOF_KERNELBUF  (sizeof(kernel_buf) - sizeof(kernel_buf.command_buffer_start))

namespace smi
{


    //
    // ISmi functions
    //
    IDellCallingInterfaceSmi::IDellCallingInterfaceSmi()
    {}

    IDellCallingInterfaceSmi::~IDellCallingInterfaceSmi()
    {}

    DellCallingInterfaceSmiImpl::DellCallingInterfaceSmiImpl(SmiStrategy *initStrategy, u16 address, u8 code )
            :  buffer(0), bufferSize(0), smiStrategy(initStrategy)
    {
        // this is the only place where we use 'real' sizeof(kernel_buf), 
        // everywhere else should use SIZEOF_KERNELBUF
        memset( &kernel_buf, 0, sizeof(kernel_buf) );

        memset( &smi_buf, 0, sizeof(smi_buf) );
        memset( &argIsAddress, 0, sizeof(argIsAddress) );
        memset( &argAddressOffset, 0, sizeof(argAddressOffset) );

        kernel_buf.magic = KERNEL_SMI_MAGIC_NUMBER;
        kernel_buf.ebx = 0;
        kernel_buf.ecx   = DELL_CALLINTF_SMI_MAGIC_NUMBER;
        kernel_buf.command_address = address;
        kernel_buf.command_code = code;

        /* default to "not handled" */
        smi_buf.cbRES1 = -3;
    }

    DellCallingInterfaceSmiImpl::~DellCallingInterfaceSmiImpl()
    {
        if(buffer)
        {
            delete [] buffer;
            buffer = 0;
            bufferSize = 0;
        }
    }

    const u8 *DellCallingInterfaceSmiImpl::getBufferPtr()
    {
        return buffer;
    }

    void DellCallingInterfaceSmiImpl::setBufferSize(size_t newSize)
    {
        if ( bufferSize != newSize )
        {
            delete [] buffer;
            buffer = new u8[newSize];
            memset(buffer, 0, newSize);

            bufferSize=newSize;
        }
    }

    void DellCallingInterfaceSmiImpl::setBufferContents(const u8 *src, size_t size)
    {
        if(!bufferSize)
            throw SmiExceptionImpl("Output buffer not large enough.");

        memcpy(buffer, src, bufferSize<size?bufferSize:size);
    }

    void DellCallingInterfaceSmiImpl::execute()
    {
        smiStrategy->lock() ;
        smiStrategy->setSize( SIZEOF_KERNELBUF + sizeof(smi_buf) + bufferSize );

        size_t baseAddr = smiStrategy->getPhysicalBufferBaseAddress();
        for( int i=0; i<4; i++)
            if( argIsAddress[i] )
                smi_buf.inputArgs[i] = static_cast<u32>(baseAddr + SIZEOF_KERNELBUF + sizeof(smi_buf) + argAddressOffset[i]);

        smiStrategy->addInputBuffer(reinterpret_cast<u8 *>(&kernel_buf), SIZEOF_KERNELBUF);
        smiStrategy->addInputBuffer(reinterpret_cast<u8 *>(&smi_buf), sizeof(smi_buf));
        if(buffer)
            smiStrategy->addInputBuffer(buffer, bufferSize);

        smiStrategy->execute();

        smiStrategy->getResultBuffer(reinterpret_cast<u8 *>(&kernel_buf), SIZEOF_KERNELBUF);
        smiStrategy->getResultBuffer(reinterpret_cast<u8 *>(&smi_buf), sizeof(smi_buf));
        if(buffer)
            smiStrategy->getResultBuffer(reinterpret_cast<u8 *>(buffer), bufferSize);

        smiStrategy->finish();

        if( -6 == smi_buf.cbRES1 )
            throw SmiExceptionImpl("Output buffer not large enough.");

        if( -5 == smi_buf.cbRES1 )
            throw SmiExceptionImpl("Output buffer format error.");

        if( -3 == smi_buf.cbRES1 )
            throw UnhandledSmiImpl("Unhandled SMI call.");

        if( -2 == smi_buf.cbRES1 )
            throw UnsupportedSmiImpl("Unsupported SMI call.");

        if( -1 == smi_buf.cbRES1 )
            throw SmiExecutedWithErrorImpl("BIOS returned error for SMI call.");
    }

    void DellCallingInterfaceSmiImpl::setArgAsPhysicalAddress( u8 argNumber, u32 bufferOffset )
    {
        if( argNumber >= 4 )
            throw ParameterErrorImpl("Internal programming error. Argument must be in range 0..3");

        argIsAddress[argNumber] = true;
        argAddressOffset[argNumber] = bufferOffset;
    }


    void DellCallingInterfaceSmiImpl::setClass( u16 newClass )
    {
        smi_buf.smiClass = newClass;
    }

    void DellCallingInterfaceSmiImpl::setSelect( u16 newSelect )
    {
        smi_buf.smiSelect = newSelect;
    }

    void DellCallingInterfaceSmiImpl::setArg( u8 argNumber, u32 argValue )
    {
        if( argNumber >= 4 )
            throw ParameterErrorImpl("Internal programming error. Argument must be in range 0..3");

        smi_buf.inputArgs[ argNumber ] = argValue;
    }

    u32 DellCallingInterfaceSmiImpl::getRes( u8 resNumber ) const
    {
        if( resNumber >= 4 )
            throw ParameterErrorImpl("Internal programming error. Result request must be in range 0..3");

        return smi_buf.outputRes[resNumber];
    }


    /**************************************************************************
            HELPER FUNCTIONS  (Non-member functions)
     *************************************************************************/

    std::auto_ptr<smi::IDellCallingInterfaceSmi> setupCallingInterfaceSmi(u16 smiClass, u16 select, const u32 args[4])
    {
        std::auto_ptr<smi::IDellCallingInterfaceSmi> smi = smi::SmiFactory::getFactory()->makeNew(smi::SmiFactory::DELL_CALLING_INTERFACE_SMI);

        smi->setClass( smiClass );
        smi->setSelect( select );
        smi->setArg(0, args[0]);
        smi->setArg(1, args[1]);
        smi->setArg(2, args[2]);
        smi->setArg(3, args[3]);

        return smi;
    }

    void doSimpleCallingInterfaceSmi(u16 smiClass, u16 select, const u32 args[4], u32 res[4])
    {

        std::auto_ptr<smi::IDellCallingInterfaceSmi> smi(
            setupCallingInterfaceSmi(smiClass, select, args));

        smi->execute();

        res[0] = smi->getRes(0);
        res[1] = smi->getRes(1);
        res[2] = smi->getRes(2);
        res[3] = smi->getRes(3);
    }

    /*
       An application that will attempt to set information via any Security-Key-protected Calling Interface function must first acquire a proper Security Key.  It does this by performing the following steps:

       1.Check to see if an Administrator Password is set (Class 10, Selector 0 or 3). If yes, go to 2; otherwise, go to 3.

       2.Verify the Administrator Password (Class 10 Selector 1 or 4). If the password is verified (cbRES1 == 0), read the Security Key from cbRES2, and use it on subsequent set functions where it is required. If the password does not verify (cbRES1 == -1), repeat step 2 until it does verify; otherwise, subsequent set functions protected by the Administrator Password will be rejected by the BIOS if it supports the Security Key feature.

       3.Check to see if a User Password is set (Class 9, Selector 0 or 3). If yes, go to 4; otherwise, no Security Key will be needed to change data through the Calling Interface, and the caller can use any value at all for the Security Key when using any Security-Key-protected Calling Interface function.

       4.Verify the User Password (Class 9 Selector 1 or 4). If the password is verified (cbRES1 == 0), read the Security Key from cbRES2, and use it on subsequent set functions where it is required. If the password does not verify (cbRES1 == -1), repeat step 4 until it does verify; otherwise, subsequent set functions protected by the User Password will be rejected by the BIOS if it supports the Security Key feature.

       */
    static bool getPasswordPropertiesII(u16 which, u8 &maxLen, u8 &minLen, u8 &props)
    {
        if( which != 9 && which != 10 )
            throw ParameterErrorImpl("Internal programming error. Argument must be either 9 or 10.");

        bool hasPw = false;

        u32 args[4] = {0,}, res[4] = {0,};

        // select 3 == get properties
        doSimpleCallingInterfaceSmi(which, 3, args, res);

        // byte 0 of res[1] shows password status
        // 2 = password not installed
        // 3 = password disabled by jumper
        if ( (res[1] & 0xFF)==2 || (res[1] & 0xFF)==3 )
            goto out;

        DCERR( "getPasswordPropertiesII()" << hex << endl ); 
        DCERR( "res[0]: " << res[0] << endl);
        DCERR( "res[1]: " << res[1] << endl);
        DCERR( "res[2]: " << res[2] << endl);
        DCERR( "res[3]: " << res[3] << endl);

        hasPw = true;
        maxLen = static_cast<u8>((res[1] & 0x0000FF00) >> 8);
        minLen = static_cast<u8>((res[1] & 0x00FF0000) >> 16);
        props  = static_cast<u8>((res[1] & 0xFF000000) >> 24);

out:
        return hasPw;
    }

    bool getPasswordStatus(u16 which)
    {
        if( which != 9 && which != 10 )
            throw ParameterErrorImpl("Internal programming error. Argument must be either 9 or 10.");
        try
        {
            u32 args[4] = {0,}, res[4] = {0,};
            doSimpleCallingInterfaceSmi(which, 0, args, res);
            //1 = pass not installed, 3 = pass not installed, and only setable by an admin
            if( (res[0] & 0xFF) == 1 || ( res[0] & 0xFF ) == 3)
                return false;
            return true;
        }
        catch(const exception &)
        {
            //if we caught any exceptions, try the next method
        }
        u8 max,min,props;
        return getPasswordPropertiesII(which,max,min,props);
    }

    static u32 getAuthenticationKeyII(const string &password)
    {
        u32 authKey = 0;

        DCERR( "getAuthenticationKeyII()" << endl );

        u16 toCheck[2] = { class_admin_password, class_user_password };
        DCERR( "  trying auth keys" << endl);

        // try admin password first, then user password
        for( int i=0; i<2; i++ )
        {
            DCERR( "  trying class code: " << toCheck[i] << endl);

            u8 maxLen=0, minLen=0, props=0;
            // try next password type if no password set
            try
            {
                if( ! getPasswordPropertiesII(toCheck[i], maxLen, minLen, props) )
                    continue;
            }
            catch( const exception & )
            {
                // usually get here for unsupported SMI exception.
                // in which case, it makes no sense to continue 
                //DCERR( "  Caught something." << e.what() << endl);
                continue;
            }

            DCERR("has a password." << hex << endl);
            DCERR( "  max len: " << (int)maxLen << endl);
            DCERR( "  min len: " << (int)minLen << endl);
            DCERR( "  props  : " << hex << props << endl);

            u32 args[4] = {0,};
            // select 4 == verify password
            std::auto_ptr<smi::IDellCallingInterfaceSmi> smi(setupCallingInterfaceSmi(toCheck[i], 4, args));
            smi->setBufferContents(reinterpret_cast<const u8*>(password.c_str()), min(strlen(password.c_str()), (size_t) maxLen));
            smi->setArgAsPhysicalAddress( 0, 0 );
            smi->execute();

            DCERR("after verify:"<< endl);
            DCERR("res[0]: " << smi->getRes(0) << endl; );
            DCERR("res[1]: " << smi->getRes(1) << endl; );
            DCERR("res[2]: " << smi->getRes(2) << endl; );
            DCERR("res[3]: " << smi->getRes(3) << endl; );

            if(! smi->getRes(0))
                authKey = smi->getRes(1);
            else
                throw PasswordVerificationFailedImpl("BIOS setup password enabled, but given password does not match.");

            // if this password is installed, no sense in checking the other, as it will not work.
            // highest priority password always takes precedence
            break;
        }

        return authKey;
    }

    u32 getAuthenticationKey(const string &password)
    {
        u32 authKey = 0;

        DCERR("getAuthenticationKey(" << password << ")" << endl);

        // try admin password first, then user password
        u16 toCheck[2] = { class_admin_password, class_user_password };
        DCERR("  trying auth keys" << endl);

        for( int i=0; i<2; i++ )
        {
            DCERR("    trying class code: " << toCheck[i] << endl);
            try
            {
                u32 args[4] = {0,}, res[4] = {0,};
                doSimpleCallingInterfaceSmi(toCheck[i], 0, args, res);

                // no password of this type installed if res[0] == 0
                if( res[0] != 0 )
                    continue;
            }
            catch(const SmiException &)
            {
                // We should only get here under the following circumstances:
                // - unsupported SMI call
                // - unhandled SMI call
                // - could not talk to dcdbas driver
                continue;
            }

            // If we get here, that means a password of type toCheck[i]
            // is installed.
            //
            DCERR("      password installed" << endl);

            u32 args[4] = {0}, res[4] = {0,};
            strncpy(reinterpret_cast<char *>(args), password.c_str(), 2 * sizeof(u32));

            DCERR("    args are  : 0x" << args[0] << " 0x" << args[1] << " 0x" << args[2] << " 0x" << args[3] << endl);

            // if SMI above succeeded, this should too, no exception handling
            doSimpleCallingInterfaceSmi(toCheck[i], 1, args, res);

            DCERR("    res was  : 0x" << res[0] << " 0x" << res[1] << " 0x" << res[2] << " 0x" << res[3] << endl);
            if( res[0] == 0 )
                authKey = res[1];
            else
                throw PasswordVerificationFailedImpl("BIOS setup password enabled, but given password does not match.");

            // if this password is installed, no sense in checking the other, as it will not work.
            // highest priority password always takes precedence
            break;
        }

        // if this didn't work, try other method.
        if( ! authKey )
            authKey = getAuthenticationKeyII( password );

        return authKey;
    }

    password_format_enum getPasswordFormat()
    {
        password_format_enum format = PW_FORMAT_UNKNOWN;

        try
        {
            u32 args[4] = {0,}, res[4] = {0,};
            doSimpleCallingInterfaceSmi(class_admin_password, 0, args, res);
            format = PW_FORMAT_SCAN_CODE;
            goto out;
        }
        catch(const exception &)
        { }

        try
        {
            u32 args[4] = {0,}, res[4] = {0,};
            doSimpleCallingInterfaceSmi(class_user_password, 0, args, res);
            format = PW_FORMAT_SCAN_CODE;
            goto out;
        }
        catch(const exception &)
        { }

        try
        {
            u8 maxLen=0, minLen=0, props=0;
            getPasswordPropertiesII(class_admin_password, maxLen, minLen, props);
            format = PW_FORMAT_SCAN_CODE;
            if (props & 0x01)
                format = PW_FORMAT_ASCII;
            goto out;
        }
        catch(const exception &)
        { }

        try
        {
            u8 maxLen=0, minLen=0, props=0;
            getPasswordPropertiesII(class_user_password, maxLen, minLen, props);
            format = PW_FORMAT_SCAN_CODE;
            if (props & 0x01)
                format = PW_FORMAT_ASCII;
            goto out;
        }
        catch(const exception &)
        { }

out:
        return format;
    }

    static u32 readSetting(u16 select, u32 location, u32 *minValue, u32 *maxValue)
    {
        u32 args[4] = {location, 0,}, res[4] = {0,};
        doSimpleCallingInterfaceSmi(0, select, args, res); // 0 == class code for setting/batter/ac/systemstatus
        if(minValue)
            *minValue = res[2];
        if(maxValue)
            *maxValue = res[3];
        return res[1]; // current value
    }

    u32 readNVStorage(u32 location, u32 *minValue, u32 *maxValue)
    {
        return readSetting(0, location, minValue, maxValue); // 0 = select code for nv storage
    }

    u32 readBatteryModeSetting(u32 location, u32 *minValue, u32 *maxValue)
    {
        return readSetting(1, location, minValue, maxValue); // 1 = select code for battery mode
    }

    u32 readACModeSetting(u32 location, u32 *minValue, u32 *maxValue)
    {
        return readSetting(2, location, minValue, maxValue); // 2 = select code for ac mode
    }

    u32 readSystemStatus(u32 *failingSensorHandle)
    {
        // 3 = select code for system status
        // 1st 0 == dummy location value
        // 2nd 0 == dummy max value pointer
        return readSetting(3, 0, failingSensorHandle, 0);
    }


    static u32 writeSetting(const std::string &password, u16 select, u32 location, u32 newValue, u32 *minValue, u32 *maxValue)
    {
        u32 args[4] = {location, newValue,}, res[4] = {0,};

        // go twice. Once without security key, once by trying password given.
        for(int i=0; i<2; i++)
        {
            try
            {
                // 0 == class code for writing to setting/battery/ac/systemstatus
                DCERR("Try #" << i << " for writeSetting()" << endl);
                DCERR("    args are  : 0x" << args[0] << " 0x" << args[1] << " 0x" << args[2] << " 0x" << args[3] << endl);
                doSimpleCallingInterfaceSmi(1, select, args, res);
                DCERR("    res was  : 0x" << res[0] << " 0x" << res[1] << " 0x" << res[2] << " 0x" << res[3] << endl);
                break;
            }
            catch(const SmiExecutedWithError &)
            {
                // on second time through, just pass exception upwards.
                if(i==1)
                    throw;

                DCERR("Executed with error, try password..." << endl);
                args[2] = getAuthenticationKey(password);
            }
        }

        if(minValue)
            *minValue = res[2];
        if(maxValue)
            *maxValue = res[3];
        return res[1]; // current value
    }

    u32 writeNVStorage(const std::string &password, u32 location, u32 value, u32 *minValue, u32 *maxValue)
    {
        return writeSetting(password, 0, location, value, minValue, maxValue);
    }

    u32 writeBatteryModeSetting(const std::string &password, u32 location, u32 value, u32 *minValue, u32 *maxValue)
    {
        return writeSetting(password, 1, location, value, minValue, maxValue);
    }

    u32 writeACModeSetting(const std::string &password, u32 location, u32 value, u32 *minValue, u32 *maxValue)
    {
        return writeSetting(password, 2, location, value, minValue, maxValue);
    }

    void getDisplayType(u32 &type, u32 &resolution, u32 &memSizeX256kb)
    {
        u32 args[4] = {0,}, res[4] = {0,};
        doSimpleCallingInterfaceSmi(4, 0, args, res);

        type = (res[1] & 0x00FF);
        resolution = (res[1] & 0xFF00) >> 8;
        memSizeX256kb = res[2];
    }

    void getPanelResolution(u32 &horiz, u32 &vert)
    {
        u32 args[4] = {0,}, res[4] = {0,};
        doSimpleCallingInterfaceSmi(4, 1, args, res);

        horiz = (res[1] & 0x0000FFFF);
        vert  = (res[1] & 0xFFFF0000) >> 16;
    }

    void getActiveDisplays(u32 &bits)
    {
        u32 args[4] = {0,}, res[4] = {0,};
        doSimpleCallingInterfaceSmi(4, 2, args, res);

        bits = res[1];
    }

    void setActiveDisplays(u32 &bits)
    {
        u32 args[4] = {bits, 0,}, res[4] = {0,};
        doSimpleCallingInterfaceSmi(4, 3, args, res);
    }

    void getPropertyOwnershipTag(char *tagBuf, size_t size)
    {
        u32 args[4] = {0,};
        // class 20 == property tag
        std::auto_ptr<smi::IDellCallingInterfaceSmi> smi(setupCallingInterfaceSmi(20, 0, args));
        smi->setBufferSize(80); // 80 is max len, making sure it doesn't overflow now. :-)
        smi->setArgAsPhysicalAddress( 0, 0 );
        smi->execute();
        strncpy( tagBuf, reinterpret_cast<const char*>(smi->getBufferPtr()), size < 80? size:80);
        tagBuf[size-1] = '\0';
    }

    void setPropertyOwnershipTag(const string password, const char *newTag, size_t size)
    {
        u32 args[4] = {0,};

        for(int i=0; i<2; i++)
        {
            try
            {
                // class 20 == property tag
                std::auto_ptr<smi::IDellCallingInterfaceSmi> smi(setupCallingInterfaceSmi(20, 1, args));
                smi->setBufferSize(120); // 80 is max len, making sure it doesn't overflow now. :-)
                smi->setBufferContents(reinterpret_cast<const u8*>(newTag), min(strlen(newTag), (size_t)80));
                smi->setArgAsPhysicalAddress( 0, 0 );
                smi->execute();
                break;
            }
            catch(const SmiExecutedWithError &)
            {
                // on second time through, just pass exception upwards.
                if(i==1)
                    throw;

                //cout << "Caught error. Might be bad password. Trying password: " << password << endl;
                args[1] = getAuthenticationKey(password);
            }
        }
    }


    // token list for wireless tokens. Probably not the best place for this,
    // but until something better comes along...
    const int Bluetooth_Devices_Disable = 0x0153;  // docs appear to be wrong. They say 0x0152, but this looks backwards from reality
    const int Bluetooth_Devices_Enable = 0x0152;  // docs appear to be wrong. They say 0x0153, but this looks backwards from reality
    const int Cellular_Radio_Disable = 0x017B;
    const int Cellular_Radio_Enable = 0x017C;
    const int WiFi_Locator_Disable = 0x017D;
    const int WiFi_Locator_Enable = 0x017E;
    const int Wireless_LAN_Disable = 0x017F;
    const int Wireless_LAN_Enable = 0x0180;
    const int Wireless_Switch_Bluetooth_Control_Disable = 0x0181;
    const int Wireless_Switch_Bluetooth_Control_Enable = 0x0182;
    const int Wireless_Switch_Cellular_Control_Disable = 0x0183;
    const int Wireless_Switch_Cellular_Control_Enable = 0x0184;
    const int Wireless_Switch_Wireless_LAN_Control_Disable = 0x0185;
    const int Wireless_Switch_Wireless_LAN_Control_Enable = 0x0186;
    const int Radio_Transmission_Enable = 0x010c;
    const int Radio_Transmission_Disable = 0x010d;
    const int Wireless_Device_Disable = 0x0114;
    const int Wireless_Device_App_Control = 0x0115;
    const int Wireless_Device_App_Or_Hotkey_Control = 0x0116;

    // cbClass 17
    // cbSelect 11
    // WiFi Control
    // Entry/return values grouped by the value of cbARG1, byte0 which indicates the function to perform:
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
    void wirelessRadioControl(bool enable, bool boot, bool runtime, int enable_token, int disable_token, int radioNum, std::string password)
    {
        if (boot)
            smbios::activateToken( (enable ?
                enable_token :
                disable_token),
                password
                );
        if (runtime)
        {
            if (enable && !smbios::isTokenActive(enable_token))
                throw ConfigErrorImpl("boot time config disabled, runtime setting has no effect.");

            u32 disable = enable ? 0:1;
            u32 args[4] = {(1 | (static_cast<u32>(radioNum)<<8) | ((disable)<<16)), 0, 0, 0};
            u32 res[4] = {0,};
            doSimpleCallingInterfaceSmi(17, 11, args, res);
        }
    }


    static void switchControl(u32 whichConfig, u32 whichSwitch, bool enable)
    {
        std::auto_ptr<smi::IDellCallingInterfaceSmi> smi = smi::SmiFactory::getFactory()->makeNew(smi::SmiFactory::DELL_CALLING_INTERFACE_SMI);
        smi->setClass( 17 );  /* ? */
        smi->setSelect( 11 );  /* WiFi Control */
    
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
        smi->setArg(smi::cbARG1, 0x2);
        smi->execute();
    
        u32 oldConfig = smi->getRes(smi::cbRES2);
        if (whichConfig == 1)
            oldConfig &= 0xFF;
        else if (whichConfig == 2)
            oldConfig = ((oldConfig>>8) & 0xFF);
    
        u32 newConfig = (oldConfig & ~whichSwitch) | ((enable?1:0) * whichSwitch);
        smi->setArg(smi::cbARG1, (0x2 | (whichConfig << 8) | (newConfig << 16)));
        smi->execute();
    }
    
    void wirelessSwitchControl(bool enable, bool boot, bool runtime, int enable_token, int disable_token, int switchNum, std::string password)
    {
        int intSwitchConfig = 0, intSwitchNum = 0;
        switch(switchNum)
        {
            case WLAN_SWITCH_CTL:
                intSwitchConfig = 1;
                intSwitchNum = 1;
                break;
            case BLUETOOTH_SWITCH_CTL:
                intSwitchConfig = 1;
                intSwitchNum = 2;
                break;
            case WWAN_SWITCH_CTL:
                intSwitchConfig = 1;
                intSwitchNum = 4;
                break;
            case LOCATOR_SWITCH_CTL:
                intSwitchConfig = 2;
                intSwitchNum = 1;
                break;
            default:
                throw ParameterErrorImpl("Invalid switch number passed to wirelessSwitchControl()");
        }
        
        if (boot)
            smbios::activateToken( (enable ?
                enable_token :
                disable_token),
                password
                );
        if (runtime)
            switchControl(intSwitchConfig, intSwitchNum, enable);
    }

    radioStatusCode wirelessRadioStatus(radioNum which, std::ostream &cout, u32 defRes2)
    {
        radioStatusCode ret = STATUS_UNKNOWN;
        try
        {
            u32 args[4] = {0,}, res[4] ={0,};
            if (!defRes2)
                smi::doSimpleCallingInterfaceSmi(17, 11, args, res);
            else
                res[smi::cbRES2] = defRes2;
    
            int supported_bit=0, installed_bit=0, disabled_bit=0;
            string name;
            switch(which)
            {
                case smi::WLAN_RADIO_NUM:
                    supported_bit = 2;
                    installed_bit = 8;
                    disabled_bit = 17;
                    name = "WLAN";
                    break;
                case smi::BLUETOOTH_RADIO_NUM:
                    supported_bit = 3;
                    installed_bit = 9;
                    disabled_bit = 18;
                    name = "Bluetooth";
                    break;
                case smi::WWAN_RADIO_NUM:
                    supported_bit = 4;
                    installed_bit = 10;
                    disabled_bit = 19;
                    name = "WWAN";
                    break;
            }
    
            cout << "Radio Status for " << name << ":" << endl;
            if (res[smi::cbRES2] & (1 << supported_bit))
            {
                cout << "\t" << name << " supported" << endl;
                cout << "\t" << name << " " << ((res[smi::cbRES2] & (1 << installed_bit)) ? "installed":"not installed") << endl;
                cout << "\t" << name << " " << ((res[smi::cbRES2] & (1 << disabled_bit)) ? "disabled" : "enabled") << endl;
    
                ret = STATUS_DISABLED;
                if (!(res[smi::cbRES2] & (1 << installed_bit)))
                    ret = STATUS_NOT_PRESENT;
                else if (!(res[smi::cbRES2] & (1 << disabled_bit)))
                    ret = STATUS_ENABLED;
            } else {
                cout << "\t" << name << " not supported" << endl;
                ret = STATUS_UNSUPPORTED;
        }
            cout << "\tStatus Code: " << ret << endl;
        } catch (smi::UnsupportedSmi &) {
            // this interface not available...
        }
        return ret;
    }

}
