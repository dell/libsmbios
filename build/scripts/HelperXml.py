#!/usr/bin/python
# VIM declarations
# vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=python:

  #############################################################################
  #
  # Copyright (c) 2003 Dell Computer Corporation
  # All Rights Reserved.
  #
  #############################################################################
"""
$Id: HelperXml.py,v 1.1 2004/03/14 06:09:08 michael_e_brown Exp $
"""

__version__ = "$Revision: 1.1 $"
# $Source: /home/cvsroot-hb/cvsroot/libsmbios/build/scripts/HelperXml.py,v $

import types

def getText(nodelist):
    rc = ""
    if nodelist is not None:
        for node in nodelist:
            if node.nodeType == node.TEXT_NODE:
                rc = rc + node.data
    return rc

def getNodeText( node, *args ):
    rc = ""
    node = getNodeElement(node, *args)
    if node is not None:
        rc = getText( node.childNodes )
    return rc

def getNodeElement( node, *args ):
    if len(args) == 0:
        return node

    #print "DEBUG: args(%s)" % repr(args)
    if node is not None:
        for search in node.childNodes:
            if isinstance(args[0], types.StringTypes):
                if search.nodeName == args[0]:
                    candidate = getNodeElement( search, *args[1:] )
                    if candidate is not None:
                        return candidate
            else:
                if search.nodeName == args[0][0]:
                    attrHash = args[0][1]
                    found = 1
                    for (key, value) in attrHash.items():
                        if search.getAttribute( key ) != value:
                            found = 0
                    if found:
                        candidate = getNodeElement( search, *args[1:] )
                        if candidate is not None:
                            return candidate

    return None



