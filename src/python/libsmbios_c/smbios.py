# vim:tw=0:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=python:

  #############################################################################
  #
  # Copyright (c) 2005 Dell Computer Corporation
  # Dual Licenced under GNU GPL and OSL
  #
  #############################################################################
"""
smbios:
    python interface to functions in libsmbios_c  obj/smbios.h
"""

# imports (alphabetical)
import ctypes
import exceptions

from _common import *

__all__ = ["SmbiosAccess", "SMBIOS_DEFAULTS", "SMBIOS_GET_SINGLETON", "SMBIOS_GET_NEW", "SMBIOS_UNIT_TEST_MODE"]

SMBIOS_DEFAULTS      =0x0000
SMBIOS_GET_SINGLETON =0x0001
SMBIOS_GET_NEW       =0x0002
SMBIOS_UNIT_TEST_MODE=0x0004

def SmbiosTable(flags=SMBIOS_GET_SINGLETON, factory_args=None):
    if factory_args is None: factory_args = []
    if _SmbiosTable._instance is None:
        _SmbiosTable._instance = _SmbiosTable( flags, *factory_args)
    return _SmbiosTable._instance

class _SmbiosTable(object):
    _instance = None
    def __init__(self, *args):
        self._memobj = _libsmbios_c.smbios_table_factory(*args)

    def __del__(self):
        _libsmbios_c.smbios_table_free(self._memobj)


# initialize libsmbios lib
_libsmbios_c = ctypes.cdll.LoadLibrary("libsmbios_c.so.2")

#struct smbios_table;
class _smbios_table(ctypes.Structure): pass

#// format error string
#const char *smbios_table_strerror(const struct smbios_table *m);
# define strerror first so we can use it in error checking other functions.
_libsmbios_c.smbios_table_strerror.argtypes = [ ctypes.POINTER(_smbios_table) ]
_libsmbios_c.smbios_table_strerror.restype = ctypes.c_char_p
def _strerror(obj):
    return Exception(_libsmbios_c.smbios_table_strerror(obj))


#struct smbios_table *smbios_table_factory(int flags, ...);
# dont define argtypes because this is a varargs function...
#_libsmbios_c.smbios_table_factory.argtypes = [ctypes.c_int, ]
_libsmbios_c.smbios_table_factory.restype = ctypes.POINTER(_smbios_table)
_libsmbios_c.smbios_table_factory.errcheck = errorOnNullPtrFN(lambda r,f,a: _strerror(r))

#void   smbios_table_free(struct smbios_table *);
_libsmbios_c.smbios_table_free.argtypes = [ ctypes.POINTER(_smbios_table) ]
_libsmbios_c.smbios_table_free.restype = None




