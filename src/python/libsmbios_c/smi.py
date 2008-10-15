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


array_4_u32 = ctypes.c_int32 * 4

#void dell_simple_ci_smi(u16 smiClass, u16 select, const u32 args[4], u32 res[4]);
_libsmbios_c.dell_simple_ci_smi.argtypes = [ctypes.c_uint16, ctypes.c_uint16, array_4_u32, array_4_u32]
_libsmbios_c.dell_simple_ci_smi.restype = None
def simple_ci_smi(smiClass, select, *args):
    arg = array_4_u32(*args)
    res = array_4_u32(0, 0, 0, 0)
    _libsmbios_c.dell_simple_ci_smi(smiClass, select, arg, res)
    return [ i for i in res ]
__all__.append("dell_simple_ci_smi")

#int dell_smi_read_nv_storage         (u32 location, u32 *minValue, u32 *maxValue);
_libsmbios_c.dell_smi_read_nv_storage.restype = ctypes.c_int
_libsmbios_c.dell_smi_read_nv_storage.argtypes = [
        ctypes.c_uint32, 
        ctypes.POINTER(ctypes.c_uint32), 
        ctypes.POINTER(ctypes.c_uint32)]
def read_nv_storage(location):
    min = ctypes.c_uint32(0)
    max = ctypes.c_uint32(0)
    cur = _libsmbios_c.dell_smi_read_nv_storage(location, ctypes.pointer(min), ctypes.pointer(max))
    return (cur, min.value, max.value)
__all__.append("read_nv_storage")

#int dell_smi_read_battery_mode_setting(u32 location, u32 *minValue, u32 *maxValue);
_libsmbios_c.dell_smi_read_battery_mode_setting.argtypes = [ctypes.c_uint32, ctypes.POINTER(ctypes.c_uint32), ctypes.POINTER(ctypes.c_uint32)]
_libsmbios_c.dell_smi_read_battery_mode_setting.restype = ctypes.c_int
def read_battery_mode_setting(location):
    min = ctypes.c_uint32(0)
    max = ctypes.c_uint32(0)
    cur = _libsmbios_c.dell_smi_read_battery_mode_setting(location, ctypes.pointer(min), ctypes.pointer(max))
    return (cur, min.value, max.value)
__all__.append("read_battery_mode_setting")

#int dell_smi_read_ac_mode_setting     (u32 location, u32 *minValue, u32 *maxValue);
_libsmbios_c.dell_smi_read_ac_mode_setting.argtypes = [ctypes.c_uint32, ctypes.POINTER(ctypes.c_uint32), ctypes.POINTER(ctypes.c_uint32)]
_libsmbios_c.dell_smi_read_ac_mode_setting.restype = ctypes.c_int
def read_ac_mode_setting(location):
    min = ctypes.c_uint32(0)
    max = ctypes.c_uint32(0)
    cur = _libsmbios_c.dell_smi_read_ac_mode_setting(location, ctypes.pointer(min), ctypes.pointer(max))
    return (cur, min.value, max.value)
__all__.append("read_ac_mode_setting")


#int dell_smi_write_nv_storage         (u16 security_key, u32 location, u32 value);
#int dell_smi_write_battery_mode_setting(u16 security_key, u32 location, u32 value);
#int dell_smi_write_ac_mode_setting     (u16 security_key, u32 location, u32 value);
#
#int dell_smi_get_security_key(const char *pass_ascii, const char *pass_scancode, u16 *security_key);



