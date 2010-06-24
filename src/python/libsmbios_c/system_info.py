# vim:tw=0:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=python:

  #############################################################################
  #
  # Copyright (c) 2005 Dell Computer Corporation
  # Dual Licenced under GNU GPL and OSL
  #
  #############################################################################
"""
systeminfo:
    python interface to functions in libsmbios_c  system_info.h
"""

# imports (alphabetical)
import ctypes
from exceptions import *

from libsmbios_c import libsmbios_c_DLL as DLL
from _common import errorOnNullPtrFN, errorOnNegativeFN, errorOnZeroFN, freeLibStringFN
from trace_decorator import decorate, traceLog, getLog, strip_trailing_whitespace

__all__ = []

#// format error string
#const char *token_table_strerror(const struct token_table *m);
# define strerror first so we can use it in error checking other functions.
DLL.sysinfo_strerror.argtypes = [ ]
DLL.sysinfo_strerror.restype = ctypes.c_char_p
decorate(traceLog())
def _strerror():
    return Exception(DLL.sysinfo_strerror())

#    const char *smbios_get_library_version_string(); 
DLL.smbios_get_library_version_string.argtypes = []
DLL.smbios_get_library_version_string.restype = ctypes.c_char_p
get_library_version_string = DLL.smbios_get_library_version_string
__all__.append("get_library_version_string")

#    const char *smbios_get_library_version_major(); 
DLL.smbios_get_library_version_major.argtypes = []
DLL.smbios_get_library_version_major.restype = ctypes.c_int
get_library_version_major = DLL.smbios_get_library_version_major
__all__.append("get_library_version_major")

#    const char *smbios_get_library_version_minor(); 
DLL.smbios_get_library_version_minor.argtypes = []
DLL.smbios_get_library_version_minor.restype = ctypes.c_int
get_library_version_minor = DLL.smbios_get_library_version_minor
__all__.append("get_library_version_minor")

#    const char *sysinfo_get_dell_system_id(); 
DLL.sysinfo_get_dell_system_id.argtypes = []
DLL.sysinfo_get_dell_system_id.restype = ctypes.c_int
DLL.sysinfo_get_dell_system_id.errcheck=errorOnZeroFN(lambda r,f,a: Exception(_("Could not determine System ID.")))
get_dell_system_id = DLL.sysinfo_get_dell_system_id
__all__.append("get_dell_system_id")

#    void sysinfo_string_free( const char * );
DLL.sysinfo_string_free.argtypes = [ctypes.POINTER(ctypes.c_char),]
DLL.sysinfo_string_free.restype = None

def _mk_simple_sysinfo_str_fn(name):
    import sys
    getattr(DLL,  "sysinfo_%s" % name).argtypes=[]
    getattr(DLL,  "sysinfo_%s" % name).restype=ctypes.POINTER(ctypes.c_char)
    getattr(DLL,  "sysinfo_%s" % name).errcheck=strip_trailing_whitespace()(freeLibStringFN( DLL.sysinfo_string_free, lambda r,f,a: Exception(_strerror() )))
    sys.modules[__name__].__dict__[name] = getattr(DLL,  "sysinfo_%s" % name)
    __all__.append(name)

_mk_simple_sysinfo_str_fn("get_system_name")
_mk_simple_sysinfo_str_fn("get_vendor_name")
_mk_simple_sysinfo_str_fn("get_bios_version")
_mk_simple_sysinfo_str_fn("get_asset_tag")
_mk_simple_sysinfo_str_fn("get_service_tag")
_mk_simple_sysinfo_str_fn("get_property_ownership_tag")

decorate(traceLog())
def set_service_tag(newtag, pass_ascii=None, pass_scancode=None):
    return DLL.sysinfo_set_service_tag(newtag, pass_ascii, pass_scancode)
__all__.append("set_service_tag")

DLL.sysinfo_set_asset_tag.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
DLL.sysinfo_set_asset_tag.restype = ctypes.c_int
DLL.sysinfo_set_asset_tag.errcheck=errorOnNegativeFN(lambda r,f,a: _strerror())
decorate(traceLog())
def set_asset_tag(newtag, pass_ascii=None, pass_scancode=None):
    return DLL.sysinfo_set_asset_tag(newtag, pass_ascii, pass_scancode)
__all__.append("set_asset_tag")

DLL.sysinfo_set_property_ownership_tag.argtypes = [ctypes.c_char_p, ctypes.c_char_p, ctypes.c_char_p]
DLL.sysinfo_set_property_ownership_tag.restype = ctypes.c_int
DLL.sysinfo_set_property_ownership_tag.errcheck=errorOnNegativeFN(lambda r,f,a: _strerror())
decorate(traceLog())
def set_property_ownership_tag(newtag, pass_ascii=None, pass_scancode=None):
    return DLL.sysinfo_set_property_ownership_tag(newtag, pass_ascii, pass_scancode)
__all__.append("set_property_ownership_tag")

if __name__ == "__main__":
    exitRet = 0
    def pr(s, f):
        try:
            ret = f()
        except Exception, e:
            exitRet=1
            ret = str(e)
            print "Error getting the",

        print s % ret

    pr("Libsmbios:    %s", get_library_version_string)

    try:
        sysid = "0x%04X" % get_dell_system_id()
    except Exception, e:
        exitRet=1
        print "Error getting the",
        sysid = str(e)
    print "System ID:    %s" % sysid
        
    pr("Service Tag:  %s", get_service_tag)
    pr("Asset Tag:    %s", get_asset_tag)

    try:
        esc = int(get_service_tag(), 36)
    except Exception, e:
        esc = str(e)
        print "Error getting the",

    print "Express Service Code: %s" % esc
    pr("Product Name: %s", get_system_name)
    pr("BIOS Version: %s", get_bios_version)
    pr("Vendor:       %s", get_vendor_name)
    pr("Property Ownership Tag: %s", get_property_ownership_tag)

    import sys
    sys.exit(exitRet)
