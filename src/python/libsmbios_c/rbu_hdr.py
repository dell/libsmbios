# vim:tw=0:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=python:

  #############################################################################
  #
  # Copyright (c) 2005 Dell Computer Corporation
  # Dual Licenced under GNU GPL and OSL
  #
  #############################################################################
"""
token:
    python interface to rbu hdr files
"""

# imports (alphabetical)
import ctypes
import exceptions
import struct

from libsmbios_c import libsmbios_c_DLL as DLL
from _common import errorOnNullPtrFN, errorOnNegativeFN, freeLibStringFN
from trace_decorator import decorate, traceLog, getLog

# use python-decoratortools if it is installed, otherwise use our own local
# copy. Imported this locally because it doesnt appear to be available on SUSE
# and the fedora RPM doesnt appear to compile cleanly on SUSE
try:
    from peak.util.decorators import decorate_class
except ImportError:
    from libsmbios_c._peak_util_decorators import decorate_class

class InvalidRbuHdr(Exception): pass

decorate(traceLog())
def dumpHdrFileInfo(hdr):
    print _("BIOS HDR file information dump.")
    print
    print _("Filename: %s") % hdr.filename
    print _("File magic header: %s") % hdr.hdr.headerId
    print _("Header length: %s") % hdr.hdr.headerLength
    print _("Header major version: %s") % hdr.hdr.headerMajorVer
    print _("Header minor version: %s") % hdr.hdr.headerMinorVer
    print _("Number of supported systems: %s") % hdr.hdr.numSystems
    print _("Quick check field: %s") % hdr.hdr.quickCheck
    print _("BIOS Version (RAW): %s") % hdr.hdr.biosVersion
    print _("BIOS Version: %s") % hdr.biosVersion()
    print _("Misc flags: %s") % hdr.hdr.miscFlags
    #print "biosInternalOnly: %s" % hdr.hdr.biosInternalOnly
    #for i in hdr.hdr.reserved:
        #print "reserved: 0x%04x" % i
    print _("Compatibility flags: %s") % hdr.hdr.compatFlags
    for id, hwrev in hdr.systemIds():
        print _("System ID: 0x%04x  Hardware Revision: %d") % (id, hwrev)

class HdrFile(object):
    decorate(traceLog())
    def __init__(self, filename):
        self.filename = filename
        self.fd = open(filename, "rb")
        self.hdr = HdrFileStructure()
        buf = self.fd.read(ctypes.sizeof(self.hdr))
        self.fd.seek(0,0)
        ctypes.memmove(ctypes.addressof(self.hdr), buf, min(len(buf), ctypes.sizeof(self.hdr)))

        if self.hdr.headerId != "$RBU":
            raise InvalidRbuHdr("Not a valid RBU HDR File. Header doesnt have '$RBU' header.")

        self.sysidlist = []
        for i in range(min(NUM_SYS_ID_IN_HDR, self.hdr.numSystems)):
            f = self.hdr.systemIdList[i]
            # see note in struct definition for the bit manipulations here
            self.sysidlist.append(
                  (
                    (f & 0xFF) | ((f & 0xF800) >> 3), # system id
                    (f & 700) >> 8                    # hw rev
                  )
                )

    decorate(traceLog())
    def systemIds(self):
        for i in self.sysidlist:
            yield i

    decorate(traceLog())
    def biosVersion(self):
        ver = ""
        if self.hdr.headerMajorVer < 2:
            ver = "".join([ c for c in self.hdr.biosVersion if c.isalnum() ])
        else:
            ver = "%d.%d.%d" % struct.unpack("BBB", self.hdr.biosVersion)
        return ver


# current HDR def leaves room for 12 system ids
NUM_SYS_ID_IN_HDR = 12

class HdrFileStructure(ctypes.Structure):
    _pack_ = 1
    _fields_ = [
        #char headerId[4];
        ("headerId", ctypes.c_char * 4),
        #u8  headerLength;
        ("headerLength", ctypes.c_uint8),
        #u8  headerMajorVer;
        ("headerMajorVer", ctypes.c_uint8),
        #u8  headerMinorVer;
        ("headerMinorVer", ctypes.c_uint8),
        #u8  numSystems;
        ("numSystems", ctypes.c_uint8),
        #char quickCheck[40];
        ("quickCheck", ctypes.c_char * 40),
        #char biosVersion[3];
        ("biosVersion", ctypes.c_char * 3),
        #u8  miscFlags;
        ("miscFlags", ctypes.c_uint8),
        #u8  biosInternalUse;
        ("biosInternalOnly", ctypes.c_uint8),
        #u8  reserved[5];
        ("reserved", ctypes.c_uint8 * 5),
        #u16 compatFlags;
        ("compatFlags", ctypes.c_uint16),
        #u16 systemIdList[NUM_SYS_ID_IN_HDR];
        ("systemIdList", ctypes.c_uint16 * NUM_SYS_ID_IN_HDR)

# Contains the list of NumSystems Dell System ID and Hardware Revision
# ID pairs for which the Image Data is valid, in the following format:
#
# Bit Range  Description
# 15:11      Dell System ID, bits 12:8.
#               This range is set to 00000b if the Dell System ID
#               is a 1-byte value.
# 10:8       Hardware Revision ID
# 7:0        Dell System ID, bits 7:0.
        ]





