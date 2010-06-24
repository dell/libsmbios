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

from libsmbios_c import libsmbios_c_DLL as DLL
from _common import errorOnNullPtrFN, errorOnNegativeFN
from trace_decorator import decorate, traceLog, getLog, strip_trailing_whitespace

__all__ = ["SmbiosTable", "SMBIOS_DEFAULTS", "SMBIOS_GET_SINGLETON", "SMBIOS_GET_NEW", "SMBIOS_UNIT_TEST_MODE"]

SMBIOS_DEFAULTS      =0x0000
SMBIOS_GET_SINGLETON =0x0001
SMBIOS_GET_NEW       =0x0002
SMBIOS_UNIT_TEST_MODE=0x0004

class TableParseError(Exception): pass

class SmbiosStructure(ctypes.Structure): 
    decorate(traceLog(), strip_trailing_whitespace())
    def getString(self, off):
        return DLL.smbios_struct_get_string_from_offset(self, off)

    decorate(traceLog(), strip_trailing_whitespace())
    def getStringNumber(self, num):
        return DLL.smbios_struct_get_string_number(self, num)

    decorate(traceLog())
    def getType(self):
        return DLL.smbios_struct_get_type(self)

    decorate(traceLog())
    def getLength(self):
        return DLL.smbios_struct_get_length(self)

    decorate(traceLog())
    def getHandle(self):
        return DLL.smbios_struct_get_handle(self)

    # use struct module to pull data out
    decorate(traceLog())
    def getData(self, offset, len):
        buf = ctypes.create_string_buffer(len)
        DLL.smbios_struct_get_data(self, buf, offset, len)
        return buf.raw

decorate(traceLog())
def SmbiosTable(flags=SMBIOS_GET_SINGLETON, *factory_args):
    if flags & SMBIOS_GET_SINGLETON:
        if _SmbiosTable._instance is None:
            _SmbiosTable._instance = _SmbiosTable( flags, *factory_args)
        return _SmbiosTable._instance
    else:
        return _SmbiosTable( flags, *factory_args)

class _SmbiosTable(ctypes.Structure):
    _instance = None

    decorate(traceLog())
    def __init__(self, *args):
        self._tableobj = None
        self._tableobj = DLL.smbios_table_factory(*args)

    def __del__(self):
        if self._tableobj is not None:
            DLL.smbios_table_free(self._tableobj)

    decorate(traceLog())
    def __iter__(self):
        cur = ctypes.POINTER(SmbiosStructure)()
        while 1:
            cur =DLL.smbios_table_get_next_struct( self._tableobj, cur )
            if bool(cur):
                yield cur.contents
            else:
                raise exceptions.StopIteration( _("hit end of table.") )

    decorate(traceLog())
    def iterByType(self, t):
        cur = ctypes.POINTER(SmbiosStructure)()
        while 1:
            cur =DLL.smbios_table_get_next_struct( self._tableobj, cur )
            if bool(cur):
                if cur.contents.getType() == t:
                    yield cur.contents
            else:
                raise exceptions.StopIteration( _("hit end of table.") )

    decorate(traceLog())
    def getStructureByHandle(self, handle):
        cur = ctypes.POINTER(SmbiosStructure)()
        cur =DLL.smbios_table_get_next_struct_by_handle( self._tableobj, cur, handle )
        if not bool(cur):
            raise exceptions.IndexError( _("No SMBIOS structure found with handle %s") % handle)
        return cur.contents

    decorate(traceLog())
    def getStructureByType(self, t):
        cur = ctypes.POINTER(SmbiosStructure)()
        cur =DLL.smbios_table_get_next_struct_by_type( self._tableobj, cur, t )
        if not bool(cur):
            raise exceptions.IndexError( _("No SMBIOS structure found with type %s") % t)
        return cur.contents

    __getitem__ = getStructureByType

#// format error string
#const char *smbios_table_strerror(const struct smbios_table *m);
# define strerror first so we can use it in error checking other functions.
DLL.smbios_table_strerror.argtypes = [ ctypes.POINTER(_SmbiosTable) ]
DLL.smbios_table_strerror.restype = ctypes.c_char_p
decorate(traceLog())
def _strerror(obj):
    return DLL.smbios_table_strerror(obj)

#struct smbios_table *smbios_table_factory(int flags, ...);
# dont define argtypes because this is a varargs function...
#DLL.smbios_table_factory.argtypes = [ctypes.c_int, ]
DLL.smbios_table_factory.restype = ctypes.POINTER(_SmbiosTable)
DLL.smbios_table_factory.errcheck = errorOnNullPtrFN(lambda r,f,a: TableParseError(_strerror(r)))

#void   smbios_table_free(struct smbios_table *);
DLL.smbios_table_free.argtypes = [ ctypes.POINTER(_SmbiosTable) ]
DLL.smbios_table_free.restype = None

#struct smbios_struct *smbios_table_get_next_struct(const struct smbios_table *, const struct smbios_struct *cur);
DLL.smbios_table_get_next_struct.argtypes = [ ctypes.POINTER(_SmbiosTable), ctypes.POINTER(SmbiosStructure) ]
DLL.smbios_table_get_next_struct.restype = ctypes.POINTER(SmbiosStructure)

#struct smbios_struct *smbios_table_get_next_struct_by_type(const struct smbios_table *, const struct smbios_struct *cur);
DLL.smbios_table_get_next_struct_by_type.argtypes = [ ctypes.POINTER(_SmbiosTable), ctypes.POINTER(SmbiosStructure), ctypes.c_uint8 ]
DLL.smbios_table_get_next_struct_by_type.restype = ctypes.POINTER(SmbiosStructure)

#struct smbios_struct *smbios_table_get_next_struct_by_handle(const struct smbios_table *, const struct smbios_struct *cur);
DLL.smbios_table_get_next_struct_by_handle.argtypes = [ ctypes.POINTER(_SmbiosTable), ctypes.POINTER(SmbiosStructure), ctypes.c_uint16 ]
DLL.smbios_table_get_next_struct_by_handle.restype = ctypes.POINTER(SmbiosStructure)

#u8 DLL_SPEC smbios_struct_get_type(const struct smbios_struct *);
DLL.smbios_struct_get_type.argtypes = [ ctypes.POINTER(SmbiosStructure) ]
DLL.smbios_struct_get_type.restype = ctypes.c_uint8

#u8 DLL_SPEC smbios_struct_get_length(const struct smbios_struct *);
DLL.smbios_struct_get_length.argtypes = [ ctypes.POINTER(SmbiosStructure) ]
DLL.smbios_struct_get_length.restype = ctypes.c_uint8

#u16 DLL_SPEC smbios_struct_get_handle(const struct smbios_struct *);
DLL.smbios_struct_get_handle.argtypes = [ ctypes.POINTER(SmbiosStructure) ]
DLL.smbios_struct_get_handle.restype = ctypes.c_uint16

#const char * DLL_SPEC smbios_struct_get_string_from_offset(const struct smbios_struct *s, u8 offset);
DLL.smbios_struct_get_string_from_offset.argtypes = [ ctypes.POINTER(SmbiosStructure), ctypes.c_uint8 ]
DLL.smbios_struct_get_string_from_offset.restype = ctypes.c_char_p
DLL.smbios_struct_get_string_from_offset.errcheck = errorOnNullPtrFN(lambda r,f,a: exceptions.IndexError( _("String from offset %d doesnt exist") % a[1]))

#const char * DLL_SPEC smbios_struct_get_string_number(const struct smbios_struct *s, u8 which);
DLL.smbios_struct_get_string_number.argtypes = [ ctypes.POINTER(SmbiosStructure), ctypes.c_uint8 ]
DLL.smbios_struct_get_string_number.restype = ctypes.c_char_p
DLL.smbios_struct_get_string_number.errcheck = errorOnNullPtrFN(lambda r,f,a: exceptions.IndexError( _("String number %d doesnt exist") % a[1]))

#int DLL_SPEC smbios_struct_get_data(const struct smbios_struct *s, void *dest, u8 offset, size_t len);
DLL.smbios_struct_get_data.argtypes = [ ctypes.POINTER(SmbiosStructure), ctypes.c_void_p, ctypes.c_uint8, ctypes.c_size_t ]
DLL.smbios_struct_get_data.restype = ctypes.c_int
DLL.smbios_struct_get_data.errcheck = errorOnNegativeFN(lambda r,f,a: exceptions.IndexError( _("Tried to get data past the end of the structure.") ))



