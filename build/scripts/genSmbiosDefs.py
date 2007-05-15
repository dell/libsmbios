#!/usr/bin/python
# vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=python:


import sys
import xml.dom.minidom

import HelperXml

def main():
    try:
        doc = xml.dom.minidom.parse( sys.argv[1]  )
        elem = HelperXml.getNodeElement( doc, "STRUCTUREDEFS" )
        for node in elem.childNodes:
            if node.nodeName == "STRUCTURE":
                print node.getAttribute("description").replace(" ", "_"), " = ", 
                print node.getAttribute( "type" ), ","
    except (IOError, KeyError):
        pass


if __name__ == "__main__":
        main()
