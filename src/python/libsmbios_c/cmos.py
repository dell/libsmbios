# vim:tw=0:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=python:

  #############################################################################
  #
  # Copyright (c) 2005 Dell Computer Corporation
  # Dual Licenced under GNU GPL and OSL
  #
  #############################################################################
"""
cmos_obj:
    python interface to functions in libsmbios_c  obj/cmos.h
"""

# imports (alphabetical)
import ctypes
import exceptions

from libsmbios_c import libsmbios_c_DLL as DLL
from _common import errorOnNegativeFN, errorOnNullPtrFN
from trace_decorator import decorate, traceLog, getLog

__all__ = ["CmosAccess", "CMOS_DEFAULTS", "CMOS_GET_SINGLETON", "CMOS_GET_NEW", "CMOS_UNIT_TEST_MODE"]

CMOS_DEFAULTS      =0x0000
CMOS_GET_SINGLETON =0x0001
CMOS_GET_NEW       =0x0002
CMOS_UNIT_TEST_MODE=0x0004

decorate(traceLog())
def CmosAccess(flags=CMOS_GET_SINGLETON, *factory_args):
    if flags & CMOS_GET_SINGLETON:
        if _CmosAccess._instance is None:
            _CmosAccess._instance = _CmosAccess( flags, *factory_args)
        return _CmosAccess._instance
    else:
        return _CmosAccess( flags, *factory_args)

class _CmosAccess(ctypes.Structure):
    _instance = None

    decorate(traceLog())
    def __init__(self, *args):
        self._cmosobj = None
        self._cmosobj = DLL.cmos_obj_factory(*args)
        self._callbacks = []

    # dont decorate __del__
    def __del__(self):
        DLL.cmos_obj_free(self._cmosobj)

    decorate(traceLog())
    def readByte(self, indexPort, dataPort, offset):
        buf = ctypes.c_uint8()
        DLL.cmos_obj_read_byte(self._cmosobj, buf, indexPort, dataPort, offset)
        return buf.value

    decorate(traceLog())
    def writeByte(self, buf, indexPort, dataPort, offset):
        DLL.cmos_obj_write_byte(self._cmosobj, buf, indexPort, dataPort, offset)

    decorate(traceLog())
    def registerCallback(self, callback, userdata, freecb):
        cb = WRITE_CALLBACK(callback)
        # append callback to array that has same lifetime as object so python
        # doesnt garbage collect it from under us
        self._callbacks.append(cb)

        # special handling for possible NULL free callback
        fcb = ctypes.cast(None, FREE_CALLBACK)
        if freecb is not None:
            fcb = FREE_CALLBACK(freecb)
            self._callbacks.append(fcb)

        DLL.cmos_obj_register_write_callback(self._cmosobj, cb, userdata, fcb)


#// format error string
#const char *cmos_obj_strerror(const struct cmos_access_obj *m);
# define strerror first so we can use it in error checking other functions.
DLL.cmos_obj_strerror.argtypes = [ ctypes.POINTER(_CmosAccess) ]
DLL.cmos_obj_strerror.restype = ctypes.c_char_p
decorate(traceLog())
def _strerror(obj):
    return Exception(DLL.cmos_obj_strerror(obj))

#struct cmos_access_obj *cmos_obj_factory(int flags, ...);
# dont define argtypes because this is a varargs function...
#DLL.cmos_obj_factory.argtypes = [ctypes.c_int, ]
DLL.cmos_obj_factory.restype = ctypes.POINTER(_CmosAccess)
DLL.cmos_obj_factory.errcheck = errorOnNullPtrFN(lambda r,f,a: _strerror(r))

#void   cmos_obj_free(struct cmos_access_obj *);
DLL.cmos_obj_free.argtypes = [ ctypes.POINTER(_CmosAccess) ]
DLL.cmos_obj_free.restype = None

#int     cmos_obj_read_byte(const struct cmos_access_obj *, u8 *byte, u32 indexPort, u32 dataPort, u32 offset);
DLL.cmos_obj_read_byte.argtypes = [ ctypes.POINTER(_CmosAccess), ctypes.POINTER(ctypes.c_uint8), ctypes.c_uint32, ctypes.c_uint32, ctypes.c_uint32 ]
DLL.cmos_obj_read_byte.restype = ctypes.c_int
DLL.cmos_obj_read_byte.errcheck = errorOnNegativeFN(_strerror)

#int    cmos_obj_write_byte(const struct cmos_access_obj *, u8 byte,  u32 indexPort, u32 dataPort, u32 offset);
DLL.cmos_obj_write_byte.argtypes = [ ctypes.POINTER(_CmosAccess), ctypes.c_uint8, ctypes.c_uint32, ctypes.c_uint32, ctypes.c_uint32 ]
DLL.cmos_obj_write_byte.restype = ctypes.c_int
DLL.cmos_obj_write_byte.errcheck = errorOnNegativeFN(_strerror)

#// useful for checksums, etc
#typedef int (*cmos_write_callback)(const struct cmos_access_obj *, bool, void *);
WRITE_CALLBACK = ctypes.CFUNCTYPE(ctypes.c_int, ctypes.POINTER(_CmosAccess), ctypes.c_bool, ctypes.c_void_p)
FREE_CALLBACK = ctypes.CFUNCTYPE(None, ctypes.c_void_p)

#void cmos_obj_register_write_callback(struct cmos_access_obj *, cmos_write_callback, void *, void (*destruct)(void *));
DLL.cmos_obj_register_write_callback.argtypes = [ ctypes.POINTER(_CmosAccess), WRITE_CALLBACK, ctypes.c_void_p, FREE_CALLBACK ]
DLL.cmos_obj_register_write_callback.restype = None

#int cmos_obj_run_callbacks(const struct cmos_access_obj *m, bool do_update);


