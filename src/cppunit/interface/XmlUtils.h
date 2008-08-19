// vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=c:cindent:
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

#ifndef XMLUTILS_H
#define XMLUTILS_H

// compat header should always be first header
#include "smbios/compat.h"

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#include "ExceptionImpl.h"

// xerces compat stuff until transition is complete
#define XERCES_CPP_NAMESPACE_QUALIFIER
#define DOMElement xmlNode
#define DOMNode    xmlNode
#define DOMDocument xmlDoc
#define DOMBuilder int
#define SETUP_XML_NAMESPACE
#define XML_NAMESPACE
#define CHECK_VERSION_COMPAT LIBXML_TEST_VERSION
#define xmlDocGetRootElement(doc) xmlDocGetRootElement(const_cast<xmlDocPtr>(doc))
#define InitXML()  \
    do {        \
    } while(0)
#define FiniXML() do{}while(0)
#define xmlFreeParser(parser) do{parser=0;}while(0)
#define xmlFreeDoc(doc) do{xmlFreeDoc(doc); doc=0;}while(0)

        /*
XML_PARSE_RECOVER = 1 : recover on errors
XML_PARSE_NOENT = 2 : substitute entities
XML_PARSE_DTDLOAD = 4 : load the external subset
XML_PARSE_DTDATTR = 8 : default DTD attributes
XML_PARSE_DTDVALID = 16 : validate with the DTD
XML_PARSE_NOERROR = 32 : suppress error reports
XML_PARSE_NOWARNING = 64 : suppress warning reports
XML_PARSE_PEDANTIC = 128 : pedantic error reporting
XML_PARSE_NOBLANKS = 256 : remove blank nodes
XML_PARSE_SAX1 = 512 : use the SAX1 interface internally
XML_PARSE_XINCLUDE = 1024 : Implement XInclude substitition
XML_PARSE_NONET = 2048 : Forbid network access
XML_PARSE_NODICT = 4096 : Do not reuse the context dictionnary
XML_PARSE_NSCLEAN = 8192 : remove redundant namespaces declarations
XML_PARSE_NOCDATA = 16384 : merge CDATA as text nodes
XML_PARSE_NOXINCNODE = 32768 : do not generate XINCLUDE START/END nodes
XML_PARSE_COMPACT = 65536 : compact small text nodes
           */

// We program to the 2.6 API. Here are some backwards compat stuff
#if LIBXML_VERSION < 20600

#undef InitXML
#define InitXML() xmlSetGenericErrorFunc(NULL, xmlutils::suppressLibxmlWarnings);
namespace xmlutils
{
    void  suppressLibxmlWarnings (void *ctx, const char *msg, ...);
}

#  define compatXmlReadFile(parser, doc, name)  do{UNREFERENCED_PARAMETER(parser); doc = xmlParseFile(name);}while(0)
#  define compatXmlReadMemory(parser, doc, ptr, len) do{UNREFERENCED_PARAMETER(parser); doc = xmlParseMemory(ptr, len);}while(0)

#else

#  define compatXmlReadFile(parser, doc, name)  do{UNREFERENCED_PARAMETER(parser); doc = xmlReadFile(name, \
        NULL,     \
        XML_PARSE_RECOVER | XML_PARSE_NOENT |   XML_PARSE_DTDATTR |     XML_PARSE_NOWARNING |   XML_PARSE_NONET |   XML_PARSE_NOCDATA \
                );}while(0)
#  define compatXmlReadMemory(parser, doc, buf, len)  do{UNREFERENCED_PARAMETER(parser);doc = xmlReadMemory(\
        buf,      \
        len,      \
        NULL,     \
        NULL,     \
        XML_PARSE_RECOVER | \
        XML_PARSE_NOENT |   \
        XML_PARSE_DTDATTR |     \
        XML_PARSE_NOWARNING |   \
        XML_PARSE_NONET |   \
        XML_PARSE_NOCDATA \
                );}while(0)
#endif


namespace xmlutils
{

    // declare exceptions
    //    Internal users should catch() these...
    DECLARE_EXCEPTION( XmlUtilsException );
    DECLARE_EXCEPTION_EX( NotFound, xmlutils, XmlUtilsException );
    DECLARE_EXCEPTION_EX( Invalid, xmlutils, XmlUtilsException );

    // Since this is also a private header, define them
    //  internal use only inside XmlUtils.cpp
    DEFINE_EXCEPTION_EX( NotFoundImpl, xmlutils, NotFound );
    DEFINE_EXCEPTION_EX( InvalidImpl, xmlutils, Invalid );

    std::string safeGetAttribute( const xmlNode *node, const std::string &attr );
    DOMBuilder *getParser( );

    xmlNodePtr findElement( xmlNodePtr root, const std::string elementName, const std::string &attribute, const std::string &value );
    xmlNodePtr findElementWithNumericAttr( xmlNodePtr root, const std::string elementName, const std::string &attribute, long value);

    std::string getNodeText( xmlNodePtr elem );
    int getNumberFromXmlAttr( xmlNodePtr element, const std::string field, int base );
}

#endif
