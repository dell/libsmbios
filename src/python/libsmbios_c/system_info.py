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
import exceptions

__all__ = []

# initialize libsmbios lib
_libsmbios_c = ctypes.cdll.LoadLibrary("libsmbios_c.so.2")

class Exception(exceptions.Exception): pass

def _freeLibString(result, func, args):
    pystr = ctypes.cast(result, ctypes.c_char_p).value
    if pystr is None:
        raise Exception("null string returned")

    _libsmbios_c.sysinfo_string_free(result)
    return pystr

def _errorOnZero(result, func, args):
    if result is None or result == 0:
        raise Exception, ("returned error")
    return result
 
def _errorOnNegative(result, func, args):
    if result is None or result < 0:
        raise Exception, ("returned error")
    return result

#    const char *smbios_get_library_version_string(); 
_libsmbios_c.smbios_get_library_version_string.argtypes = []
_libsmbios_c.smbios_get_library_version_string.restype = ctypes.c_char_p
get_library_version_string = _libsmbios_c.smbios_get_library_version_string
__all__.append("get_library_version_string")

#    const char *smbios_get_library_version_major(); 
_libsmbios_c.smbios_get_library_version_major.argtypes = []
_libsmbios_c.smbios_get_library_version_major.restype = ctypes.c_int
get_library_version_major = _libsmbios_c.smbios_get_library_version_major
__all__.append("get_library_version_major")

#    const char *smbios_get_library_version_minor(); 
_libsmbios_c.smbios_get_library_version_minor.argtypes = []
_libsmbios_c.smbios_get_library_version_minor.restype = ctypes.c_int
get_library_version_minor = _libsmbios_c.smbios_get_library_version_minor
__all__.append("get_library_version_minor")

#    const char *sysinfo_get_dell_system_id(); 
_libsmbios_c.sysinfo_get_dell_system_id.argtypes = []
_libsmbios_c.sysinfo_get_dell_system_id.restype = ctypes.c_int
_libsmbios_c.sysinfo_get_dell_system_id.errcheck=_errorOnZero
get_dell_system_id = _libsmbios_c.sysinfo_get_dell_system_id
__all__.append("get_dell_system_id")

#    void sysinfo_string_free( const char * );
_libsmbios_c.sysinfo_string_free.argtypes = [ctypes.POINTER(ctypes.c_char),]
_libsmbios_c.sysinfo_string_free.restype = None

def _mk_simple_sysinfo_str_fn(name):
    import sys
    getattr(_libsmbios_c,  "sysinfo_%s" % name).argtypes=[]
    getattr(_libsmbios_c,  "sysinfo_%s" % name).restype=ctypes.POINTER(ctypes.c_char)
    getattr(_libsmbios_c,  "sysinfo_%s" % name).errcheck=_freeLibString
    sys.modules[__name__].__dict__[name] = getattr(_libsmbios_c,  "sysinfo_%s" % name)
    __all__.append(name)

_mk_simple_sysinfo_str_fn("get_system_name")
_mk_simple_sysinfo_str_fn("get_vendor_name")
_mk_simple_sysinfo_str_fn("get_bios_version")
_mk_simple_sysinfo_str_fn("get_asset_tag")
_mk_simple_sysinfo_str_fn("get_service_tag")


#int get_property_ownership_tag(char *tagBuf, size_t size);
_libsmbios_c.sysinfo_get_property_ownership_tag.argtypes = [ctypes.c_char_p, ctypes.c_size_t]
_libsmbios_c.sysinfo_get_property_ownership_tag.restype = ctypes.c_int
_libsmbios_c.sysinfo_get_property_ownership_tag.errcheck=_errorOnNegative
def get_property_ownership_tag():
    buf = ctypes.create_string_buffer(81)
    _libsmbios_c.sysinfo_get_property_ownership_tag(buf, len(buf)-1)
    return buf.value.strip()
__all__.append("get_property_ownership_tag")

#int set_property_ownership_tag(u32 security_key, const char *newTag, size_t size);
_libsmbios_c.sysinfo_set_property_ownership_tag.argtypes = [ctypes.c_uint16, ctypes.c_char_p, ctypes.c_size_t]
_libsmbios_c.sysinfo_set_property_ownership_tag.restype = ctypes.c_int
_libsmbios_c.sysinfo_set_property_ownership_tag.errcheck=_errorOnNegative
def set_property_ownership_tag(newtag, key=0):
    return _libsmbios_c.sysinfo_set_property_ownership_tag(key, newtag, len(newtag))
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
