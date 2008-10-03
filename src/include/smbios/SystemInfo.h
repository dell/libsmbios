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


#ifndef SYSTEMINFO_H
#define SYSTEMINFO_H

// C-header files do not include compat.h
// Instead, they should include auto_link.hpp, which sets up automatic
// library selection on MSVC
#include "smbios/config/auto_link.hpp"

#include <stdio.h>  // for size_t

#define SMBIOSEXCEPTION             0x01
#define SMBIOSPARSEEXCEPTION        0x02
#define SMBIOSITEMDATAOUTOFBOUNDS   0x03
#define SMBIOSITEMSTRINGUNAVAILABLE 0x04
#define SMBIOSPERMISSIONEXCEPTION   0x05
#define SMBIOSPARAMETEREXCEPTION    0x06
#define INTERNALERROR               0x07
#define NOTIMPLEMENTED              0x08

#ifdef __cplusplus
extern "C"
{
#endif

    //////////////////////////////////////////////////////////////////////////
    //
    // Stable API section.
    //
    // All of the functions in this section have a strong guarantee that we
    // will not break API compatibility.
    //
    //////////////////////////////////////////////////////////////////////////

    //! Return a string representing the version of the libsmbios library.
    /** Returns the current version of the SMBIOS library as a string
     */
    const char *SMBIOSGetLibraryVersionString();


    //! Return the Dell System ID Byte or Word
    /** The Dell System ID is a unique number allocated to each Dell System
     * (server, desktop, workstation, or laptop) that uniquely identifies that
     * system within Dell's product line.
     */
    int         SMBIOSGetDellSystemId();

    //! Return a buffer containing the System Name.
    /** Allocates a buffer on behalf of the caller. Caller must use the
     * freeMemory() function call to free this memory when finished.
     */
    const char *SMBIOSGetSystemName();

    //! Return a buffer containing the BIOS version string.
    /** Allocates a buffer on behalf of the caller. Caller must use the
     * freeMemory() function call to free this memory when finished.
     */
    const char *SMBIOSGetBiosVersion();

    //! Returns the 10 character Dell Asset Tag
    /** Allocates a buffer on behalf of the caller. Caller must use the
     * freeMemory() function call to free this memory when finished.
     */
    const char *SMBIOSGetAssetTag();

    //! Returns the 5 or 7 character Dell Service Tag.
    /** Allocates a buffer on behalf of the caller. Caller must use the
     * freeMemory() function call to free this memory when finished.
     */
    const char *SMBIOSGetServiceTag();

    //! Free memory allocated by libsmbios
    /** This function must be called to free memory for any function that
     * allocates memory on behalf of its caller.
     */
    void SMBIOSFreeMemory( const char * );

    /**
     * This function returns the string error message raised by any of the
     * SystemInfo functions
    */
    const char *SMBIOSGetSysInfoErrorString();

    //////////////////////////////////////////////////////////////////////////
    //
    // UNStable API section.
    //
    // API for the following functions has not been frozen yet and is
    // subject to change.
    //
    //////////////////////////////////////////////////////////////////////////

    // Almost stable
    int  SMBIOSHasBootToUp();
    int  SMBIOSGetBootToUp();      //  -- gets BIOS boot-to-UP flag
    void SMBIOSSetBootToUp(int state);  //

    // Almost stable
    int  SMBIOSHasNvramStateBytes();
    int  SMBIOSGetNvramStateBytes(int user);
    void SMBIOSSetNvramStateBytes(int value, int user);

    // Almost stable
    /** Copies chars from inputbuf to outputbuf, changing to scan-codes from ascii.
        buffers must be pre-allocated. */
    void SMBIOSMapAsciiTo_en_US_ScanCode(char *outputScanCodeBuf, const char *inputAsciiBuf, size_t outputBufSize);

    // Almost stable
    /** returns password coding used in SMI calls
        0 == unknown
        1 == scan code
        2 == ascii
    */
    int SMBIOSGetSmiPasswordCoding();

    // Almost stable
    /** Returns 0 if the system is not a Dell, 1 if it is. */
    int  SMBIOSIsDellSystem();

    /** Allocates a buffer on behalf of the caller. Caller must use the
     * freeMemory() function call to free this memory when finished.
     */
    const char *SMBIOSGetVendorName();  //    -- Dell or OEM vendor

    // almost stable, CMOS-only implemented at this time
    //! Set the 5 or 7 character Dell Service Tag.
    /** The Dell Asset Tag is displayed in BIOS and is also contained in SMBIOS.
      * This tag can generally be up to 7 chars long. There are SMI and CMOS
      * methods to set this tag. At present, only CMOS access method is
      * implemented in libsmbios. This works across all Dell hardware that
      * the author is aware of.
      *
      * WARNING! The Dell Service Tag is very closely tied into the Dell
      * support system. This tag should not be changed except under direction
      * from Dell support. */
    int        SMBIOSSetServiceTag(const char *password, const char *newTag, size_t len);

    // almost stable, CMOS-only implemented at this time
    //! Set the Dell Asset Tag
    /** The Dell Asset Tag is displayed in BIOS and is also contained in SMBIOS.
      * This tag can generally be up to 10 chars long. There are SMI and CMOS
      * methods to set this tag. At present, only CMOS access method is
      * implemented in libsmbios. This works across all Dell hardware that
      * the author is aware of.
      *
      * User of the system can use this field to store any user-defined
      * data. Dell BIOS/support/etc does not utilize this value. */
    int        SMBIOSSetAssetTag(const char *password, const char *newTag, size_t len);

#ifdef __cplusplus
}
#endif

#endif  /* SYSTEMINFO_H */
