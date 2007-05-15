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

#include <iostream>

#include "smbios/ISmi.h"
#include "smbios/version.h"
#include "getopts.h"

// always include last if included.
#include "smbios/message.h"

using namespace std;

struct options opts[] =
{
    { 1, "value", "ASCII string to convert to keyboard scan code (en_US keyboard mapping only)", "v", 1 },
    { 255, "version", "Display libsmbios version information", "v", 0 },
    { 0, NULL, NULL, NULL, 0 }
};

//--------------------------------------------------------------------
// Global Data Area
//--------------------------------------------------------------------
// maps ASCII number to a scan code
// sorted by ASCII value
const char ascMap[256] = 
	{
		0x03,
		0x1E,
		0x30,
		0x46,
		0x20,
		0x12,
		0x21,
		0x22,
		0x0E,
		0x0F,
		0x1C,
		0x25,
		0x26,
		0x1C,
		0x31,
		0x18,
		0x19,
		0x10,
		0x13,
		0x1F,
		0x14,
		0x16,
		0x2F,
		0x11,
		0x2D,
		0x15,
		0x2C,
		0x1A,
		0x2B,
		0x1B,
		0x07,
		0x0C,
		0x39,
		0x02,
		0x28,
		0x04,
		0x05,
		0x06,
		0x08,
		0x28,
		0x0A,
		0x0B,
		0x09,
		0x0D,
		0x33,
		0x0C,
		0x34,
		0x35,
		0x0B,
		0x02,
		0x03,
		0x04,
		0x05,
		0x06,
		0x07,
		0x08,
		0x09,
		0x0A,
		0x27,
		0x27,
		0x33,
		0x0D,
		0x34,
		0x35,
		0x03,
		0x1E,
		0x30,
		0x2E,
		0x20,
		0x12,
		0x21,
		0x22,
		0x23,
		0x17,
		0x24,
		0x25,
		0x26,
		0x32,
		0x31,
		0x18,
		0x19,
		0x10,
		0x13,
		0x1F,
		0x14,
		0x16,
		0x2F,
		0x11,
		0x2D,
		0x15,
		0x2C,
		0x1A,
		0x2B,
		0x1B,
		0x07,
		0x0C,
		0x29,
		0x1E,
		0x30,
		0x2E,
		0x20,
		0x12,
		0x21,
		0x22,
		0x23,
		0x17,
		0x24,
		0x25,
		0x26,
		0x32,
		0x31,
		0x18,
		0x19,
		0x10,
		0x13,
		0x1F,
		0x14,
		0x16,
		0x2F,
		0x11,
		0x2D,
		0x15,
		0x2C,
		0x1A,
		0x2B,
		0x1B,
		0x29,
		0x0E,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00,
		0x00
	};

//---------------------------------------------------------------------
// KBDMapASCIIToScanCode - Maps a string of ASCII codes to scan code
// bytes
//---------------------------------------------------------------------
void mapAsciiToScanCode(char *outputScanCodeBuf, const char *inputAsciiBuf, u32 outputBufSize)
{
    memset(outputScanCodeBuf, 0, outputBufSize);
	for (size_t i = 0; i<outputBufSize && i<strlen(inputAsciiBuf); i++)
	{
        outputScanCodeBuf[i] = ascMap[static_cast<size_t>(inputAsciiBuf[i])];
	}
} 



//---------------------------------------------------------------------
//  Code Ends
//---------------------------------------------------------------------



int
main (int argc, char **argv)
{
    int retval = 0;
    try
    {
        string asciiStr("");

        int c;
        char *args = 0;
        while ( (c=getopts(argc, argv, opts, &args)) != 0 )
        {
            switch(c)
            {
            case 1:
                asciiStr = args;
                break;
            case 255:
                cout << "Libsmbios version:    " << LIBSMBIOS_RELEASE_VERSION << endl;
                exit(0);
                break;
            default:
                break;
            }
            free(args);
        }

        cout << "mapping ascii for string: " << asciiStr << endl;
        char *outputBuf = new char[strlen(asciiStr.c_str()) + 1];
        mapAsciiToScanCode(outputBuf, asciiStr.c_str(), strlen(asciiStr.c_str()) + 1);
        
        cout << "Keyboard Scan Codes: " << oct;
        for(size_t i=0; i<strlen(asciiStr.c_str()); i++)
        {
            cout << "\\0" << static_cast<int>(outputBuf[i]);
        }
        cout << endl;

    }
    catch ( ... )
    {
        cerr << endl << "An Unknown Error occurred. Aborting." << endl;
        retval = 2;
    }

    return retval;
}
