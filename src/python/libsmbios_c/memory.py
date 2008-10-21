# vim:tw=0:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=python:

  #############################################################################
  #
  # Copyright (c) 2005 Dell Computer Corporation
  # Dual Licenced under GNU GPL and OSL
  #
  #############################################################################
"""
memory_obj:
    python interface to functions in libsmbios_c  obj/memory.h
"""

# imports (alphabetical)
import ctypes
import exceptions

from _common import *

__all__ = ["MemoryAccess", "MEMORY_DEFAULTS", "MEMORY_GET_SINGLETON", "MEMORY_GET_NEW", "MEMORY_UNIT_TEST_MODE"]

MEMORY_DEFAULTS      =0x0000
MEMORY_GET_SINGLETON =0x0001
MEMORY_GET_NEW       =0x0002
MEMORY_UNIT_TEST_MODE=0x0004

def MemoryAccess(flags=MEMORY_GET_NEW, factory_args=None):
    if factory_args is None: factory_args = []
    if _MemoryAccess._instance is None:
        _MemoryAccess._instance = _MemoryAccess( flags, *factory_args)
    return _MemoryAccess._instance

class _MemoryAccess(object):
    _instance = None
    def __init__(self, *args):
        self._memobj = _libsmbios_c.memory_obj_factory(*args)
            
    def __del__(self):
        _libsmbios_c.memory_obj_free(self._memobj)

    def read(self, offset, length):
        buf = ctypes.create_string_buffer(length)
        _libsmbios_c.memory_obj_read(self._memobj, buf, offset, length)
        return buf

    def write(self, buf, offset):
        _libsmbios_c.memory_obj_write(self._memobj, buf, offset, len(buf))



# initialize libsmbios lib
_libsmbios_c = ctypes.cdll.LoadLibrary("libsmbios_c.so.2")

#struct memory_access_obj;
class _memory_access_obj(ctypes.Structure): pass

# define strerror first so we can use it in error checking other functions.
_libsmbios_c.memory_obj_strerror.argtypes = [ ctypes.POINTER(_memory_access_obj) ]
_libsmbios_c.memory_obj_strerror.restype = ctypes.c_char_p
def _strerror(result, func, args):
    # all pass memory_access_obj as first arg
    _obj = args[0]
    _str = _libsmbios_c.memory_obj_strerror(_obj)
    return Exception(_str)

#struct memory_access_obj *memory_obj_factory(int flags, ...);
# dont define argtypes because this is a varargs function...
#_libsmbios_c.memory_obj_factory.argtypes = [ctypes.c_int, ]
_libsmbios_c.memory_obj_factory.restype = ctypes.POINTER(_memory_access_obj)
_libsmbios_c.memory_obj_factory.errcheck = errorOnNullPtrFN(lambda r,f,a: Exception(_libsmbios_c.memory_obj_strerror(r)))

#void memory_obj_free(struct memory_access_obj *);
_libsmbios_c.memory_obj_free.argtypes = [ ctypes.POINTER(_memory_access_obj) ]
_libsmbios_c.memory_obj_free.restype = None

#int  memory_obj_read(const struct memory_access_obj *, void *buffer, u64 offset, size_t length);
_libsmbios_c.memory_obj_read.argtypes = [ ctypes.POINTER(_memory_access_obj), ctypes.c_void_p, ctypes.c_uint64, ctypes.c_size_t ]
_libsmbios_c.memory_obj_read.restype = ctypes.c_int
_libsmbios_c.memory_obj_read.errcheck = errorOnNegativeFN(_strerror)

#int  memory_obj_write(const struct memory_access_obj *, void *buffer, u64 offset, size_t length);
_libsmbios_c.memory_obj_write.argtypes = [ ctypes.POINTER(_memory_access_obj), ctypes.c_void_p, ctypes.c_uint64, ctypes.c_size_t ]
_libsmbios_c.memory_obj_write.restype = ctypes.c_int
_libsmbios_c.memory_obj_write.errcheck = errorOnNegativeFN(_strerror)

#s64  memory_obj_search(const struct memory_access_obj *, const char *pat, size_t patlen, u64 start, u64 end, u64 stride);
#void  memory_obj_suggest_leave_open(struct memory_access_obj *);
#void  memory_obj_suggest_close(struct memory_access_obj *);
#bool  memory_obj_should_close(const struct memory_access_obj *);

