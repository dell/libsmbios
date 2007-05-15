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

#define LIBSMBIOS_SOURCE
#include "XmlUtils.h"

// no trailing ';' because macro already has one
XERCES_CPP_NAMESPACE_USE

using namespace std;

namespace xmlutils
{
    //
    // NON-MEMBER FUNCTIONS
    //

    // workaround for missing dynamic_cast on windows:
    // xerces not compiled with RTTI on windows
    // use this in place of:
    //      elem = dynamic_cast<DOMElement *>(node);
    DOMElement *castNode2Element( DOMNode *node )
    {
        DOMElement *elem = 0;
        if ( node->getNodeType() == DOMNode::ELEMENT_NODE )
        {
            elem = reinterpret_cast<DOMElement *>(node);
        }
        else
        {
            // workaround for gcc 2.96. Doesn't have bad_cast. :-(
            //throw std::bad_cast();
            throw smbios::Exception<smbios::IException>("could not reinterpret cast element to requested type.");
        }
        return elem;
    }

    // const version of function above.
    const DOMElement *castNode2Element( const DOMNode *node )
    {
        const DOMElement *elem = 0;
        if ( node->getNodeType() == DOMNode::ELEMENT_NODE )
        {
            elem = reinterpret_cast<const DOMElement *>(node);
        }
        else
        {
            // workaround for gcc 2.96. Doesn't have bad_cast. :-(
            //throw std::bad_cast();
            throw smbios::Exception<smbios::IException>("could not reinterpret cast element to requested type.");
        }
        return elem;
    }

    // even better
    string safeXMLChToString( const XMLCh *src )
    {
        string dest = "";
        if( src )
        {
            const char *temp = XMLString::transcode( src );
            dest = temp;
            delete [] const_cast<char *>(temp);
        }
        return dest;
    }

    // the best. use this when possible.
    string safeGetAttribute( const DOMNode *node, const string &attr )
    {
        const DOMElement *elem = castNode2Element(node);

        // extract type information
        XMLCh *attrName = X(attr.c_str()); // NEED TO 'release' !!!
        const XMLCh *attrValue = elem->getAttribute( attrName );
        XMLString::release(&attrName); // released.

        // return the type as an INT.
        return  safeXMLChToString( attrValue );
    }

    DOMBuilder *getParser( )
    {
        static const XMLCh gLS[] =
            {
                chLatin_L, chLatin_S, chNull
            };
        DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(gLS);
        DOMBuilder        *parser = (static_cast<DOMImplementationLS*>(impl))->createDOMBuilder(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
        parser->setFeature( XMLUni::fgDOMNamespaces, false );
        parser->setFeature( XMLUni::fgXercesSchema, false );
        parser->setFeature( XMLUni::fgXercesSchemaFullChecking, false );

        parser->resetDocumentPool();

        return parser;
    }

    //
    // Finds a "STRUCTURE" element with the specified attribute and returns
    // a pointer to it.
    //
    DOMElement *findElement( DOMElement *root, const string elementName, const string &attribute, const string &value )
    {
        DOMElement *elem = 0;

        // If we don't have a ref to XML file, we cannot find this info
        if( ! root )
            throw NotFoundImpl("no root element ref to xml file, cannot findElement");

        XMLCh *tagName = X(elementName.c_str()); // NEED TO 'release' !!!
        DOMNodeList *structureList = root->getElementsByTagName(tagName);
        XMLString::release(&tagName);

        if( !structureList )
            throw NotFoundImpl("could not find element.");

        int length = structureList->getLength();
        for( int index = 0; (index < length) && !elem ; ++index )
        {
            DOMNode *node = structureList->item( index );
            if( node->getNodeType() == DOMNode::ELEMENT_NODE )
            {
                string strAttrValue = safeGetAttribute( node, attribute );
                if( (strAttrValue == value) || (attribute == "") )
                {
                    elem = castNode2Element( node );
                }
            }
        }

        if( ! elem )
            throw NotFoundImpl("could not find element.");

        return elem;
    }

    //
    // Finds a "STRUCTURE" element with the specified attribute and returns
    // a pointer to it.
    //
    DOMElement *findElementWithNumericAttr( DOMElement *root, const string elementName, const string &attribute, long value)
    {
        DOMElement *elem = 0;

        // If we don't have a ref to XML file, we cannot find this info
        if( ! root )
            throw NotFoundImpl("no root element ref to xml file, cannot findElement");

        XMLCh *tagName = X(elementName.c_str()); // NEED TO 'release' !!!
        DOMNodeList *structureList = root->getElementsByTagName(tagName);
        XMLString::release(&tagName);

        if( !structureList )
            throw NotFoundImpl("could not find element.");

        int length = structureList->getLength();
        for( int index = 0; (index < length) && !elem ; ++index )
        {
            DOMNode *node = structureList->item( index );
            if( node->getNodeType() == DOMNode::ELEMENT_NODE )
            {
                string strAttrValue = safeGetAttribute( node, attribute );
                char *endptr = 0;
                long attrValue = strtol(strAttrValue.c_str(), &endptr, 0);
                if(endptr == strAttrValue.c_str()) continue;
                if((attrValue == value) || (attribute == "") )
                {
                    elem = castNode2Element( node );
                }
            }
        }

        if( ! elem )
            throw NotFoundImpl("could not find element.");

        return elem;
    }

    string getNodeText( DOMNode *elem )
    {
        string retval = "";

        DOMNodeList *children = elem->getChildNodes ();

        int length = children->getLength();
        for( int index = 0; index < length; ++index )
        {
            DOMNode *node = children->item( index );
            if( node->getNodeType() == DOMNode::TEXT_NODE  )
            {
                DOMText *txt = reinterpret_cast<DOMText *>(node);
                const XMLCh *src = txt->getNodeValue();
                retval += safeXMLChToString(src);
            }
        }
        return retval;
    }

    int getNumberFromXmlAttr( DOMElement *element, const string field, int base )
    {
        int tempNum = 0;
        string tempStr = safeGetAttribute( element, field );
        if(tempStr.length() != 0)
            tempNum = strtol( tempStr.c_str(), 0, base);

        return tempNum;
    }
}
