# VIM declarations
# vim:tw=0:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=python:

  #############################################################################
  #
  # Copyright (c) 2003 Dell Computer Corporation
  # All Rights Reserved.
  #
  #############################################################################


from __future__ import generators

import types

def getText(nodelist):
    rc = ""
    if nodelist is not None:
        for node in nodelist:
            if node.nodeType == node.TEXT_NODE or node.nodeType == node.CDATA_SECTION_NODE:
                rc = rc + node.data
    return rc

def getNodeText( node, *args ):
    rc = ""
    node = getNodeElement(node, *args)
    if node is not None:
        rc = getText( node.childNodes )
    return rc

def getNodeAttribute(node, attrName, *args ):
    attribute = None
    aNode = getNodeElement(node, *args)
    if aNode is not None:
        attribute = aNode.getAttribute(attrName)
        if attribute == '':
            attribute = None
    return attribute

def setNodeAttributes(node, attrName, attrValue, *args ):
    aNode = getNodeElement(node, *args)
    if aNode is not None:
        aNode.setAttribute(attrName, attrValue)
    return 1


def iterNodeAttribute(node, attrName, *args):
    for aNode in iterNodeElement(node, *args):
        attribute = aNode.getAttribute(attrName)
        if attribute == '':
            attribute = None
        yield attribute
   

def iterNodeElement( node, *args ):
     if len(args) == 0:
        yield node
     elif node is not None:
        for search in node.childNodes:
            if isinstance(args[0], types.StringTypes):
                if search.nodeName == args[0]:
                    for elem in iterNodeElement( search, *args[1:] ):
                        yield elem
            else:
                if search.nodeName == args[0][0]:
                    attrHash = args[0][1]
                    found = 1
                    for (key, value) in attrHash.items():
                        if search.getAttribute( key ) != value:
                            found = 0
                    if found:
                        for elem in iterNodeElement( search, *args[1:] ):
                            yield elem


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



