# vim:tw=0:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=python:

  #############################################################################
  #
  # Copyright (c) 2005 Dell Computer Corporation
  # Dual Licenced under GNU GPL and OSL
  #
  #############################################################################
"""
rbu_update:
    python interface to rbu bios update functions
"""

# imports (alphabetical)
import ctypes
import exceptions
import struct
import os
import random
import time

from libsmbios_c import libsmbios_c_DLL as DLL
from _common import errorOnNullPtrFN, errorOnNegativeFN, freeLibStringFN
from libsmbios_c import system_info as sysinfo, smbios, token, localedir, GETTEXT_PACKAGE
from trace_decorator import decorate, traceLog, getLog

# use python-decoratortools if it is installed, otherwise use our own local
# copy. Imported this locally because it doesnt appear to be available on SUSE
# and the fedora RPM doesnt appear to compile cleanly on SUSE
try:
    from peak.util.decorators import decorate_class
except ImportError:
    from libsmbios_c._peak_util_decorators import decorate_class

moduleLog = getLog()
verboseLog = getLog(prefix="verbose.")

class InappropriateHDRFile(Exception): pass

RBU_SMBIOS_STRUCT = 0xDE
TOKEN_RBU_CANCEL = 0x005d
TOKEN_RBU_STAGE  = 0x005c

completion_messages = {
    0x0000: _("The update was completed successfully."),
    0x0001: _("The image failed one or more consistency checks."),
    0x0002: _("The BIOS could not access the flash-memory device."),
    0x0003: _("The flash-memory device was not ready when an erase was attempted."),
    0x0004: _("Flash programming is currently disabled on the system, or the voltage is low."),
    0x0005: _("A battery must be installed for the operation to complete."),
    0x0006: _("A fully-charged battery must be present for the operation to complete."),
    0x0007: _("An external power adapter must be connected for the operation to complete."),
    0x0008: _("The 12V required to program the flash-memory could not be set."),
    0x0009: _("The 12V required to program the flash-memory could not be removed."),
    0x000A: _("A flash-memory failure occurred during a block-erase operation."),
    0x000B: _("A general failure occurred during the flash programming."),
    0x000C: _("A data miscompare error occurred during the flash programming."),
    0x000D: _("The image could not be found in memory, i.e. the header could not be located."),
    0xFFFF: _("No update operation has been performed on the system."),
}


decorate(traceLog())
def getCompletion(struct_0xDE=None):
    if struct_0xDE is None:
        struct_0xDE = smbios.SmbiosTable()[RBU_SMBIOS_STRUCT]
    return (_getNum(struct_0xDE, 0x06, 2), completion_messages[_getNum(struct_0xDE, 0x06, 2)])

decorate(traceLog())
def getRbuLastUpdate(struct_0xDE=None):
    if struct_0xDE is None:
        struct_0xDE = smbios.SmbiosTable()[RBU_SMBIOS_STRUCT]
    return {
        "year": _getNum(struct_0xDE, 0x08, 1),
        "month": _getNum(struct_0xDE, 0x09, 1),
        "day": _getNum(struct_0xDE, 0x0a, 1),
        "hour": _getNum(struct_0xDE, 0x0b, 1),
        "minute": _getNum(struct_0xDE, 0x0c, 1),
        }

decorate(traceLog())
def updateBios(hdrfile, options, testMode=False):
    meth = BaseRbu.getRbuMethod(hdrfile, options.update_mode)
    if options.check_sysid:
        id = sysinfo.get_dell_system_id()
        if id not in [i[0] for i in hdrfile.systemIds()]:
            raise InappropriateHDRFile( _("The .HDR file does not list this system (0x%04x) as a supported system. The supported systems are: %s") % (id, ["0x%04x" % x for x in hdrfile.systemIds()]) )

    if options.check_bios_version:
        ver = sysinfo.get_bios_version()
        if compareBiosVersions(ver, hdrfile.biosVersion()) >= 0:
            raise InappropriateHDRFile( _("The system bios version (%s) is the same as or newer than the .HDR file (%s).") % (ver, hdrfile.biosVersion()) )

    exit_code = 0
    if not testMode:
        exit_code = meth.doUpdate(options)

    return exit_code

decorate(traceLog())
def cancelUpdate():
    table = token.TokenTable()
    table[TOKEN_RBU_CANCEL].activate()
    meth = BaseRbu.getRbuMethod(None)
    meth.cancelUpdate()

decorate(traceLog())
def getUpdateModes(struct_0xDE=None):
    if struct_0xDE is None:
        struct_0xDE = smbios.SmbiosTable()[RBU_SMBIOS_STRUCT]

    modes = [ "mono" ]
    characteristics = _getNum(struct_0xDE, 0x0f, 1)
    if characteristics & 0x01:
        modes.append("packet")
    return modes


firstSpecialVer = 90
decorate(traceLog())
def compareBiosVersions(latest, toTest):
    latest = latest.lower()
    toTest = toTest.lower()

    if latest == toTest:
        return 0

    #some broken bios were leaked with bad version
    # never let 'unknown' version override good version
    if toTest == "unknown" or toTest == "49.0.48":
        return 1

    if latest == "unknown" or latest == "49.0.48":
        return -1

    # old style bios versioning ("Ann", eg. "A01"...)
    if "." not in latest and "." not in toTest:
        # anything non "Ann" version is "special" and should never win a
        # ver comparison unless 'latest' is also "special"
        if not toTest.lower().startswith("a") and latest.lower().startswith("a"):
            return 1
        elif toTest.lower().startswith("a") and not latest.lower().startswith("a"):
            return -1

        if toTest > latest:
            return -1
        else:
            return 1

    # only get here if one or other has new-style bios versioning

    # new style bios overrides old style...
    if "." not in latest:
        return -1
    if "." not in toTest:
        return 1

    # both new style, compare major/minor/build individually
    latestArr = latest.split(".")
    toTestArr = toTest.split(".")

    # versions 90-99 are "special" and should never win a ver comparison
    # unless 'latest' is also "special"
    try:
        if int(toTestArr[0]) >= firstSpecialVer and int(latestArr[0]) < firstSpecialVer:
            return 1
        if int(latestArr[0]) >= firstSpecialVer and int(toTestArr[0]) < firstSpecialVer:
            return -1
    except ValueError: # non-numeric version ?
        pass

    for i in xrange(0, len(latestArr)):
        # test array shorter than latest,
        if i >= len(toTestArr):
            return 1
        try:
            if int(toTestArr[i]) > int(latestArr[i]):
                return -1
            if int(toTestArr[i]) < int(latestArr[i]):
                return 1
        except ValueError:  # non-numeric version?
            pass #punt...

    # if we get here, everything is equal (so far)
    if len(toTestArr) > len(latestArr):
        return -1

    return 1



class BaseRbu(object):
    subclasses = []
    decorate(traceLog())
    def __init__(self, hdrfile):
        self.hdrfile = hdrfile

    decorate(staticmethod)
    decorate(traceLog())
    def getRbuMethod(hdrfile, forcetype="auto"):
        meths = {}
        for i in BaseRbu.subclasses:
            meths[i] = i.validRbuMethod(hdrfile, forcetype)

        retval = (None, 0)
        for cls in meths.keys():
            if meths[cls] > retval[1]:
                retval = (cls, meths[cls])

        if retval[0] is not None:
            return retval[0](hdrfile)
        return None

# use this class decorator on subclasses
def BaseRbuSubclass():
    def decorator(cls):
        BaseRbu.subclasses.append(cls)
        return cls
    decorate_class(decorator)

class BackCompatRbu(BaseRbu):
    BaseRbuSubclass()
    # this class calls the old dellBiosUpdate binary
    # to handle 2.4 kernels and old pre-integrated rbu driver

    decorate(classmethod)
    decorate(traceLog())
    def validRbuMethod(cls, hdrfile=None, forcetype="auto"):
        # should always work:
        return 1

    decorate(traceLog())
    def doUpdate(self, options):
        verboseLog.info( _("Performing BIOS update in using old dellBiosUpdate executable.") )

        paths = os.environ.get("PATH").split(":")
        if "/usr/sbin" not in paths:
            paths.append("/usr/sbin")
        os.environ["PATH"] = ":".join(paths)

        cmdline = ["dellBiosUpdate-compat", "-u", "-f", self.hdrfile.filename]
        if not options.check_bios_version:
            cmdline.append( "--override_bios_version" )
        if not options.check_sysid:
            cmdline.append( "--override_sysid" )
        if options.update_mode == "mono":
            cmdline.append( "--force_mono" )
        if options.update_mode == "packet":
            cmdline.append( "--force_packet" )
        ret = os.system(" ".join(cmdline))
        return os.WEXITSTATUS(ret)

    decorate(traceLog())
    def cancelUpdate(self):
        verboseLog.info( _("Cancelling BIOS update using old dellBiosUpdate executable."))
        os.system("dellBiosUpdate -c")

class MonolithicRbu(BaseRbu):
    BaseRbuSubclass()
    sysbasedir = "/sys/devices/platform/dell_rbu/"
    img_type_file = "/sys/devices/platform/dell_rbu/image_type"
    pkt_size_file = "/sys/devices/platform/dell_rbu/packet_size"
    fw_data_file = "/sys/class/firmware/dell_rbu/data"
    fw_load_file = "/sys/class/firmware/dell_rbu/loading"

    method = "mono"
    validtypes = ["auto", "mono"]
    methodWeight = 10

    decorate(classmethod)
    decorate(traceLog())
    def validRbuMethod(cls, hdrfile=None, forcetype="auto"):
        verboseLog.debug("Class %s, forcetype(%s) validtypes(%s)" % (cls, forcetype, cls.validtypes))
        if not forcetype in cls.validtypes:
            verboseLog.debug("\tforcetype not in validtypes")
            return 0
        if not os.path.exists(cls.img_type_file):
            verboseLog.debug("\timg_type_file doesnt exist.")
            return 0
        if forcetype == "auto" and cls.method not in getUpdateModes():
            verboseLog.debug("\tcls.method(%s) not in getUpdateModes(%s)", (cls.method, getUpdateModes()))
            return 0
        verboseLog.debug("\treturning weight %d for %s" % (cls.methodWeight, cls))
        return cls.methodWeight

    decorate(traceLog())
    def cancelUpdate(self):
        self.setPacketType('init')
        self.setLoadValue('0')

    decorate(traceLog())
    def doUpdate(self, options):
        verboseLog.info( _("Performing BIOS update in monolithic mode.") )
        self.setPacketType('init')
        self.setPacketType('mono')
        self.setLoadValue('1')
        outputFd = open(self.fw_data_file, "wb")
        _streamDataToFile(self.hdrfile.fd, outputFd)
        outputFd.close()
        self.setLoadValue('0')
        table = token.TokenTable()
        table[TOKEN_RBU_STAGE].activate()
        return 0

    decorate(traceLog())
    def setSize(self, val):
        _dumpDataToFile(self.pkt_size_file, str(val), wait=False)

    decorate(traceLog())
    def setLoadValue(self, val):
        _dumpDataToFile(self.fw_load_file, val, wait=True)

    decorate(traceLog())
    def setPacketType(self, val):
        _dumpDataToFile(self.img_type_file, val, wait=False)

class PacketRbu(MonolithicRbu):
    BaseRbuSubclass()

    method = "packet"
    validtypes = ["auto", "packet"]
    methodWeight = 20
    packetSize = 4096

    decorate(traceLog())
    def doUpdate(self, options):
        verboseLog.info( _("Performing BIOS update in packet mode.") )
        self.setPacketType('init')
        self.setPacketType('packet')
        self.setSize(0)
        self.setSize(self.packetSize)
        self.setLoadValue('1')
        self.pktUpdateLoop()
        self.setLoadValue('0')
        table = token.TokenTable()
        table[TOKEN_RBU_STAGE].activate()
        return 0

    decorate(traceLog())
    def pktUpdateLoop(self):
        outfd = open(self.fw_data_file, "w+")

        imageSize = _getFileSize( self.hdrfile.fd )
        pktHeader = RbuPacketHeader(self.packetSize, imageSize)
        pktHeader.setBuf( "" )
        pktHeader.writeTo(outfd)

        while 1:
            buf = self.hdrfile.fd.read(pktHeader.getPayloadSize())
            if buf == "":
                break
            pktHeader.incrementPktNum()
            pktHeader.setBuf(buf)
            pktHeader.writeTo(outfd)

        outfd.close()


decorate(traceLog())
def _getFileSize(fd):
    curPos = fd.tell()
    fd.seek(0,2)
    size = fd.tell()
    fd.seek(curPos, 0)
    return size


decorate(traceLog())
def _dumpDataToFile(filename, data, wait=False):
    while wait and not os.path.exists(filename):
        time.sleep(0.1)
    fh = open(filename, "wb")
    fh.write(data)
    fh.close()

decorate(traceLog())
def _streamDataToFile(inputStream, outputStream, bufsize=1024):
    while 1:
        buf = inputStream.read(bufsize)
        if buf == "": break
        outputStream.write(buf)

decorate(traceLog())
def _getNum(s, off, len):
    retval = 0
    try:
        t = list(struct.unpack( 'B' * len, s.getData(off, len) ))
        t.reverse()
        for i in t:
            retval = (retval << 8) | i
    except IndexError, e:
        pass
    return retval






class RbuPacketHeader(ctypes.Structure):
    _pack_ = 1
    _fields_ = [
        #u32 pktId;      # must be '$RPK'
        ("pktId", ctypes.c_uint32),
        #u16 pktSize;    # size of packet in KB
        ("pktSizeKb", ctypes.c_uint16),
        #u16 reserved1;  #
        ("reserved1", ctypes.c_uint16),
        #u16 hdrSize;    # size of packet header in paragraphs (16 byte chunks)
        ("hdrSizePg", ctypes.c_uint16),
        #u16  reserved2; #
        ("reserved2", ctypes.c_uint16),
        #u32 pktSetId;   # unique id for packet set, can be anything
        ("pktSetId", ctypes.c_uint32),
        #u16 pktNum;     # sequential pkt number (only thing that changes)
        ("pktNum", ctypes.c_uint16),
        #u16 totPkts;    # total number of packets
        ("totalPkts", ctypes.c_uint16),
        #u8  pktVer;     # version == 1 for now
        ("pktVer", ctypes.c_uint8),
        #u8  reserved[9];
        ("reserved3", ctypes.c_uint8 * 9),
        #u16 pktChksum;  # sum all bytes in pkt must be zero
        ("pktChksum", ctypes.c_uint16),
        #u8  pktData;  # Start of packet data.
        ]

    decorate(traceLog())
    def __init__(self, packetSize, imageSize):
        self.packetSize = packetSize
        self.pktId = 0x4B505224;  # 2452504B;   # must be '$RPK'
        self.pktSizeKb = packetSize // 1024
        self.hdrSizePg = 2  # hdr is 32 bytes
        self.reserved2 = 0;
        self.pktSetId = random.randint(0, 0xFFFFFFFF) # 0x12345678;
        self.pktNum = 0
        self.pktVer = 1  # hardcoded to rbu version we support
        self.pktChksum = 0
        # +1 takes into account packet 0 which is a header of sorts
        self.totalPkts = imageSize // self.getPayloadSize() + 1
        if imageSize % self.getPayloadSize():
            self.totalPkts = self.totalPkts + 1

    decorate(traceLog())
    def setBuf(self, buf):
        self.buf = buf
        self.checksum()

    decorate(traceLog())
    def incrementPktNum(self):
        self.pktNum = self.pktNum + 1

    decorate(traceLog())
    def getPayloadSize(self):
        return self.packetSize - ctypes.sizeof(self)

    decorate(traceLog())
    def _makeBuf(self):
        rawbuf = ctypes.create_string_buffer(self.packetSize)
        ctypes.memmove(rawbuf, ctypes.byref(self), ctypes.sizeof(self))
        ctypes.memmove(
            ptr_add(ctypes.pointer(rawbuf), ctypes.sizeof(self)),
            ctypes.create_string_buffer(self.buf, self.getPayloadSize()),
            self.getPayloadSize())
        return rawbuf

    decorate(traceLog())
    def checksum(self):
        self.pktChksum = 0
        rawbuf = self._makeBuf()
        ptr = ctypes.cast(rawbuf, ctypes.POINTER(ctypes.c_uint16))
        for i in range( len(rawbuf) // 2 ):
            self.pktChksum = self.pktChksum + ptr[i]

        self.pktChksum = - self.pktChksum

    decorate(traceLog())
    def writeTo(self, fd):
        rawbuf = self._makeBuf()
        fd.write(rawbuf.raw)

def ptr_add(ptr, offset):
    address = ctypes.addressof(ptr.contents) + offset
    return ctypes.pointer(type(ptr.contents).from_address(address))




# RBU Packet Requirements
#
#    1.All values in the packet header except PktNum must be the same for all packets in a set with the following exception:
#            -- Packet 0 may have a different packet size (PktSize).
#            -- checksums
#    2.Packet 0 data does not contain RBU data. Packet 1 contains the first chunk of RBU data.
#    3.Packet data begins immediately after the header. Packet data size and
#    offset can be calculated from PktSize and HdrSize.
#    4.Reserved fields are 0.
#    5.If multiple packets sets are written to memory, all packet sets must be identical.
#    6.All packets must start on 4 KB boundaries.
#    7.All packets must be placed in non-paged memory.
#    8.The maximum size of a packet is 64 MB.
#    9.The maximum size of a packet header is 4 KB.
#    10.The maximum number of packets is 64 KB - 1.
#    11.CPU INIT# must be immediately asserted (e.g. via OS shutdown or restart)
#    after the RBU packet set is placed in memory.
#    12.PktChk is the value resulting in a zero sum of all packet words (header and data).
#    13.PktSetId uniquely identifies a packet set. BIOS aborts the packet
#    search if all packets do not have the same PkSetId. Example identifiers: a
#    4-character ASCII ID string (e.g. "_A00"), a 4-byte hash value (e.g. CRC).
#
#
#  RBU Packet 0
#
#    struct  rbu_packet_0
#    {
#        rbu_packet  header;
#        u8  passwordCheckInfo;  # bit 7: passwordCheck is present   bits 0-6: reserved
#        u32 passwordCheck;      # crc-32 of admin/setup password
#        # the rest is reserved for future expansion.
#    }
#    LIBSMBIOS_PACKED_ATTR;
#
#  RBU Packet 0 Definition
#
#    Packet 0 is reserved for packet set information. Packet 0 data consists of data items -- each item consists of an info byte followed by the actual data item. If bit 0 of the info byte is 1, the actual data starting at the next byte is present. If bit 0 is 0, the data is not present.
#
#    The system flash password is currently defined as the admin or setup password.
#
#    BIOS reject the packet set when:
#    1.The packet set flash password CRC and the system flash password CRC do not match.
#    2.The packet set flash password CRC is not present but the system flash password is present.
#

