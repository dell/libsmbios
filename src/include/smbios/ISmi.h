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


#ifndef SMIINTERFACE_H
#define SMIINTERFACE_H

// compat header should always be first header, if system headers included
#include "smbios/compat.h"

#include <iostream>
#include <memory>

// types.h should be first user-defined header.
#include "smbios/types.h"

#include "smbios/IFactory.h"
#include "smbios/IException.h"

// abi_prefix should be last header included before declarations
#include "smbios/config/abi_prefix.hpp"

namespace smi
{
    //! Abstract base class for the smi read write operations
    /**
    */
    // Exceptions
    DECLARE_EXCEPTION( SmiException );
    DECLARE_EXCEPTION_EX( InvalidSmiMode, smi, SmiException );
    DECLARE_EXCEPTION_EX( ParameterError, smi, SmiException );
    DECLARE_EXCEPTION_EX( UnhandledSmi, smi, SmiException );
    DECLARE_EXCEPTION_EX( UnsupportedSmi, smi, SmiException );
    DECLARE_EXCEPTION_EX( SmiExecutedWithError, smi, SmiException );
    DECLARE_EXCEPTION_EX( PasswordVerificationFailed, smi, SmiException );
    DECLARE_EXCEPTION_EX( ConfigError, smi, SmiException );

    class IDellCallingInterfaceSmi
    {
    public:
        virtual ~IDellCallingInterfaceSmi();
        // compiler-generated copy and operator = are good for now, I think.

        virtual void setClass( u16 newClass ) = 0;
        virtual void setSelect( u16 newSelect ) = 0;
        virtual void setArg( u8 argNumber, u32 argValue ) = 0;
        virtual u32  getRes( u8 resNumber ) const = 0;
        virtual void setArgAsPhysicalAddress( u8 argNumber, u32 bufferOffset ) = 0;
        virtual void setBufferSize(size_t size) = 0;
        virtual void setBufferContents(const u8 *, size_t size) = 0;
        virtual const u8 *getBufferPtr() = 0;

        virtual void execute() = 0;

    protected:
        explicit IDellCallingInterfaceSmi();
    };

    // NOTE: does not hand out singletons
    class SmiFactory : public virtual factory::IFactory
    {
    public:
        enum { DELL_CALLING_INTERFACE_SMI_RAW, DELL_CALLING_INTERFACE_SMI,};

        static SmiFactory *getFactory();
        virtual ~SmiFactory() throw();
        virtual std::auto_ptr<IDellCallingInterfaceSmi> makeNew(u8 type) = 0; // not for use
    protected:
        SmiFactory();
    };

    enum {cbARG1 = 0, cbARG2 = 1, cbARG3 = 2, cbARG4 = 3};
    enum {cbRES1 = 0, cbRES2 = 1, cbRES3 = 2, cbRES4 = 3};

    // non-member helper functions
    // These encapsulate some common calling-interface SMI functions
    //
    void doSimpleCallingInterfaceSmi(u16 smiClass, u16 select, const u32 args[4], u32 res[4]);
    std::auto_ptr<smi::IDellCallingInterfaceSmi> setupCallingInterfaceSmi(u16 smiClass, u16 select, const u32 args[4]);
    u32 getAuthenticationKey(const std::string &password);

    enum password_format_enum { PW_FORMAT_UNKNOWN, PW_FORMAT_SCAN_CODE, PW_FORMAT_ASCII };
    password_format_enum getPasswordFormat();

#if 0
    // not yet implemented
    std::string getServiceTag();
    void setServiceTag(const std::string &password, const std::string &newTag);
    std::string getAssetTag();
    void setAssetTag(const std::string &password, const std::string &newTag);
#endif

    bool getPasswordStatus(u16 which);

    u32 readNVStorage         (u32 location, u32 *minValue, u32 *maxValue);
    u32 readBatteryModeSetting(u32 location, u32 *minValue, u32 *maxValue);
    u32 readACModeSetting     (u32 location, u32 *minValue, u32 *maxValue);
    u32 readSystemStatus(u32 *failingSensorHandle);

    u32 writeNVStorage         (const std::string &password, u32 location, u32 value, u32 *minValue, u32 *maxValue);
    u32 writeBatteryModeSetting(const std::string &password, u32 location, u32 value, u32 *minValue, u32 *maxValue);
    u32 writeACModeSetting     (const std::string &password, u32 location, u32 value, u32 *minValue, u32 *maxValue);

    void getDisplayType(u32 &type, u32 &resolution, u32 &memSizeX256kb);
    void getPanelResolution(u32 &horiz, u32 &vert);
    void getActiveDisplays(u32 &bits);
    void setActiveDisplays(u32 &bits);

    void getPropertyOwnershipTag(char *tagBuf, size_t size);
    void setPropertyOwnershipTag(const std::string password, const char *newTag, size_t size);

    // newer interface
    extern const int Bluetooth_Devices_Disable;  // docs appear to be wrong. They say 0x0152, but this looks backwards from reality
    extern const int Bluetooth_Devices_Enable;  // docs appear to be wrong. They say 0x0153, but this looks backwards from reality
    extern const int Cellular_Radio_Disable;
    extern const int Cellular_Radio_Enable;
    extern const int WiFi_Locator_Disable;
    extern const int WiFi_Locator_Enable;
    extern const int Wireless_LAN_Disable;
    extern const int Wireless_LAN_Enable;
    extern const int Wireless_Switch_Bluetooth_Control_Disable;
    extern const int Wireless_Switch_Bluetooth_Control_Enable;
    extern const int Wireless_Switch_Cellular_Control_Disable;
    extern const int Wireless_Switch_Cellular_Control_Enable;
    extern const int Wireless_Switch_Wireless_LAN_Control_Disable;
    extern const int Wireless_Switch_Wireless_LAN_Control_Enable;

    // old interface
    extern const int Radio_Transmission_Enable;
    extern const int Radio_Transmission_Disable;
    extern const int Wireless_Device_Disable;
    extern const int Wireless_Device_App_Control;
    extern const int Wireless_Device_App_Or_Hotkey_Control;

    enum radioNum { WLAN_RADIO_NUM=1, BLUETOOTH_RADIO_NUM=2, WWAN_RADIO_NUM=3 };
    void wirelessRadioControl(bool enable, bool boot, bool runtime, int enable_token, int disable_token, int radioNum, std::string password);

    enum { WLAN_SWITCH_CTL=1, BLUETOOTH_SWITCH_CTL=2, WWAN_SWITCH_CTL=4, LOCATOR_SWITCH_CTL=5 };
    void wirelessSwitchControl(bool enable, bool boot, bool runtime, int enable_token, int disable_token, int switchNum, std::string password);

    enum radioStatusCode { STATUS_ENABLED, STATUS_DISABLED, STATUS_NOT_PRESENT, STATUS_UNSUPPORTED, STATUS_UNKNOWN };
    radioStatusCode wirelessRadioStatus(radioNum which, std::ostream &cout=std::cout, u32 defRes2=0);
}

// always should be last thing in header file
#include "smbios/config/abi_suffix.hpp"

#endif  /* SMIINTERFACE_H */
