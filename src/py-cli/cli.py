from optparse import OptionParser
import string
import sys

def getStdOptionParser(usage, version):
    parser = OptionParser(usage=usage, version=version)
    return addStdOptions(parser)

def addStdOptions(parser):
    parser.add_option('--password', action="store", dest="password",
                      help=_("BIOS Setup password for set/activate operations."))
    parser.add_option('-r', '--rawpassword', action="store_true", dest="raw", default=False,
                      help=_("Do not auto-convert password to scancodes."))

    parser.add_option('--memory-dat', action="store", type="string", dest="memory_dat",
                      help=_("Path to a memory dump file to use instead of real RAM"),
                      default=None)
    parser.add_option('--cmos-dat', action="store", type="string", dest="cmos_dat",
                      help=_("Path to a CMOS dump file to use instead of real CMOS"),
                      default=None)
    return parser

def setup_std_options(options):
    if options.memory_dat is not None:
        import libsmbios_c.memory as mem
        mem.MemoryAccess( flags= mem.MEMORY_GET_SINGLETON | mem.MEMORY_UNIT_TEST_MODE, factory_args=(options.memory_dat,))

    if options.cmos_dat is not None:
        import libsmbios_c.cmos as cmos
        cmos.CmosAccess( flags= cmos.CMOS_GET_SINGLETON | cmos.CMOS_UNIT_TEST_MODE, factory_args=(options.cmos_dat,))

def wrap(s, line_len=80, indent=0, first_line_indent=0, first_line_start=0):
    sys.stdout.write(" "*first_line_indent)
    chars_printed = first_line_start
    for c in s:
        sys.stdout.write(c)
        chars_printed = chars_printed + 1
        if chars_printed > line_len:
            sys.stdout.write("\n")
            chars_printed=indent
            sys.stdout.write(" "*indent)

def makePrintable(s):
    printable = 1
    for ch in s:
        if ch not in string.printable:
            printable=0

    if printable:
        return s

    else:
        retstr=""
        i = 0
        for ch in s:
            i = i+1
            retstr = retstr + "0x%02x" % ord(ch)
        return retstr


