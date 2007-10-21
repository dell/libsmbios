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
#define LIBSMBIOS_SOURCE
#include "smbios/compat.h"

#include <sstream>
#include <iomanip>
#include <string.h>

#include "SmbiosXmlImpl.h"
#include "StdSmbiosXml.h"
#include "FactoryImpl2.h"
#include "XmlUtils.h"

// message.h should be included last.
#include "smbios/message.h"

// no trailing ';' because macro already has one
SETUP_XML_NAMESPACE

using namespace std;
using namespace smbiosLowlevel;
using namespace xmlutils;

#if defined(DEBUG_SMBIOSXML)
#   define DCOUT(line) do { cout << line; } while(0)
#   define DCERR(line) do { cerr << line; } while(0)
#else
#   define DCOUT(line) do {} while(0)
#   define DCERR(line) do {} while(0)
#endif

namespace smbios
{
    //
    // FACTORY
    //
    SmbiosXmlFactory::~SmbiosXmlFactory() throw()
    {}

    class SmbiosXmlFactoryImpl: public SmbiosFactoryImpl
    {
    public:
        virtual ISmbiosTable *makeNew();
        SmbiosXmlFactoryImpl() : SmbiosFactoryImpl() {};
        virtual ~SmbiosXmlFactoryImpl() throw () {};
    };

    SmbiosFactory *SmbiosXmlFactory::getFactory()
    {
        // reinterpret_cast<...>(0) to ensure template parameter is correct
        // this is a workaround for VC6 which cannot use explicit member template
        // funciton initialization.
        return SmbiosFactoryImpl::getFactory(reinterpret_cast<SmbiosXmlFactoryImpl *>(0));
    }

    ISmbiosTable *SmbiosXmlFactoryImpl::makeNew()
    {
        // stupid, ugly hack to supress (C4800) warning on msvc++
        bool strict = getParameterNum("strictValidation") ? 1 : 0;

        SmbiosTableXml *table = 0;

        std::vector<SmbiosStrategy *> strategies;

        if (mode == AutoDetectMode)
        {
            strategies.push_back( new SmbiosMemoryStrategy(getParameterNum("offset")) );
#ifdef LIBSMBIOS_PLATFORM_WIN32
            strategies.push_back( new SmbiosWinGetFirmwareTableStrategy() );
            strategies.push_back( new SmbiosWinWMIStrategy() );
#endif
        }
        else if (mode == UnitTestMode)
        {
            strategies.push_back( new SmbiosMemoryStrategy(getParameterNum("offset")) );
        }
        else
        {
        throw NotImplementedImpl(_("Unknown smbios factory mode requested"));
        }


        table = new SmbiosTableXml( 
                strategies,
                strict 
            );
        table->setXmlFilePath( getParameterString("xmlFile") );
        table->initializeWorkaround();
        return table;
    }


    // if user give us a file with smbios xml information, use that
    // if there are any problems parsing the doc, or if they do not give us a
    // filename, use the built-in xml stuff.
    DOMDocument *getSmbiosXmlDoc( DOMBuilder *parser, std::string &xmlFile )
    {
        DOMDocument *doc = 0;

        // parse
        DCERR("Trying to parse file: '" << xmlFile << "'" << endl);
        // this is a macro that sets doc.
        compatXmlReadFile(parser, doc, xmlFile.c_str());

        if (!doc)
        {
            DCERR("Parse failed, no valid doc. Trying internal XML." << endl);
            // this is a macro that sets doc.
            compatXmlReadMemory(parser, doc, stdXml, strlen(stdXml));
        }

        if (!doc)
        {
            DCERR("Bad stuff... file parse failed and internal failed." << endl);
            throw ParseExceptionImpl("problem parsing xml file.");
        }

        DCERR("Returning doc."<< endl);
        return doc;
    }

    void validateSmbiosXmlDoc( DOMDocument *doc )
    {
        xmlNodePtr cur = xmlDocGetRootElement(doc);
    
        if (cur == NULL) 
        {
            fprintf(stderr,"empty document\n");
            xmlFreeDoc(doc);
            throw ParseExceptionImpl("problem parsing xml file. empty document.");
        }
        
        if (xmlStrcmp(cur->name, reinterpret_cast<const xmlChar *>("STRUCTUREDEFS"))) 
        {
            fprintf(stderr,"document of the wrong type, root node != story");
            xmlFreeDoc(doc);
            throw ParseExceptionImpl("problem parsing xml file. root doc name not STRUCTUREDEFS.");
        }
    }

    unsigned int parseLengthStr(string size)
    {
       if (size == "BYTE")
           return 1;
       else if (size == "WORD")
           return 2;
       else if (size == "DWORD")
           return 4;
       else if (size == "QWORD")
           return 8;

       return strtol(size.c_str(), NULL, 0);
       //throw ParseExceptionImpl("Error parsing length information xml file. Invalid value." );
       //return 0;
    }

    void verifyElementAttr( const DOMElement *element, const string elementName, const string value )
    {
        string xmlValue = safeGetAttribute( element, elementName );
        if( value != xmlValue )
            throw ParseExceptionImpl("could not verify element attribute.");
    }

    // sneaky... :-)
    void verifyElementAttr( const DOMElement *element, const string elementName, unsigned int size )
    {
        string xmlValue = safeGetAttribute( element, elementName );
        if( size != parseLengthStr(xmlValue) )
            throw ParseExceptionImpl("could not verify element attribute was correct size.");
    }

    int getTypeForString( DOMDocument *doc, const string searchForDesc )
    {
        // find element with this description
        DOMElement *elem = findElement( xmlDocGetRootElement(doc), "STRUCTURE", "description", searchForDesc );

        // return the type as an INT.
        return strtol( safeGetAttribute( elem, "type" ).c_str(), 0, 0);
    }

    const string getStringForType(const DOMDocument *doc, const int searchForType )
    {
        // find matching element
        DOMElement *elem = 0;
        try
        {
            elem = findElementWithNumericAttr( xmlDocGetRootElement(doc), "STRUCTURE", "type", searchForType);
        }
        catch(const NotFound &)
        {
            elem = findElement( xmlDocGetRootElement(doc), "STRUCTURE", "type", "unknown");
        }

        // extract the description
        return safeGetAttribute( elem, "description");
    }

    //
    // MEMBER FUNCTIONS
    //

    // CONSTRUCTORS
    //
    // REGULAR CONSTRUCTOR
    SmbiosTableXml::SmbiosTableXml()
            : SmbiosTable(), xmlFile(""), parser(0), doc(0), xmlInitialized(false)
    {
        CHECK_VERSION_COMPAT;
        setXmlFilePath(xmlFile);
    }

    SmbiosTableXml::SmbiosTableXml(std::vector<SmbiosStrategy *> initStrategyList, bool strictValidation)
            : SmbiosTable(initStrategyList, strictValidation), xmlFile(""), parser(0), doc(0), xmlInitialized(false)
    {
        CHECK_VERSION_COMPAT;
        setXmlFilePath(xmlFile);
    }

    // DESTRUCTOR
    SmbiosTableXml::~SmbiosTableXml()
    {
        if (parser)
            xmlFreeParser(parser);

        if (doc)
            xmlFreeDoc(doc);

        if( xmlInitialized )
            FiniXML();
    }


    // good exception guarantee.
    // either we allocate new stuff, the new stuff validates, and we
    // set ourselves up with the new stuff, or we keep whatever we
    // used to have and raise the exception.
    void SmbiosTableXml::setXmlFilePath( std::string newFile )
    {
        try
        {
            // Initialize XML DOM subsystem
            if( ! xmlInitialized )
                InitXML();

            xmlInitialized = true;

            DOMBuilder *newParser = getParser();
            DOMDocument *newdoc = getSmbiosXmlDoc( newParser, newFile );
            validateSmbiosXmlDoc( newdoc );
            // if we get to this point, that means the
            // new doc exists and is valid.

            if (parser)
                xmlFreeParser(parser);

            if (doc)
                xmlFreeDoc(doc);

            parser = newParser;
            xmlFile = newFile;
            doc = newdoc;
        }
        catch(const exception &toCatch)
        {
            cerr << "Error during XML Initialization.\n"
            << "  Exception message:"
            << toCatch.what() << endl;
            throw ParseExceptionImpl("XML initialization failed.");
        }
    }

    const DOMDocument *SmbiosTableXml::getXmlDoc() const
    {
        return doc;
    }

    int SmbiosTableXml::getTypeForString( const string searchForDesc ) const
    {
        return smbios::getTypeForString( doc, searchForDesc );
    }

    // only used by unit test code.
    const string SmbiosTableXml::getStringForType( const int searchForType ) const
    {
        return smbios::getStringForType( doc, searchForType );
    }

    // we were passed a string. convert to a number by looking up the
    // type for this string in the XML File
    // forward to base class operator[]
    SmbiosTable::iterator SmbiosTableXml::operator[] (const string &searchFor)
    {
        int type = getTypeForString( searchFor );
        return SmbiosTable::iterator (this, type);
    }

    // we were passed a string. convert to a number by looking up the
    // type for this string in the XML File
    // forward to base class operator[]
    SmbiosTable::const_iterator SmbiosTableXml::operator[](const string &searchFor) const
    {
        // this == const SmbiosTable();
        int type = getTypeForString( searchFor );
        return SmbiosTable::const_iterator (this, type);
    }


    static void getData_UsingXml(const ISmbiosItem &item, const string fieldName, void *out, size_t size )
    {
        DOMElement *element = 0;

        smbios::ISmbiosTable *table =
                    smbios::SmbiosFactory::getFactory()->getSingleton();

        const SmbiosTableXml *tableXml = dynamic_cast<const SmbiosTableXml *>(table);
        if(!tableXml)
            throw NotImplementedImpl();

        const DOMDocument *doc = tableXml->getXmlDoc();

        // get the element corresponding to the STRUCTURE user specified
        DOMElement *Structure = findElementWithNumericAttr( xmlDocGetRootElement(doc), "STRUCTURE", "type", item.getType() );
        element = findElement( Structure, "FIELD", "name", fieldName );

        // Is this the correct length?
        verifyElementAttr( element, "length", size );

        // call parent method to get actual data. :-)
        item.getData( getNumberFromXmlAttr(element, "offset", 0), out, size );
    }

    u8 getU8_FromItem(const ISmbiosItem &item, std::string field)
    {
        u8 retval = 0;
        getData_UsingXml(item, field, reinterpret_cast<u8 *>(&retval), sizeof(retval));
        return retval;
    }

    u16 getU16_FromItem(const ISmbiosItem &item, std::string field)
    {
        u16 retval = 0;
        getData_UsingXml(item, field, reinterpret_cast<u8 *>(&retval), sizeof(retval));
        return retval;
    }

    u32 getU32_FromItem(const ISmbiosItem &item, std::string field)
    {
        u32 retval = 0;
        getData_UsingXml(item, field, reinterpret_cast<u8 *>(&retval), sizeof(retval));
        return retval;
    }

    u64 getU64_FromItem(const ISmbiosItem &item, std::string field)
    {
        u64 retval = 0;
        getData_UsingXml(item, field, reinterpret_cast<u8 *>(&retval), sizeof(retval));
        return retval;
    }

    const char *getString_FromItem(const ISmbiosItem &item, std::string field)
    {
        DOMElement *element = 0;

        smbios::ISmbiosTable *table =
                    smbios::SmbiosFactory::getFactory()->getSingleton();

        const SmbiosTableXml *tableXml = dynamic_cast<const SmbiosTableXml *>(table);
        if(!tableXml)
            throw NotImplementedImpl();

        const DOMDocument *doc = tableXml->getXmlDoc();

        // get the element corresponding to the STRUCTURE user specified
        DOMElement *Structure = findElementWithNumericAttr( xmlDocGetRootElement(doc), "STRUCTURE", "type", item.getType() );
        element = findElement( Structure, "FIELD", "name", field );

        // Is this the correct length?
        verifyElementAttr( element, "length", 1 );

        // Is this an actual string?
        verifyElementAttr( element, "usage", "STRING" );

        // call parent method to get actual data. :-)
        return getString_FromItem(item, getNumberFromXmlAttr(element, "offset", 0) );
    }

    void *getBits_FromItem(const ISmbiosItem &item, const string field, const string bitField, void *out)
    {
        DOMElement *bitElement = 0;
        DOMElement *fieldElement = 0;

        smbios::ISmbiosTable *table =
                    smbios::SmbiosFactory::getFactory()->getSingleton();

        const SmbiosTableXml *tableXml = dynamic_cast<const SmbiosTableXml *>(table);
        if(!tableXml)
            throw NotImplementedImpl();

        const DOMDocument *doc = tableXml->getXmlDoc();

        try
        {
            DOMElement *Structure = findElementWithNumericAttr( xmlDocGetRootElement(doc), "STRUCTURE", "type", item.getType() );
            fieldElement = findElement( Structure, "FIELD", "name", field );
            bitElement = findElement( fieldElement, "BITS", "name", bitField );
        }
        catch (const NotFound & )
        {
            throw ParseExceptionImpl("could not fine bitfield name in xml file.");
        }

        // call parent method to get actual data. :-)
        return getBits_FromItem(item,
                   getNumberFromXmlAttr(fieldElement, "offset", 0),
                   out,
                   getNumberFromXmlAttr(bitElement, "lsb", 0),
                   getNumberFromXmlAttr(bitElement, "msb", 0)
               );
    }

    void printStructureField( std::ostream &cout, const DOMNode *node, const ISmbiosItem &item )
    {
        std::ios::fmtflags old_opts = cout.flags ();
        try
        {
            unsigned int length = parseLengthStr(safeGetAttribute( node, "length" ));
            string strOffset = safeGetAttribute( node, "offset" );
            unsigned int offset = strtol( strOffset.c_str(), 0, 0 );

            string usage = safeGetAttribute( node, "usage" );
            if (usage == "STRING")
            {
                try
                {
                    cout << getString_FromItem(item, offset);
                }
                catch(const StringUnavailable &)
                {
                }
            }
            else
            {
                cout << hex << "0x";
                for(unsigned int i=0;i<length; i++)
                {
                    cout << setfill('0') << setw(2) << 
                        static_cast<int>(getU8_FromItem(item, offset + length - i - 1));
                }
            }
        }
        catch( const std::exception & )
        {
            cout.flags (old_opts);
            throw;
        }
        cout.flags (old_opts);
    }

    std::ostream &SmbiosTableXml::streamify(ostream & cout) const
    {
        cout << "\nSMBIOS table " << endl;
        cout << "\tversion    : ";
        cout << static_cast<int>(table_header.major_ver) << ".";
        cout << static_cast<int>(table_header.minor_ver) << endl;
        cout << hex ;
        cout << "\taddress    : " << table_header.dmi.table_address << endl;
        cout << dec;
        cout << "\tlength     : " << table_header.dmi.table_length << endl;
        cout << "\tnum structs: " << table_header.dmi.table_num_structs << endl;
        cout << endl;

        SmbiosTable::const_iterator position = begin();
        while (position != end())
        {
             cout << *position << endl;
            ++position;
        }
        return cout;
    }


    /*********************************
      XML OUTPUT FUNCTIONS
      *******************************/

    std::ostream &toXmlString(const ISmbiosTable &table, ostream & cout)
    {
        UNREFERENCED_PARAMETER(table);
        cout << "XML output not yet supported in std lib." << endl;
        return cout;
    }

}
