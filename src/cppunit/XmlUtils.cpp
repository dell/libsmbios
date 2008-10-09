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

#if defined(DEBUG_XMLUTILS)
#define DEBUG_OUTPUT_ALL
#endif

#define LIBSMBIOS_SOURCE
#include "XmlUtils.h"

using namespace std;

namespace xmlutils
{
    //
    // NON-MEMBER FUNCTIONS
    //
    DOMBuilder *getParser() { return 0; }

    // backwards compat for libxml < 2.6 without xmlReadFile()
    void  suppressLibxmlWarnings (void *ctx, const char *msg, ...)
    { 
        UNREFERENCED_PARAMETER(ctx); 
        UNREFERENCED_PARAMETER(msg); 
    }

    // the best. use this when possible.
    string safeGetAttribute( const xmlNode *node, const string &attr )
    {
        string retval("");
        xmlChar *text = xmlGetProp(const_cast<xmlNode *>(node), reinterpret_cast<const xmlChar *>(attr.c_str()));
        if (text)
            retval = reinterpret_cast<const char *>(text);
        xmlFree(text);
        return retval;
    }

    //
    // Finds a "STRUCTURE" element with the specified attribute and returns
    // a pointer to it.
    //
    xmlNodePtr findElement( xmlNodePtr root, const string elementName, const string &attribute, const string &value )
    {
        xmlNodePtr elem = 0;
        DCERR("findElement( root, " << "\"" << elementName << "\", \"" << attribute << "\", \"" << value << "\");" << endl);

        // If we don't have a ref to XML file, we cannot find this info
        if( ! root )
            throw NotFoundImpl("no root element ref to xml file, cannot findElement");

        xmlNodePtr cur_node = NULL;
        for (cur_node = root; cur_node; cur_node = cur_node->next) {
            DCERR("\tnode type: Element, name: " << cur_node->name << endl);
            if (cur_node->type == XML_ELEMENT_NODE) {
                if (!xmlStrcmp(cur_node->name, reinterpret_cast<const xmlChar *>(elementName.c_str())))
                {
                    string strAttrValue = safeGetAttribute( cur_node, attribute );
                    DCERR("\tELEMENT attribute ("<< attribute <<") value: " << "\"" << strAttrValue << "\"" << endl);
                    if( (strAttrValue == value) || (attribute == "") )
                    {
                        DCERR("MATCH!" << endl);
                        elem = cur_node;
                        goto out;
                    }
                }
            }
            try
            {
                DCERR("\tsearching child: " << cur_node->name << endl);
                elem = findElement(cur_node->children, elementName, attribute, value);
                goto out;
            }
            catch (NotFound) 
            {
            // not an error yet
                DCERR("\tDid not find match in child: " << cur_node->name << endl);
            } 
        }

out:
        if (!elem)
        {
            DCERR("Throwing not found error!"<< endl);
            throw NotFoundImpl("could not find element.");
        }

        return elem;
    }


    //
    // Finds a "STRUCTURE" element with the specified attribute and returns
    // a pointer to it.
    //
    xmlNodePtr findElement( xmlNodePtr root, const string elementName, const string &attribute, long value)
    {
        xmlNodePtr elem = 0;

        DCERR("findElement( root, " << "\"" << elementName << "\", \"" << attribute << "\", \"" << value << "\");" << endl);

        // If we don't have a ref to XML file, we cannot find this info
        if( ! root )
            throw NotFoundImpl("no root element ref to xml file, cannot findElement");

        xmlNodePtr cur_node = NULL;
        for (cur_node = root; cur_node; cur_node = cur_node->next) {
            DCERR("\tnode type: Element, name: " << cur_node->name << endl);
            if (cur_node->type == XML_ELEMENT_NODE) {
                if (!xmlStrcmp(cur_node->name, reinterpret_cast<const xmlChar *>(elementName.c_str())))
                {
                    // printf("node type: Element, name: %s\n", cur_node->name);
                    string strAttrValue = safeGetAttribute( cur_node, attribute );
                    char *endptr = 0;
                    long attrValue = strtol(strAttrValue.c_str(), &endptr, 0);
                    DCERR("\tELEMENT attribute ("<< attribute <<") value: " << "\"" << strAttrValue << "\"" << endl);
                    if(endptr != strAttrValue.c_str())
                        if( (attrValue == value) || (attribute == "") )
                        {
                            DCERR("MATCH!" << endl);
                            elem = cur_node;
                            goto out;
                        }
                }
            }
            try
            {
                DCERR("\tsearching child: " << cur_node->name << endl);
                elem = findElement(cur_node->children, elementName, attribute, value);
                goto out;
            }
            catch (NotFound) {} // not an error yet
        }

out:
        if (!elem)
        {
            DCERR("Throwing not found error!"<< endl);
            throw NotFoundImpl("could not find element.");
        }

        return elem;
    }

    xmlNodePtr findElementWithNumericAttr( xmlNodePtr root, const string elementName, const string &attribute, long value)
    {
        return findElement(root, elementName, attribute, value);
    }

    string getNodeText( xmlNodePtr elem )
    {
        string retval = "";
        xmlChar *text = 0;
        text = xmlNodeGetContent(elem);
        retval = reinterpret_cast<const char *>(text);
        xmlFree(text);
        return retval;
    }


    int getNumberFromXmlAttr( xmlNodePtr element, const string field, int base )
    {
        int tempNum = 0;
        string tempStr = safeGetAttribute( element, field );
        if(tempStr.length() != 0)
            tempNum = strtol( tempStr.c_str(), 0, base);

        return tempNum;
    }
}
