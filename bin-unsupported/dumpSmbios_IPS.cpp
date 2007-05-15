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

/*****************************************************************************
README:
    This executable is unsupported.

    The reason for this is that the output format from this file is not the
    final XML output format. The output format _will_ be changing, and this 
    file will become obsolete.

    This file is provided for use by the Dell IPS group as an interim format
    until the final output format is solidified and conversion tools exist to
    transform the official format to this executable's output format.

    At that time, this executable will be removed.

 ****************************************************************************/

// compat header should always be first header if including system headers
#include "smbios/compat.h"

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>

#include "smbios/ISmbios.h"
#include "smbios/ISmbiosXml.h"
#include "smbios/IMemory.h"  // only needed if you want to use fake input (memdump.dat)
#include "smbios/version.h"
#include "../libraries/common/XmlUtils.h"   // illegal... 
#include "../libraries/smbiosxml/SmbiosXmlImpl_xerces.h"   // illegal... 
#include "getopts.h"

// always include last if included.
#include "smbios/message.h"  // not needed outside of this lib. (mainly for gettext i18n)

using namespace std;
using namespace smbios;
XERCES_CPP_NAMESPACE_USE;
using namespace xmlutils;

struct options opts[] =
    {
        { 1, "xml", "Dump SMBIOS table structures in XML format", "x", 0 },
        { 254, "memory_file", "Debug: Memory dump file to use instead of physical memory", "m", 1 },
        { 255, "version", "Display libsmbios version information", "v", 0 },
        { 0, NULL, NULL, NULL, 0 }
    };



/******************************************************************************
  *       XML OUTPUT FUNCTIONS                                                *
  ****************************************************************************/
// extra defines...
namespace smbios {
    unsigned int parseLengthStr(string size);
    void printStructureField( std::ostream &cout, const DOMNode *node, const ISmbiosItem &item );
    const string getStringForType(const DOMDocument *doc, const int searchForType );
}

	string BeginTag (std::string instr, std::string tagStart="<")
	{
		// fix illegal chars
        const char *illegals = " ,-/";
        for(size_t i=0; i<strlen(illegals); ++i)
        {
		    int ndx_spc = instr.find( illegals[i] ,1);
		    while (ndx_spc > 0)
		    {
			    instr.at(ndx_spc) = '_';
			    ndx_spc = instr.find( illegals[i] ,ndx_spc);
		    }
        }

		// now that we have a good tag name XML ize it
		std::string mystr ;
		mystr.reserve(instr.length()+2);
		mystr.append(tagStart);
		mystr.append(instr);
		mystr.append(">");
		// now output tag to cout
		return  mystr;
	}

	string EndTag (std::string instr)
	{
        return BeginTag(instr, "</");
	}
    

    void parseStructureField (DOMElement *inNode, const ISmbiosItem &itemRef,std::ostream &cout) 
    { 
        string strOffset = safeGetAttribute( inNode, "offset" );
        unsigned int offset = strtol( strOffset.c_str(), 0, 0 );

        XMLCh *bitName = X("BITS"); // NEED TO 'release' !!!
        DOMNodeList *bitList = inNode->getElementsByTagName(bitName);
        XMLString::release(&bitName);

        unsigned int size = parseLengthStr(safeGetAttribute (inNode ,"length"));

        // if whole-FIELD is ENUM, short-circuit parsing below
        string type= safeGetAttribute(inNode ,"usage");
        bool isEnum = false;
        bool isBitfield = false;
        if( type == "ENUM" )
            isEnum = true;
        if( type == "BITFIELD" )
            isBitfield = true;

        cout << "    " << BeginTag(safeGetAttribute( inNode, "name" )) << endl;
        cout << "        " << BeginTag("raw_data");
        printStructureField(cout, inNode, itemRef);
        cout << EndTag("raw_data") << endl;

        if(bitList->getLength() && (isBitfield || isEnum))
            cout << "        " << BeginTag("interpreted_data") << endl;


        for( unsigned int index = 0; index < bitList->getLength(); ++index )
        {
            std::ios::fmtflags old_opts = cout.flags ();
            DOMNode *node = bitList->item( index );
            if( node->getNodeType() != DOMNode::ELEMENT_NODE )
                continue;

            string lsbStr= safeGetAttribute(node ,"lsb");
            if(lsbStr == "") lsbStr = "0";
            unsigned int lsb = strtoul(lsbStr.c_str(), NULL, 0);

            string msbStr = safeGetAttribute(node ,"msb");
            unsigned int msb = 0;
            if(msbStr == "")
                msb = size * 8 - 1;
            else
                msb = strtoul(msbStr.c_str(), NULL, 0);

            string type= safeGetAttribute(node ,"type");
            if( type == "ENUM" || isEnum )
            {
                u32 bitValue = 0;
                getBits_FromItem(itemRef, offset, &bitValue, lsb, msb );

                cout << "            " << BeginTag("EnumValue");
                try
                {
                    DOMNode *mapValue = findElementWithNumericAttr( castNode2Element(node), "MAP", "value", bitValue );
                    cout << safeGetAttribute(mapValue, "meaning");
                }
                catch(const NotFound &)
                {
                    cout << "UNKNOWN enumeration value: " << bitValue;
                }

                cout << EndTag("EnumValue") << endl;
            }
            else if (isBitfield)
            {
                if( lsb == msb )
                {
                    if(isBitSet(&itemRef, offset, lsb))
                    {
                        cout << "            " << BeginTag("EnabledBit");
                        cout << safeGetAttribute( node, "name" );
                        cout << EndTag("EnabledBit") << endl;
                    }
                    else
                    {
                        cout << "            " << BeginTag("DisabledBit");
                        cout << safeGetAttribute( node, "name" );
                        cout << EndTag("DisabledBit") << endl;
                    }
                }
                else
                {
                        cout << "            " << BeginTag("MultiBits");
                        u32 mb=0;
                        getBits_FromItem(itemRef, offset, &mb, lsb, msb);
                        cout << "0x" << hex << mb;
                        cout << EndTag("MultiBits") << endl;
                }
            }
            else
            {
                // normal value that may have 'default' data. try to look 
                // up default name, fall back to just printing value
                u32 bitValue = 0;
                getBits_FromItem(itemRef, offset, &bitValue, lsb, msb );
                try
                {
                    DOMNode *mapValue = findElementWithNumericAttr( castNode2Element(node), "MAP", "value", bitValue );
                    cout << "        " << BeginTag("interpreted_data") << endl;
                    cout << "            " << BeginTag("RawValueMeaning");
                    cout << safeGetAttribute(mapValue, "meaning");
                    cout << EndTag("RawValueMeaning") << endl;
                    cout << "        " << EndTag("interpreted_data") << endl;
                }
                catch(const NotFound &)
                {
                }
            }
            cout.flags (old_opts);
        }

        if(bitList->getLength() && (isBitfield || isEnum))
            cout << "        " << EndTag("interpreted_data") << endl;

        cout << "    " << EndTag(safeGetAttribute( inNode, "name" )) << endl;
    } 

     // this will output one structure at a time as passed to std out
    std::ostream &IPS_toXmlString(const ISmbiosItem &item,const DOMDocument *doc, DOMDocument *oDoc, std::ostream &cout)
    {
        (void)(oDoc);

        std::ios::fmtflags old_opts = cout.flags ();

        // If we don't have a ref to XML file, we cannot find this info
        if( ! doc )
            return cout;

        DOMElement *Structure = 0;
        try
        { // find the structure TAG in std_xml to begin iteration
            Structure = findElementWithNumericAttr(doc->getDocumentElement(), "STRUCTURE", "type", item.getType());
        }
        catch (const NotFound &)
        {
            Structure = findElement(doc->getDocumentElement(), "STRUCTURE", "type", "unknown");
        }

        cout << BeginTag(smbios::getStringForType(doc, item.getType())) << endl;

        XMLCh *tagName = X("FIELD"); // NEED TO 'release' !!!
        DOMNodeList *fieldList = Structure->getElementsByTagName(tagName);
        XMLString::release(&tagName);

        // in this loop we will iterate through the Structures's  subtags as attributes
        for( unsigned int index = 0; index < fieldList->getLength(); ++index )
        {
            DOMNode *node = fieldList->item( index );
            if( node->getNodeType() == DOMNode::ELEMENT_NODE )
            {
                ostringstream tmpBuf("");
                try
                {
                    parseStructureField(castNode2Element(node),item,tmpBuf);
                }
                catch(const exception &)
                {
                    continue;
                }
                cout << tmpBuf.str();
            }
        }

        // close root tag
        cout << EndTag(smbios::getStringForType( doc, item.getType() ) ) << endl;

        cout.flags (old_opts);
        return cout;

    } 


    /******************
        TABLE OUTPUT
      ****************/

     // here is where we will do the XML output
    std::ostream &IPS_toXmlString(const ISmbiosTable &table, ostream & cout)
    {
        ISmbiosTable::const_iterator position = table.begin();
        const DOMDocument *doc = dynamic_cast<const SmbiosTableXml *>(& table)->getXmlDoc();
        while (position != table.end())
        {
            IPS_toXmlString(*position, doc, 0, cout);
            ++position;
        }
        return cout;
     }


int
main (int argc, char **argv)
{
    int retval = 0;
    try
    {
        int c=0;
        char *args = 0;
        memory::MemoryFactory *memoryFactory = 0;
        bool xml = false;

        while ( (c=getopts(argc, argv, opts, &args)) != 0 )
        {
            switch(c)
            {
            case 1:
                xml = true;
                break;
            case 254:
                // This is for unit testing. You can specify a file that
                // contains a dump of memory to use instead of writing
                // directly to RAM.
                memoryFactory = memory::MemoryFactory::getFactory();
                memoryFactory->setParameter("memFile", args);
                memoryFactory->setMode( memory::MemoryFactory::UnitTestMode );
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

        smbios::SmbiosFactory *smbiosFactory = smbios::SmbiosXmlFactory::getFactory();
        smbios::ISmbiosTable *table = smbiosFactory->getSingleton();

        if(xml)
        {
            cout << "<?xml-stylesheet type=\"text/xsl\" version=\"1.0\" encoding=\"UTF-8\"?>\n<SMBIOS>\n";;
            IPS_toXmlString(*table, cout);
            cout << "</SMBIOS>" << endl;
        }
        else
            cout << *table << endl;

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
