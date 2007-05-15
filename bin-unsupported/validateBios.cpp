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
#include <sstream>
#include <memory>
#include <vector>

#include "smbios/IMemory.h"
#include "smbios/ISmbiosXml.h"
#include "smbios/IToken.h"
#include "smbios/ICmosRW.h"
#include "smbios/IObserver.h"
#include "smbios/ISmbios.h"
#include "smbios/SmbiosDefs.h"
#include "smbios/SmbiosLowLevel.h"

using namespace std;
using namespace smbios;
using namespace smbiosLowlevel;

class myException: public exception {
    public:
        myException(const std::string &initMsg): msg(initMsg) {};
        virtual ~myException() throw() {};
        virtual const char *what() const throw() { return msg.c_str(); };
    private:
        std::string msg;
};

// exception list is global
vector<exception> excList;

typedef void (*f_ptr)();

int reservedUnknown[2]={0x00,// Unknown
    0x02 // Reserved
};

struct validator
{
    void (*f_ptr)();
};

int countItem(smbios::ISmbiosTable *table, int type)
{
    int tableEntriesCounted = 0;
    for( smbios::ISmbiosTable::iterator item = (*table)[type] ; item != table->end(); ++item)
    {
        tableEntriesCounted++;
    }
    return tableEntriesCounted;
}

void reportItemException(std::string specDesc, smbios::ISmbiosTable::iterator item, int foundValues[], int noVal)
{
        ostringstream oss;
        oss << "Violation of " << specDesc << endl
            << "Structure Type: " << (int)item->getType() << endl
            << "Structure Handle: " << (int)item->getHandle() << endl;
        if(noVal == 0)
        {
            oss << " Found null. " << endl;
        }
        else
        {
            oss << " Found values: " ;
        }
        for(int i = 0; i < noVal;i++)
        {
            oss << foundValues[i] << " " ;
        }
        oss << endl;
        excList.push_back(myException(oss.str()));

}

void wrongNumberOfEntriesException(smbios::ISmbiosTable *table, int type,int entries,std::string specDesc)
{
        ostringstream oss;
        if(entries != 0)
        {
            oss << "Violation of " << specDesc << endl
                << " Number of structures  are " << entries << endl;
        }
        else
        {
            oss << "Violation of " << specDesc << endl
                << " Found 0 structures " << endl;
        }
        // Looping for getting the handle of structure
        for( smbios::ISmbiosTable::iterator item = (*table)[type] ; item != table->end(); ++item)
        {
            oss << "Structure Type: " << item->getType() << endl
                << "Structure Handle: " << item->getHandle() << endl << endl;
        }
}

void checkStructLength(smbios::ISmbiosTable *table, int type, int length, std::string specDesc)
{
    for( smbios::ISmbiosTable::iterator item = (*table)[type] ; item != table->end(); ++item)
    {
        if((int)item->getLength() < length)
        {
            int values[1]= { (int)item->getLength() };
            int index = 1;
            reportItemException(specDesc,item,values,index);
        }
    }
}

void checkStringNotNull(smbios::ISmbiosTable *table, int type, unsigned int offset, std::string specDesc)
{
    for( smbios::ISmbiosTable::iterator item = (*table)[type] ; item != table->end(); ++item)
    {
        if((item->getU8(offset) == 0) && (item->getString(offset) == ""))
        {
            // No values to report, so value array and index are 0
            reportItemException(specDesc,item,0,0); 
        }
    }
}

void checkByteValues(smbios::ISmbiosTable *table, int type, unsigned int offset, int errorValues[], int noVal, std::string specDesc, int fieldLen)
{
    for( smbios::ISmbiosTable::iterator item = (*table)[type] ; item != table->end(); ++item)
    {
        bool errorValue=false;
        for(int i = 0; i < noVal; i++)
        {
            switch(fieldLen)
            {
                case ISmbiosItem::FIELD_LEN_BYTE:
                    if((int)item->getU8(offset) == (int)errorValues[i])
                    {
                        errorValue=true;
                    }
                    break;
                case ISmbiosItem::FIELD_LEN_WORD:
                    if((int)item->getU16(offset)== (int)errorValues[i])
                    {
                        errorValue=true;
                    }
                    break;
                case ISmbiosItem::FIELD_LEN_DWORD:
                    if((int)item->getU32(offset)== (int)errorValues[i])
                    {
                        errorValue=true;
                    }
                    break;
                case ISmbiosItem::FIELD_LEN_QWORD:
                    if((int)item->getU64(offset)== (int)errorValues[i])
                    {
                        errorValue=true;
                    }
                    break;
                default:
                    cout << " Error in calling function ";
            }
            if(errorValue)
            {
                int values[1] = { (int)errorValues[i] };
                int index = 1;
                reportItemException(specDesc,item,values,index); 
                break;
            }
        }
    }
}


void checkMemoryHandles(smbios::ISmbiosTable *table, int currentType, int refType, int offset, std::string specDesc)
{
    for( smbios::ISmbiosTable::iterator currentItem = (*table)[currentType] ;
            currentItem != table->end(); ++currentItem)
    {
        bool references=false;
        for( smbios::ISmbiosTable::iterator  refItem = (*table)[refType] ; refItem != table->end(); ++refItem)
        {
            if(currentItem->getU16(offset) == refItem->getHandle())
            {
                references=true;
                break;
            }
        }
        if(!references)
        {
            int errorValues[1] = { currentItem->getU16(offset)};
            int index = 1;
            reportItemException(specDesc,currentItem,errorValues,index);
        }
    }
}

void checkAddresses(smbios::ISmbiosTable *table,int type, int startOffset,int endOffset, std::string specDesc)
{
    for( smbios::ISmbiosTable::iterator item = (*table)[type] ; item != table->end(); ++item)
    {
        if(item->getU32(startOffset) > item->getU32(endOffset))
        {
            int errorValues[2]={ item->getU32(startOffset),
                                 item->getU32(endOffset)
            };
            int index=2;
            reportItemException(specDesc,item,errorValues,index);
        }
    }
}

void reportOverallViolation(smbios::ISmbiosTable::iterator item, std::string specDesc)
{
    ostringstream oss;
    oss << "Violation of " << specDesc << endl
        << "Item Type: " << (int)item->getType() << endl
        << "Item Handle: " << (int)item->getHandle() << endl
        << "Item Length: " << (int)item->getLength() << endl
        << "===============================" << endl
        << *item << endl;
    excList.push_back(myException(oss.str()));
}

void validateTableEntryPoint()
{
    
    // This validation is checked to get the table. If this validation fails
    // then the table is not get created. So, no need to check the validation as
    // this is done in the file SmbiosTable.cpp.
    // The getBiosTableHeader() func checks in the memory for the "_SM_" string
    // in the address range 0xf000 to 0xffff and returns the header.
    cout << " // 1. The table Anchor String is \"_SM_\" is present in the address range 0xf0000 to 0xffff on a 16-byte boundary. " << endl;

    // All these validation are checked in the SmbiosTable.cpp file. If
    // validation 1 is true, then all thease validation are checked to return a
    // valid table entry point. As they are already checked, no need to check
    // here.
    cout << " // 2. Table entry-point verification. " << endl;
    cout << " // 2.1 The Entry Point field value is at least 0x1f. " << endl;
    cout << " // 2.2 The entry-point checksum evaluates to 0. " << endl;
    cout << " // 2.3 The SMBIOS Version(Major.Minor) is at least 2.3. " << endl;
    cout << " // 2.4 The intermediate Anchor String is \"_DMI_\". " << endl;
    cout << " // 2.5 The intermediate checksum evaluates to 0. " << endl << endl;
}


void validateOverall()
{
    smbios::ISmbiosTable *table = smbios::SmbiosFactory::getFactory()->getSingleton();
    smbios_table_entry_point tableEPS = table->getTableEPS();

    cout << "// 3 Validate Overall SMBIOS table." << endl;
    u16 size=0;
    int tableEntriesCounted = 0;
    // For keeping the count of each handle in the table.
    // This helps in determining the the hadles are repeated or not.
    // The map is defined as <handle, count>
    std::map<int, int> handleMap;

    std::string specDesc = "  3.1 The structure-table's linked-list is traversable within the length and structure-count bounds specified by the entry-point structure. ";
    cout << "// " << specDesc << endl;
    for( smbios::ISmbiosTable::iterator item = (*table)[-1] ; item != table->end(); ++item)
    {
        handleMap[item->getHandle()]++; // keep count of each handle
        tableEntriesCounted++;
        size=size+item->getLength();
        if((size > tableEPS.table_length) || (tableEntriesCounted > tableEPS.table_num_structs))
        {
            reportOverallViolation(item, specDesc);
        }
    }

    specDesc = " 3.2 The overall size of the structure table is less than or equal to the Structure Table Length specified by the entry-point structure. ";
    cout << "// " << specDesc << endl;
    if(size > tableEPS.table_length)
    {
        ostringstream oss;
        oss << "Violation of " << specDesc << endl
            << "overall size: " << (int)size << endl
            << "Table Length of entry-point: " << (int)tableEPS.table_length << endl
            << "===============================" << endl;
        excList.push_back(myException(oss.str()));
    }

    specDesc = " 3.3 Each structure's length must be at least 4 (the size of a structure header.). ";
    cout << "// " << specDesc << endl;
    int lastItem = 0;
    ostringstream violation_3_5;
    for( smbios::ISmbiosTable::iterator item = (*table)[-1] ; item != table->end(); ++item)
    {
        lastItem = item->getType();

        if( item->getLength() < 4 )
        {
            reportOverallViolation(item, specDesc);
        }

        // set up here in case of error.
        violation_3_5 << "Violation of 3.5" << endl
                      << "Item Type: " << (int)item->getType() << endl
                      << "Item Handle: " << (int)item->getHandle() << endl
                      << "Item Length: " << (int)item->getLength() << endl
                      << "===============================" << endl
                      << *item << endl;
    }

    specDesc = " 3.4 No structure handle is repeated. ";
    cout << "// " << specDesc << endl;
    // handleMap is defined as <handle(int), count(int)>. 
    for(map<int,int>::iterator iter=handleMap.begin(); iter!=handleMap.end(); ++iter)
    {
        if(handleMap[iter->first] > 1)
        {
            ostringstream oss;
            oss << "Violation of " << specDesc << endl
                << " The repeated handle is:  " << iter->first << endl
                << " The handle repeated: " << iter->second << " times " << endl 
                << "===============================" << endl;
            excList.push_back(myException(oss.str()));
        }
    }

    cout << "// 3.5 The last structure is the end-of-table (0x7F)" << endl;
    if( lastItem != 0x7F )
    {
        excList.push_back(myException(violation_3_5.str()));
    }
    
    specDesc = " 3.6 The number of structures found within the table equals the Number of SMBIOS structures field present in the entry-point. ";
    cout << "// " << specDesc << endl;
    if(tableEntriesCounted != tableEPS.table_num_structs)
    {
        ostringstream oss;
        oss << "Violation of " << specDesc << endl
            << " The table_num_structs:  " << tableEPS.table_num_structs << endl
            << " Total structures found: " << tableEntriesCounted << endl 
            << "===============================" << endl;
        excList.push_back(myException(oss.str()));
    }
    
    specDesc = " 3.7 The maximum structure size(formatted area plus its string pool) is less than or equal to the Maximum Structure Size specified by the entry-point.";
    cout << "// " << specDesc << endl << endl;
    for( smbios::ISmbiosTable::iterator item = (*table)[-1] ; item != table->end(); ++item)
    {
        if(item->getBufferSize() > tableEPS.max_struct_size)
        {
            reportOverallViolation(item, specDesc);
        }
    }
    
}

void validateBiosBlock()
{
    cout << "// 4.1   Bios Information (type 0)" << endl;
    smbios::ISmbiosTable *myTablePtr = smbios::SmbiosFactory::getFactory()->getSingleton();
    u8 offset = 0x00;
    std::string specDesc = "4.1.1 One and only one structure of BIOS Information type is present.";
    
    cout << "// " << specDesc << endl;
    int entries = countItem(myTablePtr, BIOS_Information );
    if( entries != 1 )
    {
        wrongNumberOfEntriesException(myTablePtr,BIOS_Information,entries,specDesc);
    }

    specDesc = " 4.1.2 The structure Length is at least 0x13. ";
    cout << "// "<< specDesc << endl;
    checkStructLength(myTablePtr, BIOS_Information,0x13,specDesc);
     
    specDesc = " 4.1.3 BIOS Version string is present and non-null. ";
    cout << "// "<< specDesc << endl;
    offset = 0x08;
    checkStringNotNull(myTablePtr, BIOS_Information,offset,specDesc);

    specDesc = " 4.1.4 BIOS Release Date is present, non-null and includes a 4-digit year.";
    cout << "// " << specDesc << endl;
    for( smbios::ISmbiosTable::iterator item = (*myTablePtr)[BIOS_Information] ; item != myTablePtr->end(); ++item)
    {
        std::string version = item->getString(offset);
        std::string yearStr(version.substr(version.length()-5,5));
        if((item->getU8(offset)==0) && (version=="") && (yearStr.find('/')!=0))
        {
            ostringstream oss;
            oss << "Violation of " << specDesc << endl
                << " The version string:  " << version << endl;
            excList.push_back(myException(oss.str()));
        }
    }
    
    specDesc = "4.1.5 BIOS Characteristics bits 3:0 are all 0, at least one of bits 31:4 is set to 1. ";
    cout << "// " << specDesc << endl << endl;
    smbios::ISmbiosTable::iterator item = (*myTablePtr)[BIOS_Information]; 
    bool bitSet = false;
    // Going through each bit 31:4
    offset = 0x0e;
    for(int i = 4;i <= 31; i++)
    {
        if(item->getBitfield(offset,4,i,i) == 1)
        {
            bitSet = true;
        }
    }
    offset = 0x11;
    if((item->getBitfield(offset,1,0,3) != 0) && (!bitSet))
    {
        int values[2] = { item->getBitfield(offset,1,0,3),
                          item->getBitfield(0x1e,4,4,31)
        };
        int index = 2;
        reportItemException(specDesc,item,values,index);
    }
}

void validateSystemInformation()
{
    cout << "// 4.2 Validate System Information (type 1)" << endl;
    smbios::ISmbiosTable *myTablePtr = smbios::SmbiosFactory::getFactory()->getSingleton();
    u8 offset = 0x00;

    std::string specDesc = " 4.2.1 One and only one structure of System Information  is present.";
    cout << "// " << specDesc << endl;
    int entries = countItem(myTablePtr,System_Information);
    if(entries != 1)
    {
        wrongNumberOfEntriesException(myTablePtr,System_Information,entries,specDesc);
    }

    specDesc = " 4.2.2 The system Information structure length is atleast 0x19. ";
    cout << "// " << specDesc << endl;
    checkStructLength(myTablePtr,System_Information,0x19, specDesc);
    
     specDesc = " 4.2.3 Systen Information manufacturer string is present and non-null. ";
    cout << "// " << specDesc << endl;
    offset = 0x04;
    checkStringNotNull(myTablePtr,System_Information,offset, specDesc);

    specDesc = " 4.2.4 System Information Product Name string is present and non-null. ";
    cout << "// " << specDesc << endl;
    offset = 0x05;
    checkStringNotNull(myTablePtr,System_Information,offset,specDesc);
     
     specDesc = " 4.2.5 System Information UUID field is neither 00000000 00000000 nor FFFFFFFF FFFFFFFF. ";
    cout << "// " << specDesc << endl;
    smbios::ISmbiosTable::iterator item = (*myTablePtr)[System_Information]; 
    u64 uuid0 = 0x0ULL;
    u64 uuidf = -1ULL;  // (equivalent to 0xffff ffff ffff ffff)
    if(((item->getU64(0x08) == uuid0) && (item->getU64(0x0e) == uuid0))
        ||((item->getU64(0x08) == uuidf) && (item->getU64(0x0e) == uuidf)))
    {
        int values[2] = { (int)item->getU64(0x08),
                          (int)item->getU64(0x0e)
        };
        int index = 2;
        reportItemException(specDesc,item,values,index);
    }
    
    specDesc = " 4.2.6 System Information Wake-up Type field is neither 0x00(Reserved) nor 0x02 (Unknown). " ;
    cout << "// " << specDesc << endl << endl;
    offset = 0x18;
    int fieldLen=1;
    checkByteValues(myTablePtr, System_Information,offset,reservedUnknown, 2, specDesc,fieldLen);
}


void validateSystemEnclosure()
{
    cout << "// 4.3 Validate System Enclosure  (type 3)" << endl;
    smbios::ISmbiosTable *myTablePtr = smbios::SmbiosFactory::getFactory()->getSingleton();
    u8 offset = 0x00;
    std::string specDesc =" 4.3.1 One or more structures of System Enclosure is present.  ";

    cout << "// " << specDesc << endl;
    int entries = countItem(myTablePtr,System_Enclosure_or_Chassis);
    if(entries == 0)
    {
        wrongNumberOfEntriesException(myTablePtr,System_Enclosure_or_Chassis,entries,specDesc);
    }

    specDesc = " 4.3.2 The System Enclosure structure length is atleast 0x0d. ";
    cout << "// " << specDesc << endl;
    checkStructLength(myTablePtr,System_Enclosure_or_Chassis,0x0d,specDesc);

    specDesc = " 4.3.3 System Enclosure manufacture string is present and non-null in each structure ";
    cout << "// " << specDesc << endl;
    offset = 0x04;
    checkStringNotNull(myTablePtr,System_Enclosure_or_Chassis, offset, specDesc);
    
    specDesc = " 4.3.4 System Enclosure type field is neither 0x00(Reserved) nor 0x02 (Unknown)  ";
    cout << "// " << specDesc << endl << endl;
    offset = 0x05;
    int fieldLen=1;
    checkByteValues(myTablePtr,System_Enclosure_or_Chassis,offset,reservedUnknown,2,specDesc,fieldLen);
}


void validateProcessorInformation()
{
    cout << "// 4.4 Validate Processor Information (type 4)" << endl;
    smbios::ISmbiosTable *myTablePtr = smbios::SmbiosFactory::getFactory()->getSingleton();
    u8 offset = 0x00;
    std::string specDesc = " 4.4.1 The number of structures defines the maximum number of processors supported by the system. at least one structure with a Processor Type field of \"Central Processor\" must be present. "; 
    cout << "// " << specDesc << endl;

    int entries = countItem(myTablePtr,Processor_Information);
    if(entries == 0)
    {
        wrongNumberOfEntriesException(myTablePtr,Processor_Information,entries,specDesc);
    }

    specDesc = " 4.4.2 Each structure's length of Processor Information is at least 0x20. ";
    cout << "// " << specDesc << endl;
    checkStructLength(myTablePtr,Processor_Information,0x20,specDesc);

    specDesc = " 4.4.3 Socket Designation string of Processor Information is present and non-null. ";
    cout << "// " << specDesc << endl;
    offset = 0x04;
    checkStringNotNull(myTablePtr, Processor_Information,offset,specDesc);

    specDesc =" 4.4.4 Processor Type field is neither 0x00 (Reserved) nor 0x02 (Unknown). ";
    cout << "// " << specDesc << endl;
    offset = 0x05;
    int fieldLen=1;
    checkByteValues(myTablePtr,Processor_Information,offset,reservedUnknown,2,specDesc,fieldLen);

    specDesc = " NOTE: Fields preceded by (*) are only checked if the CPU Socket Populated sub-field of the Status field is set to \"CPU Populated\". ";
    cout << "// " << specDesc << endl;

    specDesc =" 4.4.5 (*) Processor family field is neither 0x00 (Reserved) nor 0x02 (Unknown). ";
    cout << "// " << specDesc << endl;
    offset = 0x06;
    for( smbios::ISmbiosTable::iterator item = (*myTablePtr)[Processor_Information] ; item != myTablePtr->end(); ++item)
    {
        // checking for CPU Populated is set or not
        if(item->getBitfield(0x18,1,6,6) == 1) 
        {
            fieldLen=1;
            checkByteValues(myTablePtr,Processor_Information,offset,reservedUnknown,2,specDesc,fieldLen);
        }
    }

    specDesc =" 4.4.6 (*) Processor Manufacturer string is present and non-null. ";
    cout << "// " << specDesc << endl;
    offset = 0x07;
    for( smbios::ISmbiosTable::iterator item = (*myTablePtr)[Processor_Information] ; item != myTablePtr->end(); ++item)
    {
        // checking for CPU Populated is set or not
        if(item->getBitfield(0x18,1,6,6) == 1)
        {
            checkStringNotNull(myTablePtr,Processor_Information, offset, specDesc);
        }
    }

    specDesc =" 4.4.8 (*) CPU status sub-field of the Status field is not 0(Unknown). ";
    cout << "// " << specDesc << endl;
    offset = 0x18;
    for( smbios::ISmbiosTable::iterator item = (*myTablePtr)[Processor_Information] ; item != myTablePtr->end(); ++item)
    {
        // checking for CPU Populated is set or not
        if(item->getBitfield(0x18,1,6,6) == 1)
        {
            if(item->getBitfield(offset,1,0,2)==0)
            {
                 reportItemException(specDesc,item,0,0);// no values to report
            }
        }
    }

    specDesc =" 4.4.7 Max speed field is non-0 ";
    cout << "// " << specDesc << endl;
    offset = 0x14;
    for( smbios::ISmbiosTable::iterator item = (*myTablePtr)[Processor_Information] ; item != myTablePtr->end(); ++item)
    {
        if((int)item->getU16(offset)==0)
        {
            reportItemException(specDesc,item,0,0);// no values to report
        }
    }

    specDesc = " 4.4.9 Processor Upgrade field is neither 0x00 (Reserved) nor 0x02 (Unknown). ";
    cout << "// " << specDesc << endl;
    offset = 0x19;
    fieldLen=1;
    checkByteValues(myTablePtr,Processor_Information,offset,reservedUnknown,2,specDesc,fieldLen);

    specDesc =" 4.4.10 Lx(x=1,2,3) Cache Handle fields, if not set to 0xFFFF, reference Cache Information(Type 7) structures. ";
    cout << "// " << specDesc << endl << endl;
    offset = 0x1a;
    // As per spec, get the number of Cache Information(Type 7) structures
    int noCache = countItem(myTablePtr,Cache_Information);
    if(noCache == 0)
    {
        for( smbios::ISmbiosTable::iterator item = (*myTablePtr)[Processor_Information] ; item != myTablePtr->end(); ++item)
        {
            if((item->getU16(offset) != 0xffff)
                &&(item->getU16(offset+0x02) != 0xffff)
                &&(item->getU16(offset+0x04) != 0xffff))
            {
                int foundValues[3] = { (int)item->getU16(offset),
                    (int)item->getU16(offset+0x02),
                    (int)item->getU16(offset+0x04)
                };
                int index = 3;
                reportItemException(specDesc,item,foundValues,index);
            }
        }
        
    }
}


void validateCacheInformatin()
{
    cout << "// 4.5  Validate Cache Information (type 7)." << endl ;
    smbios::ISmbiosTable *myTablePtr = smbios::SmbiosFactory::getFactory()->getSingleton();
    u8 offset = 0x00;
    std::string specDesc =" 4.5.1 One structure is present for each external-to-the-processor cache ";
    cout << "// " << specDesc << endl;
    int noProcessors = countItem(myTablePtr,Processor_Information);
    int entries = countItem(myTablePtr,Cache_Information);
    if(entries != noProcessors)
    {
        ostringstream oss;
        oss << "Violation of " << specDesc << endl
            << " Number of structures  are " << entries << endl
            << " Number of noProcessors  are " << noProcessors << endl;
        for( smbios::ISmbiosTable::iterator item = (*myTablePtr)[Cache_Information] ; item != myTablePtr->end(); ++item)
        {
            oss << "Structure Type: " << item->getType() << endl
                << "Structure Handle: " << item->getHandle() << endl << endl;
         }
    }
    specDesc = " 4.5.2 Each structure's Length is at least 13h. ";
    cout << "// " << specDesc << endl;
    checkStructLength(myTablePtr,Cache_Information,0x13,specDesc);
   
    specDesc = " 4.5.3 Socket Designation string is present and non-null if the cache is external to the processor. ";
    cout << "// " << specDesc << endl;
    offset = 0x04;
    for( smbios::ISmbiosTable::iterator item = (*myTablePtr)[Cache_Information] ; item != myTablePtr->end(); ++item)
    {
        // Checking for the cache is external to the processor or not
        if(item->getBitfield(0x05,2,5,6) == 1)
        {
            checkStringNotNull(myTablePtr, Cache_Information, offset, specDesc);
        }
    }
    
    specDesc = " 4.5.4  Operational Mode and Location sub-fields of the Cache Configuration field are not 11b." ;
    cout << "// " << specDesc << endl << endl;
    offset = 0x05;
    for( smbios::ISmbiosTable::iterator item = (*myTablePtr)[Cache_Information] ; item != myTablePtr->end(); ++item)
    {
        if(((int)item->getBitfield(offset,2,8,9) == 3)
          ||((int)item->getBitfield(offset,2,5,6) == 3))
        {
                int foundValues[2] = { (int)item->getBitfield(offset,2,8,9),
                    (int)item->getBitfield(offset,2,5,6)
                };
                int index = 2;
                reportItemException(specDesc,item,foundValues,index);
        }
    }
}

void validateSystemSlots()
{
    cout << "// 4.6 Validate System Slots (Type 9 - System_Slots). " << endl;
    smbios::ISmbiosTable *myTablePtr = smbios::SmbiosFactory::getFactory()->getSingleton();
    u8 offset = 0x00;

    // Can not check this validation
    std::string specDesc =" 4.6.1 One structure is present for each upgradable system slot. ";
    cout << "** " << specDesc << endl;
    cout << " The 4.6.1 validation can not be checked by the automated program, as the upgradable system slots can be determined by manually. " << endl;

    specDesc = " 4.6.2 Each structure's Length is at least 0dh. ";
    cout << "// " << specDesc << endl;
    checkStructLength(myTablePtr,System_Slots,0x0d,specDesc);

    specDesc = " 4.6.3 Slot Designation string is present and non-null. ";
    cout << "// " << specDesc << endl;
    offset = 0x04;
    checkStringNotNull(myTablePtr,System_Slots,offset,specDesc);

    specDesc = " 4.6.4 Slot Type is neither 00h(Reserved) or 02h (Unknown). ";
    cout << "// " << specDesc << endl;
    offset = 0x05;
    int fieldLen=1;
    checkByteValues(myTablePtr,System_Slots,offset,reservedUnknown,2,specDesc,fieldLen);

    specDesc = " 4.6.5 Slot Data Bus Width is neither 00h(Reserved) or 02h (Unknown). ";
    cout << "// " << specDesc << endl;
    offset = 0x06;
    checkByteValues(myTablePtr,System_Slots,offset,reservedUnknown,2,specDesc,fieldLen);
    
    ostringstream ost;
    ost << " 4.6.6 Current usage is not set to 00h(Reserved). If the Slot Type "
        << "provides device presence-detect capabilities, e.g PCI or AGP,"
        << " Current Usage is not set to 02h(Unknown). ";
    specDesc = ost.str();
    cout << "// " << specDesc << endl;
    offset = 0x07;
    checkByteValues(myTablePtr,System_Slots,offset,reservedUnknown,2,specDesc,fieldLen);

    specDesc = " 4.6.7 Slot ID is set to a meaningful value. ";
    cout << "// " << specDesc << endl;
    // The Slot ID field has meaning only for the slots given in following arry
    // From this array we can find out slot id has meaning or not
    int noTypes = 6;
    u8 slotTypes[] = {0x04, // MCA
                     0x05, // EISA
                     0x06, // PCI
                     0x0f, // AGP
                     0x12, // PCI-X
                     0x07 // PCMCIA
    };
    offset = 0x05; // Slot Type
    // for each system slot structure
    for( smbios::ISmbiosTable::iterator item = (*myTablePtr)[System_Slots] ; item != myTablePtr->end(); ++item)
    {
        bool meaningfulSlotID = false;
        for(int i = 0; i < noTypes; i++)
        {
            if(item->getU8(offset) == slotTypes[i])
            {
                meaningfulSlotID = true;
            }
        }
        if(!meaningfulSlotID)
        {
            specDesc = " 4.6.7 Slot ID is set to a meaningful value. The slot ID's should be one of the give values MCA(0x04),EISA(0x05),PCI(0x06),AGP(0x0f),PCI-X(0x12),PCMCIA(0x07)";
            int foundValues[1] = { item->getU8(offset) };
            int index = 1;
            reportItemException(specDesc,item,foundValues,index);
        }
    }

    specDesc = " 4.6.8 Slot Characteristics 1, bit 0, is not set to 1. ";
    cout << "// " << specDesc << endl << endl;
    offset = 0x0b;
    for( smbios::ISmbiosTable::iterator item = (*myTablePtr)[System_Slots] ; item != myTablePtr->end(); ++item)
    {
        if(item->getBitfield(offset,1,0,0) == 1)
        {
                int foundValues[1] = { item->getBitfield(offset,1,0,0) };
                int index=1;
                reportItemException(specDesc,item,foundValues,index);
        }
    }
}

void validatePhysicalMemoryArray()
{
    cout << "// 4.7 Validate Physical Memory Array (Type 16 - Physical_Memory_Array). " << endl;
    smbios::ISmbiosTable *myTablePtr = smbios::SmbiosFactory::getFactory()->getSingleton();
    u8 offset = 0x00;
    int entries = 0;
    std::string specDesc =" 4.7.1 Atleast one structure is present with \"Use\" set to 03h(System Memory). ";
    cout << "// " << specDesc << endl; 
    offset = 0x05; // Use feild
    for( smbios::ISmbiosTable::iterator item = (*myTablePtr)[Physical_Memory_Array] ; item != myTablePtr->end(); ++item)
    {
        if(item->getU8(offset) == 0x03)
        {
            entries++;
        }
    }
    if(entries == 0)
    {
        wrongNumberOfEntriesException(myTablePtr,Physical_Memory_Array,entries,specDesc);
    }

    specDesc =" 4.7.2 Each structure's length is at least 0fh. ";
    cout << "// " << specDesc << endl; 
    checkStructLength(myTablePtr,Physical_Memory_Array,0x0f,specDesc);
    
    specDesc =" 4.7.3 Location is neither 00h(Reserved) nor 02h(Unknown). ";
    cout << "// " << specDesc << endl; 
    offset = 0x04;
    int fieldLen=1;
    checkByteValues(myTablePtr,Physical_Memory_Array ,offset,reservedUnknown, 2, specDesc,fieldLen);

    specDesc =" 4.7.4 Use is neither 00h(Reserved) nor 02h(Unknown). ";
    cout << "// " << specDesc << endl; 
    offset = 0x05;
    checkByteValues(myTablePtr,Physical_Memory_Array ,offset,reservedUnknown, 2, specDesc,fieldLen);
    
    specDesc =" 4.7.5 Memory Error Correction is neither 00h(Reserved) nor 02h(Unknown). ";
    cout << "// " << specDesc << endl; 
    offset = 0x06;
    checkByteValues(myTablePtr,Physical_Memory_Array ,offset,reservedUnknown, 2, specDesc,fieldLen);
    
    specDesc =" 4.7.6  Maximum Capacity is not set to 80000000h. ";
    cout << "// " << specDesc << endl; 
    offset = 0x07;
    fieldLen = 4;
    int errorValues[] = {0x80000000};
    checkByteValues(myTablePtr,Physical_Memory_Array ,offset,errorValues,1, specDesc,fieldLen);
    
    specDesc =" 4.7.7  Number of Memory Devices is not 0 and equals the number of Memory Devices (Type 17) structures that reference the handle of the Physical Memory Array structure. ";
    cout << "// " << specDesc << endl << endl; 
    int noDevices = 0;
    offset = 0x0d;
    int handle = 0x04; // offset of Physical Memory Array Handle in Memory Device structure
    for( smbios::ISmbiosTable::iterator array = (*myTablePtr)[Physical_Memory_Array] ; array != myTablePtr->end(); ++array)
    {
        noDevices=0;
        for( smbios::ISmbiosTable::iterator device= (*myTablePtr)[Memory_Device] ; device != myTablePtr->end(); ++device)
        {
            if(device->getU16(handle) == array->getHandle())
            {
                noDevices++;
            }
        }
        if((array->getU8(offset) == 0)||((int)array->getU8(offset) != noDevices))
        {
            int foundValues[2] = { array->getU8(offset),
                noDevices
            };
            int index=2;
            reportItemException(specDesc,array,foundValues,index);
        }
    }

}

void validateMemoryDevice()
{
    cout << "// 4.8 Validate Memory Device (Type 17 - Memory_Device). " << endl;
    smbios::ISmbiosTable *myTablePtr = smbios::SmbiosFactory::getFactory()->getSingleton();
    u8 offset = 0x00;

    ostringstream ost;
    ost << " 4.8.1 For each Physical Array, there must be" 
        << " \"Number of Memory Devices\" Memory Device structures thati map "
        << " back(via Handle) to the reference memory array. One structure is"
        << " required for each socketed system-memory device, whether or not"
        << " the socket is currently populated. If the system includes "
        << " soldered-on system-memory, one additional structure is required"
        << " to identify that memory device. ";
    std::string specDesc = ost.str();
    cout << "// " << specDesc << endl;
    // This validation is same as 4.7.7 of Physical Memory Array validation

    specDesc = " 4.8.2 Each structure's length is atleast 15h. ";
    cout << "// " << specDesc << endl;
    checkStructLength(myTablePtr,Memory_Device,0x0f,specDesc);

    specDesc = " 4.8.3 Memory Array Handle references a Physical Memory Array(Type 16) structure. ";
    cout << "// " << specDesc << endl;
    offset = 0x04; 
    checkMemoryHandles(myTablePtr,Memory_Device, Physical_Memory_Array, offset, specDesc);

    int errorValues[1] = {0xFFFF};
    specDesc =" 4.8.4 Total width is not FFFFh(Unknown) if the memory device is installed(Size is not 0). ";
    cout << "// " << specDesc << endl;
    offset = 0x08;
    int fieldLen = 2;
    for( smbios::ISmbiosTable::iterator item = (*myTablePtr)[Memory_Device] ; item != myTablePtr->end(); ++item)
    {
        // 0x0c - size offset
        if(item->getU16(0x0c) != 0)
        {
            checkByteValues(myTablePtr,Memory_Device ,offset,errorValues, 1, specDesc,fieldLen);
        }
    }

    specDesc =" 4.8.5 Data Width is not FFFFh(Unknown). ";
    cout << "// " << specDesc << endl;
    offset = 0x0a;
    checkByteValues(myTablePtr,Memory_Device ,offset,errorValues, 1, specDesc,fieldLen);
    
    specDesc =" 4.8.6 Size is not FFFFh(Unknown). ";
    cout << "// " << specDesc << endl;
    offset = 0x0c;
    checkByteValues(myTablePtr,Memory_Device ,offset,errorValues, 1, specDesc,fieldLen);

    specDesc =" 4.8.7 Form Factor is not 00h(Reserved) or 02h(Unknown). ";
    cout << "// " << specDesc << endl;
    offset = 0x0e;
    fieldLen = 1;
    checkByteValues(myTablePtr,Memory_Device ,offset,reservedUnknown,2, specDesc,fieldLen);
    
    specDesc =" 4.8.8  Device Set is not FFh(Unknown). ";
    cout << "// " << specDesc << endl;
    offset = 0x0f;
    errorValues[0] = 0xFF;
    checkByteValues(myTablePtr,Memory_Device ,offset,errorValues, 1, specDesc,fieldLen);

    specDesc =" 4.8.9  Device Locator string is present and non-null. ";
    cout << "// " << specDesc << endl << endl;
    offset = 0x10;
    checkStringNotNull(myTablePtr,Memory_Device,offset,specDesc);
}

void validateMemoryMappedArrayAddress()
{
    cout << "// 4.9 Validate Memory Mapped Array Address (Type 19 - Memory_Array_Mapped_Address). " << endl;
    smbios::ISmbiosTable *myTablePtr = smbios::SmbiosFactory::getFactory()->getSingleton();
    u8 offset = 0x00;
    std::string specDesc =" 4.9.1 One structure is provided for each contiguous block of memory addresses mapped to a Physical Memory Array. ";
    cout << "** " << specDesc << endl;

    specDesc = " 4.9.2 Each structure's length is atleast 0Fh. ";
    cout << "// " << specDesc << endl;
    checkStructLength(myTablePtr,Memory_Array_Mapped_Address,0x0f,specDesc);
    
    specDesc = " 4.9.3 Ending address value is higher in magnitude than the Starting Address value. ";
    cout << "// " << specDesc << endl;
    u8 startOffset = 0x04; // starting address offset
    u8 endOffset = 0x08; // ending address offset
    checkAddresses(myTablePtr,Memory_Array_Mapped_Address,startOffset,endOffset,specDesc);
    
    specDesc = " 4.9.4 Memory Array Handle references a Physical Memory Array (Type 16). ";
    cout << "// " << specDesc << endl;
    offset = 0x0c; 
    checkMemoryHandles(myTablePtr,Memory_Array_Mapped_Address, Physical_Memory_Array, offset, specDesc);

    specDesc = " 4.9.5 Each structure's address range (Starting Address to Ending Address) is unique and non-overlaping. ";
    cout << "// " << specDesc << endl;
    smbios::ISmbiosTable::iterator item = (*myTablePtr)[Memory_Array_Mapped_Address] ;
    u32 startAddress = item->getU32(startOffset);
    u32 endAddress = item->getU32(endOffset);
    while(++item != myTablePtr->end())
    {
        ++item;
        if(((item->getU32(startOffset)) < endAddress)
            ||((item->getU32(startOffset) == startAddress)
                &&(item->getU32(endOffset) == endAddress)))
        {
            int errorValues[2] = { item->getU32(startOffset),
                endAddress
            };
            int index=2;
            reportItemException(specDesc,item,errorValues,index);
        }
        startAddress = item->getU32(startOffset);
        endAddress = item->getU32(endOffset);
    }
    int errorValues[] = {0x00};
    int fieldLen = 1;
    specDesc =" 4.9.6 Partition width is not 0. ";
    cout << "// " << specDesc << endl << endl;
    offset = 0x0e; // Partition width offset
    checkByteValues(myTablePtr,Memory_Array_Mapped_Address ,offset,errorValues, 1, specDesc,fieldLen);
}

void validateMemoryDeviceMappedAddress()
{
    cout << "// 4.10 Validate Memory Device Mapped Address (Type 20 - Memory_Device_Mapped_Address) " << endl;
    smbios::ISmbiosTable *myTablePtr = smbios::SmbiosFactory::getFactory()->getSingleton();
    u8 offset = 0x00;
    std::string specDesc = " 4.10.1 Sufficient structures are provided to provide device-level mapping to all address space defined by the Memory Array Mapped Address(Type 19) structures. ";
    cout << "** " << specDesc << endl;
    
    specDesc = " 4.10.2 Each strcture's length is atleast 13h "; 
    cout << "// " << specDesc << endl;
    checkStructLength(myTablePtr,Memory_Device_Mapped_Address,0x13,specDesc);

    specDesc = " 4.10.3 Ending address value is higher in magnitude than the Starting Address value. ";
    cout << "// " << specDesc << endl;
    u8 startOffset = 0x04; // starting address offset
    u8 endOffset = 0x08; // ending address offset
    checkAddresses(myTablePtr,Memory_Device_Mapped_Address,startOffset,endOffset,specDesc);

    specDesc = " 4.10.4 Memory Device Handle references a Memory Device  (Type 17) structure. ";
    cout << "// " << specDesc << endl;
    offset = 0x0c; 
    checkMemoryHandles(myTablePtr,Memory_Device_Mapped_Address, Memory_Device, offset, specDesc);

    specDesc = " 4.10.5 Memory Array Mapped Address Handle references a Memory Array Mapped Address (Type 19) structure. "; 
    cout << "// " << specDesc << endl;
    offset = 0x0e; 
    checkMemoryHandles(myTablePtr,Memory_Device_Mapped_Address, Memory_Array_Mapped_Address, offset, specDesc);

    specDesc = " 4.10.6 Partition Row Position value is not 0(Reserved), 0FFh(Unknown), or greater than the Partition Width field of the referenced Memory Array Mapped Address structure. "; 
    cout << "// " << specDesc << endl;
    offset = 0x10;
    for( smbios::ISmbiosTable::iterator item = (*myTablePtr)[Memory_Device_Mapped_Address] ; item != myTablePtr->end(); ++item)
    {
        bool largeRow=false;
        for( smbios::ISmbiosTable::iterator array = (*myTablePtr)[Memory_Array_Mapped_Address] ; array != myTablePtr->end(); ++array)
        {
            // 0x0e - offset for Memory Array Mapped Address Handle
            // 0x0e - Partition width
            if((item->getU16(0x0e) == array->getHandle())
                    &&(item->getU8(offset) > array->getU8(0x0e)))
            {
                largeRow=true;
                break;
            }
        }
        if((item->getU8(offset) == 0x00)
                ||(item->getU8(offset) == 0x0ff)||(largeRow))
        {
            int errorValues[1] = { item->getU8(offset) };
            int index = 1;
            reportItemException(specDesc,item,errorValues,index);
        }
    }

    int errorValues[1] = {0xFF};
    int fieldLen = 1;
    specDesc =" 4.10.7 Interleave position is not FF(Unknown). "; 
    cout << "// " << specDesc << endl;
    offset = 0x11;
    checkByteValues(myTablePtr, Memory_Device_Mapped_Address, offset, errorValues, 1, specDesc,fieldLen);

    specDesc =" 4.10.8 Interleaved data is not FF(Unknown). "; 
    cout << "// " << specDesc << endl << endl << endl;
    offset = 0x12;
    checkByteValues(myTablePtr, Memory_Device_Mapped_Address, offset, errorValues, 1, specDesc,fieldLen);
}

void validateBISEntryPoint()
{
    cout << "// 4.11 Validate Boot Integrity Services(BIS) Entry Point (Type 31 - Boot_Integrity_Services_Entry_Point). This structure is optional, but if it is present the following checks are performed " << endl;
    smbios::ISmbiosTable *myTablePtr = smbios::SmbiosFactory::getFactory()->getSingleton();
    u8 offset = 0x00;
    int entries = countItem(myTablePtr, Boot_Integrity_Services_Entry_Point);
    if( entries > 0 )
    {
        std::string specDesc = " 4.11.1 The structure's length is atleast 1Ch. ";
        cout << "// " << specDesc << endl;
        checkStructLength(myTablePtr,Boot_Integrity_Services_Entry_Point,0x1c,specDesc);
        offset = 0x02;
        specDesc = " 4.11.2 The structures-level checksum evaluates to 00h.";
        cout << "// " << specDesc << endl;
        for( smbios::ISmbiosTable::iterator item = (*myTablePtr)[Boot_Integrity_Services_Entry_Point] ; item != myTablePtr->end(); ++item)
        {
            u8 checksum = 0;
            size_t size = 0;
            const u8 *ptr = item->getBufferCopy(size);
            for( unsigned int i = 0; i < size; ++i )
            {
                checksum += ptr[i];
            }
                    
            if(checksum != 0x00)
            {
                int values[1] = { checksum };
                int index = 1;
                reportItemException(specDesc,item,values,index);
            }
        }
    
        offset = 0x06;
        specDesc = " 4.11.3 16-bit Entry Point is not 0. ";
        cout << "// " << specDesc << endl;
        int fieldLen = 2;
        int errorValues[] = {0};
        checkByteValues(myTablePtr,Boot_Integrity_Services_Entry_Point ,offset,errorValues, 1, specDesc,fieldLen);
    
        offset = 0x08;
        specDesc =" 4.11.4 32-bit Entry Point is not 0. ";
        cout << "// " << specDesc << endl;
        fieldLen = 4;
        checkByteValues(myTablePtr,Boot_Integrity_Services_Entry_Point,offset,errorValues, 1, specDesc,fieldLen);
    }
    cout << endl;
}

void validateSystemBootInformation()
{
    cout << "// 4.12 System Boot Information (Type 32 - System_Boot_Information) " << endl ;
    smbios::ISmbiosTable *myTablePtr = smbios::SmbiosFactory::getFactory()->getSingleton();
    std::string specDesc =" 4.12.1 One and only one structure of this type is present "; 
    cout << "// " << specDesc << endl << endl;
    int entries = countItem(myTablePtr, System_Boot_Information);
    if( entries != 1 )
    {
        wrongNumberOfEntriesException(myTablePtr,System_Boot_Information ,entries,specDesc);
    }
}


void validateCmosTokens()
{
    smbios::TokenTableFactory *ttFactory = smbios::TokenTableFactory::getFactory() ;
    smbios::ITokenTable *tokenTable = ttFactory->getSingleton();

    // initial check that checksums are correct.
    // This will catch any wrong checksums, or places where BIOS has specified
    // the wrong algorithm to use to do checksum.
    cout << "// Dell Specific CMOS 1: Validate CMOS checksums are correct." << endl;
    cout << "                       : Validate CMOS algorithms are correct." << endl; 
    cmos::ICmosRW *cmos = cmos::CmosRWFactory::getFactory()->getSingleton();
    observer::IObservable *o = dynamic_cast<observer::IObservable*>(cmos);
    bool doUpdate = false;
    if(o)
        o->notify(&doUpdate);

    // warning: Destructive test case.
    // flips all bits in the token table to ensure that they all flip correctly.
    // This will catch any incorrect "and masks" or "or values".
    cout << "// Dell Specific CMOS 2: Destructively flip all bits to ensure masks are correct." << endl;
    cout << "                       : Destructively set all strings to ensure strings are proper." << endl; 
    smbios::ITokenTable::iterator token = tokenTable->begin();
    while( token != tokenTable->end() )
    {
        //cout << *token << endl;
        if( token->isString() )
        {
            const char *testStr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890abcdefghijklmnop";
            u8 *testStrU8 = reinterpret_cast<u8*>(testStr);
            u8 *myStr=0;
            u8 *myStr1=0;
            try
            {
                unsigned int size = token->getStringLength() + 1;

                myStr1 = new u8[ size ];
                memset( myStr1, 0, size );
                token->getString( myStr1, size );

                token->setString( testStrU8, strlen(testStr) + 1 );

                myStr = new u8[ size ];
                memset( myStr, 0, size );
                token->getString( myStr, size );

                // return might be smaller, only compare up to what was stored.
                if( 0 != memcmp( testStr, reinterpret_cast<char*>(myStr), size - 1 ) )
                {
                    // FAILED
                    ostringstream ost;
                    ost << "String set on token failed." << endl;
                    ost << (*token) << endl;
                    ost << "Size of string to compare is: " << size-1 << endl;
                    ost << " (note data below includes full orig string, but only the size listed above actually written.)" << endl;
                    ost << "Original data: (" << myStr1  << ")" << endl;
                    ost << "Wrote        : (" << testStr << ")" << endl;
                    ost << "Read back    : (" << myStr   << ")" << endl;
                    excList.push_back(myException(ost.str()));
                }
                // simulate a finally: block.  :-)
                throw std::exception();
            }
            catch(...)
            {
                // we get here no matter what, so no worries about leaks.
                delete [] myStr1;
                delete [] myStr;
                myStr1 = 0;
                myStr = 0;
                // note that we eat the exception, that is ok.
            }
        }
        else
        {
            token->activate();
            if( ! token->isActive() )
            {
                ostringstream ost;
                ost << "Failed to SET bit token. Token data: " << endl;
                ost << (*token);
                excList.push_back(myException(ost.str()));
            }
        }
        ++token;
    }
}

validator validationFunctions[] =
{
    {&validateTableEntryPoint},
    {&validateOverall},
    {&validateBiosBlock},
    {&validateSystemInformation},
    {&validateSystemEnclosure},
    {&validateProcessorInformation},
    {&validateCacheInformatin},
    {&validateSystemSlots},
    {&validatePhysicalMemoryArray},
    {&validateMemoryDevice},
    {&validateMemoryMappedArrayAddress},
    {&validateMemoryDeviceMappedAddress},
    {&validateBISEntryPoint},
    {&validateSystemBootInformation},
    {&validateCmosTokens},
};

int
main (int argc, char **argv)
{
    // to run directly on system, comment out this test. (read below first,
    // though)
    if( argc < 3 )
    {
        // for the love of god, don't run this directly on your system! It 
        // destroys the contents of CMOS during the run.
        cout << endl;
        cout << "usage: validateBios [memory dump file] [cmos dump file]" << endl;
        cout << endl;
        cout << "System Direct Mode is not available in this version of the validation tool." << endl;
        cout << endl;
        cout << "Please run the 'createUnitTestFiles' executable and use those as inputs to" << endl;
        cout << "  this tool. " << endl;
        cout << endl;
        cout << "WARNING, WARNING, WARNING: You have attempted to run the BIOS validation tool" << endl;
        cout << "  in System Direct Mode. This action would normally cause the tool to work" << endl;
        cout << "  directly with the system memory and CMOS. The tests that this tool performs" << endl;
        cout << "  are DESTRUCTIVE! CMOS will be left with undefined data if this tool is run" << endl;
        cout << "  directly against CMOS. This operation is prohibited in this version of the tool." << endl;
        exit(1);
    }

    string fileName( "" );
    if( argc > 1 )
        fileName = argv[1];

    string cmosFileName("");
    if( argc > 2 )
        cmosFileName =  argv[2];

    string xmlFileName( "" );
    if( argc > 3 )
        xmlFileName = argv[3];

    try
    {
        smbios::SmbiosFactory *smbiosFactory = smbios::SmbiosXmlFactory::getFactory();

        if( fileName != "" )
        {
            memory::MemoryFactory *memoryFactory = memory::MemoryFactory::getFactory();
            memoryFactory->setParameter("memFile", fileName);
            memoryFactory->setMode( memory::MemoryFactory::UnitTestMode );
        }

        if( cmosFileName != "" )
        {
            cmos::CmosRWFactory *cmosFactory = cmos::CmosRWFactory::getFactory();
            cmosFactory->setParameter("cmosMapFile", cmosFileName);
            cmosFactory->setMode( cmos::CmosRWFactory::UnitTestMode );
        }

        if( xmlFileName != "" )
            smbiosFactory->setParameter("xmlFile", xmlFileName);

        smbios::ISmbiosTable *myTablePtr = smbiosFactory->getSingleton();
        myTablePtr->clearItemCache();
        myTablePtr->rawMode(true);

        int numEntries =
            sizeof (validationFunctions) / sizeof (validationFunctions[0]);

        cout << "BIOS Validation tool." << endl;
        cout << endl;
        cout << "Tests conformance to the SMBIOS specification." << endl;
        cout << "Each prints out a header as it is run. The header looks like this: " << endl;
        cout << "\t// X.Y  Check some aspect of SMBIOS table." << endl;
        cout << endl;
        cout << "The X.Y corresponds to a section of the SMBIOS conformance testing specification." << endl;
        cout << "At the end of the test run, a summary of all errors found is printed." << endl;
        cout << endl;
        cout << "This checking tool is still undergoing development. All tests that do not" << endl;
        cout << "actually implement a test are prefixed with '**' instead of '//' until they" << endl;
        cout << "are actually implemented." << endl;
        cout << endl;
        cout << "Validating SMBIOS table:" << endl;
        
        for (int i = 0; i < numEntries; ++i)
        {
            try
            {
                validationFunctions[i].f_ptr ();
            }
            catch ( exception &e )
            {
                excList.push_back(e);
            }
        }
    }
    catch ( smbios::ParseException &)
    {
    }

    cout << endl << endl;
    cout << "SMBIOS Conformance testing has finished." << endl;
    cout << "Here are the findings from that validation phase:" << endl;
    int failures = 0;
    for( vector<exception>::iterator i = excList.begin(); i != excList.end(); ++i )
    {
        cout << "===============================================================================" << endl;
        cout << i->what() << endl;
        ++failures;
    }
    cout << "===============================================================================" << endl;
    cout << endl << "Done." << endl;

    return failures;
}
