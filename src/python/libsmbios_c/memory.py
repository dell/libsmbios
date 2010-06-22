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

from libsmbios_c import libsmbios_c_DLL as DLL
from _common import errorOnNegativeFN, errorOnNullPtrFN
from trace_decorator import decorate, traceLog, getLog

__all__ = ["MemoryAccess", "MEMORY_DEFAULTS", "MEMORY_GET_SINGLETON", "MEMORY_GET_NEW", "MEMORY_UNIT_TEST_MODE"]

MEMORY_DEFAULTS      =0x0000
MEMORY_GET_SINGLETON =0x0001
MEMORY_GET_NEW       =0x0002
MEMORY_UNIT_TEST_MODE=0x0004

decorate(traceLog())
def MemoryAccess(flags=MEMORY_GET_SINGLETON, *factory_args):
    if flags & MEMORY_GET_SINGLETON:
        if _MemoryAccess._instance is None:
            _MemoryAccess._instance = _MemoryAccess( flags, *factory_args)
        return _MemoryAccess._instance
    else:
        return _MemoryAccess( flags, *factory_args)

class _MemoryAccess(ctypes.Structure):
    _instance = None

    decorate(traceLog())
    def __init__(self, *args):
        self._memobj = None
        self._memobj = DLL.memory_obj_factory(*args)
            
    # dont decorate __del__
    def __del__(self):
        DLL.memory_obj_free(self._memobj)

    decorate(traceLog())
    def read(self, offset, length):
        buf = ctypes.create_string_buffer(length)
        DLL.memory_obj_read(self._memobj, buf, offset, length)
        return buf

    decorate(traceLog())
    def write(self, buf, offset):
        DLL.memory_obj_write(self._memobj, buf, offset, len(buf))

    decorate(traceLog())
    def search(self, pattern, start, end, stride):
        return DLL.memory_obj_search(self._memobj, pattern, len(pattern), start, end, stride)

    decorate(traceLog())
    def close_hint(self, hint=None):
        if hint is not None:
            if hint:
                DLL.memory_obj_suggest_leave_open(self._memobj)
            else:
                DLL.memory_obj_suggest_close(self._memobj)

        return DLL.memory_obj_should_close(self._memobj)


# define strerror first so we can use it in error checking other functions.
DLL.memory_obj_strerror.argtypes = [ ctypes.POINTER(_MemoryAccess) ]
DLL.memory_obj_strerror.restype = ctypes.c_char_p
decorate(traceLog())
def _strerror(obj):
    return Exception(DLL.memory_obj_strerror(obj))

#struct memory_access_obj *memory_obj_factory(int flags, ...);
# dont define argtypes because this is a varargs function...
#DLL.memory_obj_factory.argtypes = [ctypes.c_int, ]
DLL.memory_obj_factory.restype = ctypes.POINTER(_MemoryAccess)
DLL.memory_obj_factory.errcheck = errorOnNullPtrFN(lambda r,f,a: _strerror(r))

#void memory_obj_free(struct memory_access_obj *);
DLL.memory_obj_free.argtypes = [ ctypes.POINTER(_MemoryAccess) ]
DLL.memory_obj_free.restype = None

#int  memory_obj_read(const struct memory_access_obj *, void *buffer, u64 offset, size_t length);
DLL.memory_obj_read.argtypes = [ ctypes.POINTER(_MemoryAccess), ctypes.c_void_p, ctypes.c_uint64, ctypes.c_size_t ]
DLL.memory_obj_read.restype = ctypes.c_int
DLL.memory_obj_read.errcheck = errorOnNegativeFN(lambda r,f,a: _strerror(a[0]))

#int  memory_obj_write(const struct memory_access_obj *, void *buffer, u64 offset, size_t length);
DLL.memory_obj_write.argtypes = [ ctypes.POINTER(_MemoryAccess), ctypes.c_void_p, ctypes.c_uint64, ctypes.c_size_t ]
DLL.memory_obj_write.restype = ctypes.c_int
DLL.memory_obj_write.errcheck = errorOnNegativeFN(lambda r,f,a: _strerror(a[0]))

#s64  memory_obj_search(const struct memory_access_obj *, const char *pat, size_t patlen, u64 start, u64 end, u64 stride);
DLL.memory_obj_search.argtypes = [ ctypes.POINTER(_MemoryAccess), ctypes.c_char_p, ctypes.c_size_t, ctypes.c_uint64, ctypes.c_uint64, ctypes.c_uint64 ]
DLL.memory_obj_search.restype = ctypes.c_int64
DLL.memory_obj_search.errcheck = errorOnNegativeFN(lambda r,f,a: _strerror(a[0]))

#void  memory_obj_suggest_leave_open(struct memory_access_obj *);
DLL.memory_obj_suggest_leave_open.argtypes = [ ctypes.POINTER(_MemoryAccess), ]
DLL.memory_obj_suggest_leave_open.restype = None

#void  memory_obj_suggest_close(struct memory_access_obj *);
DLL.memory_obj_suggest_close.argtypes = [ ctypes.POINTER(_MemoryAccess), ]
DLL.memory_obj_suggest_close.restype = None

#bool  memory_obj_should_close(const struct memory_access_obj *);
DLL.memory_obj_should_close.argtypes = [ ctypes.POINTER(_MemoryAccess), ]
DLL.memory_obj_should_close.restype = ctypes.c_bool
