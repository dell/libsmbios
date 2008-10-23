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

class SmbiosStructure(ctypes.Structure): 
    def getString(self, off):
        pass

    def getStringNumber(self, num):
        pass

    def getType(self):
        return _libsmbios_c.smbios_struct_get_type(self)

    def getLength(self):
        return _libsmbios_c.smbios_struct_get_length(self)

    def getHandle(self):
        return _libsmbios_c.smbios_struct_get_handle(self)

    def getData(self, offset, len):
        pass

def SmbiosTable(flags=SMBIOS_GET_SINGLETON, factory_args=None):
    if factory_args is None: factory_args = []
    if _SmbiosTable._instance is None:
        _SmbiosTable._instance = _SmbiosTable( flags, *factory_args)
    return _SmbiosTable._instance

class _SmbiosTable(object):
    _instance = None
    def __init__(self, *args):
        self._tableobj = None
        self._tableobj = _libsmbios_c.smbios_table_factory(*args)

    def __del__(self):
        _libsmbios_c.smbios_table_free(self._tableobj)

    def __iter__(self):
        cur = ctypes.POINTER(SmbiosStructure)()
        while 1:
            cur =_libsmbios_c.smbios_table_get_next_struct( self._tableobj, cur )
            if bool(cur):
                yield cur.contents
            else:
                raise exceptions.StopIteration("hit end of table.")

    def byType(self, type):
        cur = ctypes.POINTER(SmbiosStructure)()
        while 1:
            cur =_libsmbios_c.smbios_table_get_next_struct( self._tableobj, cur )
            if bool(cur):
                if cur.contents.getType() == type:
                    yield cur.contents
            else:
                raise exceptions.StopIteration("hit end of table.")

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

#struct smbios_struct *smbios_table_get_next_struct(const struct smbios_table *, const struct smbios_struct *cur);
_libsmbios_c.smbios_table_get_next_struct.argtypes = [ ctypes.POINTER(_smbios_table), ctypes.POINTER(SmbiosStructure) ]
_libsmbios_c.smbios_table_get_next_struct.restype = ctypes.POINTER(SmbiosStructure)
_libsmbios_c.smbios_table_get_next_struct.errcheck = errorOnNullPtrFN(lambda r,f,a: exceptions.StopIteration)

#u8 DLL_SPEC smbios_struct_get_type(const struct smbios_struct *);
_libsmbios_c.smbios_struct_get_type.argtypes = [ ctypes.POINTER(SmbiosStructure) ]
_libsmbios_c.smbios_struct_get_type.restype = ctypes.c_uint8

#u8 DLL_SPEC smbios_struct_get_length(const struct smbios_struct *);
_libsmbios_c.smbios_struct_get_length.argtypes = [ ctypes.POINTER(SmbiosStructure) ]
_libsmbios_c.smbios_struct_get_length.restype = ctypes.c_uint8

#u16 DLL_SPEC smbios_struct_get_handle(const struct smbios_struct *);
_libsmbios_c.smbios_struct_get_handle.argtypes = [ ctypes.POINTER(SmbiosStructure) ]
_libsmbios_c.smbios_struct_get_handle.restype = ctypes.c_uint16

#const char * DLL_SPEC smbios_struct_get_string_from_offset(const struct smbios_struct *s, u8 offset);
_libsmbios_c.smbios_struct_get_string_from_offset.argtypes = [ ctypes.POINTER(SmbiosStructure), ctypes.c_uint8 ]
_libsmbios_c.smbios_struct_get_string_from_offset.restype = ctypes.c_char_p
_libsmbios_c.smbios_table_factory.errcheck = errorOnNullPtrFN(lambda r,f,a: exceptions.Exception("String from offset %d doesnt exist" % a[1]))

#const char * DLL_SPEC smbios_struct_get_string_number(const struct smbios_struct *s, u8 which);
_libsmbios_c.smbios_struct_get_string_number.argtypes = [ ctypes.POINTER(SmbiosStructure), ctypes.c_uint8 ]
_libsmbios_c.smbios_struct_get_string_number.restype = ctypes.c_char_p
_libsmbios_c.smbios_struct_get_string_number.errcheck = errorOnNullPtrFN(lambda r,f,a: exceptions.Exception("String number %d doesnt exist" % a[1]))

#int DLL_SPEC smbios_struct_get_data(const struct smbios_struct *s, void *dest, u8 offset, size_t len);



