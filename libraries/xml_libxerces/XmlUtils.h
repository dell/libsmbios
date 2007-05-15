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

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/dom/DOMImplementationLS.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <xercesc/framework/LocalFileFormatTarget.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/Wrapper4InputSource.hpp>

#include "ExceptionImpl.h"

#define X(x)  XMLString::transcode(x)

#define xmlDocGetRootElement(doc) doc->getDocumentElement()
#define SETUP_XML_NAMESPACE XERCES_CPP_NAMESPACE_USE
#define XML_NAMESPACE XERCES_CPP_NAMESPACE_QUALIFIER
#define CHECK_VERSION_COMPAT
#define InitXML XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Initialize
#define FiniXML XERCES_CPP_NAMESPACE_QUALIFIER XMLPlatformUtils::Terminate
#define xmlFreeParser(parser) do{parser->resetDocumentPool(); parser->release();parser=0;}while(0)
#define xmlFreeDoc(doc) do{doc=0;}while(0)

#define compatXmlReadFile(parser, doc, name) do{try{doc = parser->parseURI(name);}catch( const std::exception & ){}}while(0)

                // create SAX input source
                // wrap it with a DOM input source. DOM wrapper adopts the
                // SAX, no need to delete.
#define compatXmlReadMemory(parser, doc, str, len) \
    do{ \
        try \
        {   \
            MemBufInputSource* memBufIs = new MemBufInputSource(    \
                reinterpret_cast<const XMLByte*>(stdXml),   \
                len,    \
                "standard_xml", \
                false );   \
            DOMInputSource* Is = new Wrapper4InputSource( memBufIs );   \
            doc = parser->parse( *Is ); \
            delete Is;  \
        }   \
        catch (const std::exception &)\
        {}  \
    }while(0)

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

    XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *castNode2Element(       XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node );
    const XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *castNode2Element( const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node );

    std::string safeXMLChToString( const XMLCh *src );

    std::string safeGetAttribute( const XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *node, const std::string &attr );

    XERCES_CPP_NAMESPACE_QUALIFIER DOMBuilder *getParser( );

    XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *findElement( XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *root, const std::string elementName, const std::string &attribute, const std::string &value );
    XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *findElementWithNumericAttr( XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *root, const std::string elementName, const std::string &attribute, long value);

    std::string getNodeText( XERCES_CPP_NAMESPACE_QUALIFIER DOMNode *elem );
    int getNumberFromXmlAttr( XERCES_CPP_NAMESPACE_QUALIFIER DOMElement *element, const std::string field, int base );
}

#endif
