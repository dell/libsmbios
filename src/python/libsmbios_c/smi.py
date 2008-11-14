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

from libsmbios_c import libsmbios_c_DLL as DLL
from _common import errorOnNullPtrFN, errorOnNegativeFN, errorOnZeroFN
from trace_decorator import decorate, traceLog, getLog

__all__ = ["DellSmi", "DELL_SMI_DEFAULTS", "DELL_SMI_GET_SINGLETON", "DELL_SMI_GET_NEW", "DELL_SMI_UNIT_TEST_MODE"]
__all__.extend( [ "cbARG1", "cbARG2", "cbARG3", "cbARG4", "cbRES1", "cbRES2", "cbRES3", "cbRES4", ])

cbARG1=0
cbARG2=1
cbARG3=2
cbARG4=3
cbRES1=0
cbRES2=1
cbRES3=2
cbRES4=3
DELL_SMI_DEFAULTS      =0x0000
DELL_SMI_GET_SINGLETON =0x0001
DELL_SMI_GET_NEW       =0x0002
DELL_SMI_UNIT_TEST_MODE=0x0004

class SMIExecutionError(Exception): pass # ret = -1
class SMIUnsupported(Exception): pass # ret = -2
class SMIUnhandled(Exception): pass   # ret = -3
class SMIOutputBufferFormatError(Exception): pass  # ret == -5
class SMIOutputBufferTooSmall(Exception): pass  # ret == -6

class BadPassword(Exception): pass
class SmiCreateError(Exception): pass
class SmiBufferCreateError(Exception): pass


decorate(traceLog())
def DellSmi(flags=DELL_SMI_GET_NEW, *args):
    if _DellSmi._instance is None:
        _DellSmi._instance = _DellSmi( flags, *args)
    return _DellSmi._instance

class _DellSmi(ctypes.Structure):
    _instance = None

    decorate(traceLog())
    def __init__(self, *args, **kargs):
        self._smiobj = None
        self._smiobj = DLL.dell_smi_factory(*args)
        self.bufs = [0,0,0,0]

    # dont decorate __del__
    def __del__(self):
        DLL.dell_smi_obj_free(self._smiobj)

    decorate(traceLog())
    def setClass(self, smiclass):
        DLL.dell_smi_obj_set_class(self._smiobj, smiclass)

    decorate(traceLog())
    def setSelect(self, select):
        DLL.dell_smi_obj_set_select(self._smiobj, select)

    decorate(traceLog())
    def setArg(self, arg, val):
        DLL.dell_smi_obj_set_arg(self._smiobj, arg, val)

    decorate(traceLog())
    def getRes(self, res):
        return DLL.dell_smi_obj_get_res(self._smiobj, res)

    decorate(traceLog())
    def buffer_frombios_auto(self, arg, size):
        self.bufs[arg] = DLL.dell_smi_obj_make_buffer_frombios_auto(self._smiobj, arg, size)
        return self.bufs[arg]

    decorate(traceLog())
    def buffer_frombios_withheader(self, arg, size):
        self.bufs[arg] = DLL.dell_smi_obj_make_buffer_frombios_withheader(self._smiobj, arg, size)
        return self.bufs[arg]

    decorate(traceLog())
    def buffer_frombios_withoutheader(self, arg, size):
        self.bufs[arg] = DLL.dell_smi_obj_make_buffer_frombios_withoutheader(self._smiobj, arg, size)
        return self.bufs[arg]

    decorate(traceLog())
    def buffer_tobios(self, arg, size, buf):
        self.bufs[arg] = DLL.dell_smi_obj_make_buffer_tobios(self._smiobj, arg, size)
        if len(buf) < size: size = len(buf)
        ctypes.memmove( self.bufs[arg], buf, size )

    decorate(traceLog())
    def execute(self):
        ret = DLL.dell_smi_obj_execute(self._smiobj)
        raiseExceptionOnError(ret, self)

    decorate(traceLog())
    def getBufContents(self, arg):
        return self.bufs[arg]

decorate(traceLog())
def raiseExceptionOnError(ret, smiobj=None):
    if ret == -1:
        raise SMIExecutionError( _("SMI executed but returned error"))
    if ret == -2:
        raise SMIUnsupported(_("SMI is unsupported"))
    if ret == -3:
        raise SMIUnhandled(_("SMI did not execute"))
    if ret == -5:
        raise SMIOutputBufferFormatError(_("The output buffer the BIOS requires is in the wrong format"))
    if ret == -6:
        raise SMIOutputBufferTooSmall(_("The output buffer the BIOS requires is too small"))


# define type that can be used for arg/res:  u32 arg[4]
array_4_u32 = ctypes.c_int32 * 4

# define strerror first so we can use it in error checking other functions.
DLL.dell_smi_strerror.argtypes = [ ]
DLL.dell_smi_strerror.restype = ctypes.c_char_p
decorate(traceLog())
def _strerror():
    return DLL.dell_smi_strerror()

#void dell_simple_ci_smi(u16 smiClass, u16 select, const u32 args[4], u32 res[4]);
DLL.dell_simple_ci_smi.argtypes = [ctypes.c_uint16, ctypes.c_uint16, array_4_u32, array_4_u32]
DLL.dell_simple_ci_smi.restype = ctypes.c_int
DLL.dell_simple_ci_smi.errcheck = errorOnNegativeFN(lambda r,f,a: SMIExecutionError(_strerror()))
decorate(traceLog())
def simple_ci_smi(smiClass, select, *args):
    arg = array_4_u32(*args)
    res = array_4_u32(0, 0, 0, 0)
    DLL.dell_simple_ci_smi(smiClass, select, arg, res)
    return [ i for i in res ]
__all__.append("simple_ci_smi")

#int dell_smi_read_nv_storage         (u32 location, u32 *minValue, u32 *maxValue);
DLL.dell_smi_read_nv_storage.errcheck = errorOnNegativeFN(lambda r,f,a: SMIExecutionError(_strerror()))
DLL.dell_smi_read_nv_storage.restype = ctypes.c_int
DLL.dell_smi_read_nv_storage.argtypes = [
        ctypes.c_uint32,
        ctypes.POINTER(ctypes.c_uint32),
        ctypes.POINTER(ctypes.c_uint32),
        ctypes.POINTER(ctypes.c_uint32)]
decorate(traceLog())
def read_nv_storage(location):
    min = ctypes.c_uint32(0)
    max = ctypes.c_uint32(0)
    cur = ctypes.c_uint32(0)
    DLL.dell_smi_read_nv_storage(location, ctypes.byref(cur), ctypes.byref(min), ctypes.byref(max))
    return (cur.value, min.value, max.value)
__all__.append("read_nv_storage")

#int dell_smi_read_battery_mode_setting(u32 location, u32 *minValue, u32 *maxValue);
DLL.dell_smi_read_battery_mode_setting.errcheck = errorOnNegativeFN(lambda r,f,a: SMIExecutionError(_strerror()))
DLL.dell_smi_read_battery_mode_setting.restype = ctypes.c_int
DLL.dell_smi_read_battery_mode_setting.argtypes = [
        ctypes.c_uint32,
        ctypes.POINTER(ctypes.c_uint32),
        ctypes.POINTER(ctypes.c_uint32),
        ctypes.POINTER(ctypes.c_uint32)]
decorate(traceLog())
def read_battery_mode_setting(location):
    min = ctypes.c_uint32(0)
    max = ctypes.c_uint32(0)
    cur = ctypes.c_uint32(0)
    DLL.dell_smi_read_battery_mode_setting(location, ctypes.byref(cur), ctypes.byref(min), ctypes.byref(max))
    return (cur.value, min.value, max.value)
__all__.append("read_battery_mode_setting")

#int dell_smi_read_ac_mode_setting     (u32 location, u32 *minValue, u32 *maxValue);
DLL.dell_smi_read_ac_mode_setting.errcheck = errorOnNegativeFN(lambda r,f,a: SMIExecutionError(_strerror()))
DLL.dell_smi_read_ac_mode_setting.restype = ctypes.c_int
DLL.dell_smi_read_ac_mode_setting.argtypes = [
        ctypes.c_uint32,
        ctypes.POINTER(ctypes.c_uint32),
        ctypes.POINTER(ctypes.c_uint32),
        ctypes.POINTER(ctypes.c_uint32)]
decorate(traceLog())
def read_ac_mode_setting(location):
    min = ctypes.c_uint32(0)
    max = ctypes.c_uint32(0)
    cur = ctypes.c_uint32(0)
    DLL.dell_smi_read_ac_mode_setting(location, ctypes.byref(cur), ctypes.byref(min), ctypes.byref(max))
    return (cur.value, min.value, max.value)
__all__.append("read_ac_mode_setting")


#int dell_smi_write_nv_storage         (u16 security_key, u32 location, u32 value, u32 *smiret);
DLL.dell_smi_write_nv_storage.argtypes = [ctypes.c_uint16, ctypes.c_uint32, ctypes.c_uint32, ctypes.POINTER(ctypes.c_uint32)]
DLL.dell_smi_write_nv_storage.restype = ctypes.c_int
DLL.dell_smi_write_nv_storage.errcheck=errorOnNegativeFN(lambda r,f,a: SMIExecutionError(_strerror()))
write_nv_storage = DLL.dell_smi_write_nv_storage
__all__.append("write_nv_storage")

#int dell_smi_write_battery_mode_setting(u16 security_key, u32 location, u32 value, u32 *smiret);
DLL.dell_smi_write_battery_mode_setting.argtypes = [ctypes.c_uint16, ctypes.c_uint32, ctypes.c_uint32, ctypes.POINTER(ctypes.c_uint32)]
DLL.dell_smi_write_battery_mode_setting.restype = ctypes.c_int
DLL.dell_smi_write_battery_mode_setting.errcheck=errorOnNegativeFN(lambda r,f,a: SMIExecutionError(_strerror()))
write_battery_mode_setting = DLL.dell_smi_write_battery_mode_setting
__all__.append("write_battery_mode_setting")

#int dell_smi_write_ac_mode_setting     (u16 security_key, u32 location, u32 value, u32 *smiret);
DLL.dell_smi_write_ac_mode_setting.argtypes = [ctypes.c_uint16, ctypes.c_uint32, ctypes.c_uint32, ctypes.POINTER(ctypes.c_uint32)]
DLL.dell_smi_write_ac_mode_setting.restype = ctypes.c_int
DLL.dell_smi_write_ac_mode_setting.errcheck=errorOnNegativeFN(lambda r,f,a: SMIExecutionError(_strerror()))
write_ac_mode_setting = DLL.dell_smi_write_ac_mode_setting
__all__.append("write_ac_mode_setting")

decorate(traceLog())
def securityException(r):
    if r == -1:
        return BadPassword()
    if r == -2:
        return SMIExecutionError()

#int dell_smi_get_security_key(const char *pass_scancode, u16 *security_key);
DLL.dell_smi_get_security_key.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_uint16)]
DLL.dell_smi_get_security_key.restype = ctypes.c_int
DLL.dell_smi_get_security_key.errcheck=errorOnNegativeFN(lambda r,f,a: securityException(r))
decorate(traceLog())
def get_security_key(password):
    key = ctypes.c_uint16(0)
    DLL.dell_smi_get_security_key(password, key)
    return key.value
__all__.append("get_security_key")


DELL_SMI_PASSWORD_USER = 9
DELL_SMI_PASSWORD_ADMIN = 10
DELL_SMI_PASSWORD_OWNER = 12
DELL_SMI_PASSWORD_FMT_SCANCODE = 0
DELL_SMI_PASSWORD_FMT_ASCII = 1

#int dell_smi_password_format(int which);
DLL.dell_smi_password_format.argtypes = [ctypes.c_int]
DLL.dell_smi_password_format.restype = ctypes.c_int
password_format = DLL.dell_smi_password_format
__all__.append("password_format")

#bool dell_smi_is_password_present(int which);
DLL.dell_smi_is_password_present.argtypes = [ctypes.c_int]
DLL.dell_smi_is_password_present.restype = ctypes.c_bool
is_password_present = DLL.dell_smi_is_password_present
__all__.append("is_password_present")

#int dell_smi_password_verify(int which, const char *password);
# returns:  0==failed, 1==correct password, 2==no password installed
DLL.dell_smi_password_verify.argtypes = [ctypes.c_int, ctypes.c_char_p]
DLL.dell_smi_password_verify.restype = ctypes.c_int
password_verify = DLL.dell_smi_password_verify
__all__.append("password_verify")

#int dell_smi_password_max_len(int which);
DLL.dell_smi_password_max_len.argtypes = [ctypes.c_int]
DLL.dell_smi_password_max_len.restype = ctypes.c_int
password_max_len = DLL.dell_smi_password_max_len
__all__.append("password_max_len")

#int dell_smi_password_min_len(int which);
DLL.dell_smi_password_min_len.argtypes = [ctypes.c_int]
DLL.dell_smi_password_min_len.restype = ctypes.c_int
password_min_len = DLL.dell_smi_password_min_len
__all__.append("password_min_len")

#int dell_smi_password_change(int which, const char *oldpass, const char *newpass);
DLL.dell_smi_password_change.argtypes = [ctypes.c_int, ctypes.c_char_p, ctypes.c_char_p]
DLL.dell_smi_password_change.restype = ctypes.c_int
password_change = DLL.dell_smi_password_change
__all__.append("password_change")


################################################################################
################################################################################
###                                                                          ###
###        SMI Object functions, not exported                                ###
###                                                                          ###
################################################################################
################################################################################

#struct dell_smi_obj;

# define strerror first so we can use it in error checking other functions.
DLL.dell_smi_obj_strerror.argtypes = [ ctypes.POINTER(_DellSmi) ]
DLL.dell_smi_obj_strerror.restype = ctypes.c_char_p
decorate(traceLog())
def _obj_strerror(obj):
    return DLL.dell_smi_obj_strerror(obj)

#struct dell_smi_obj *dell_smi_factory(int flags, ...);
# dont define argtypes because this is a varargs function...
#DLL.dell_smi_factory.argtypes = [ctypes.c_int, ]
DLL.dell_smi_factory.restype = ctypes.POINTER(_DellSmi)
DLL.dell_smi_factory.errcheck = errorOnNullPtrFN(lambda r,f,a: SmiCreateError(_obj_strerror(r)))

#void dell_smi_obj_free(struct dell_smi_obj *);
DLL.dell_smi_obj_free.argtypes = [ ctypes.POINTER(_DellSmi) ]
DLL.dell_smi_obj_free.restype = None

#void dell_smi_obj_set_class(struct dell_smi_obj *, u16 );
DLL.dell_smi_obj_set_class.argtypes = [ ctypes.POINTER(_DellSmi), ctypes.c_uint16 ]
DLL.dell_smi_obj_set_class.restype = None

#void dell_smi_obj_set_select(struct dell_smi_obj *, u16 );
DLL.dell_smi_obj_set_select.argtypes = [ ctypes.POINTER(_DellSmi), ctypes.c_uint16 ]
DLL.dell_smi_obj_set_select.restype = None

#void dell_smi_obj_set_arg(struct dell_smi_obj *, u8 argno, u32 value);
DLL.dell_smi_obj_set_arg.argtypes = [ ctypes.POINTER(_DellSmi), ctypes.c_uint8, ctypes.c_uint32 ]
DLL.dell_smi_obj_set_arg.restype = None

#u32  dell_smi_obj_get_res(struct dell_smi_obj *, u8 argno);
DLL.dell_smi_obj_get_res.argtypes = [ ctypes.POINTER(_DellSmi), ctypes.c_uint8 ]
DLL.dell_smi_obj_get_res.restype = ctypes.c_uint32

#u8  *dell_smi_obj_make_buffer_frombios_auto(struct dell_smi_obj *, u8 argno, size_t size);
DLL.dell_smi_obj_make_buffer_frombios_auto.argtypes = [ ctypes.POINTER(_DellSmi), ctypes.c_uint8, ctypes.c_size_t ]
DLL.dell_smi_obj_make_buffer_frombios_auto.restype = ctypes.c_void_p
DLL.dell_smi_obj_make_buffer_frombios_auto.errcheck = errorOnNullPtrFN(lambda r,f,a: SmiBufferCreateError(_obj_strerror(a[0])))

#u8  *dell_smi_obj_make_buffer_frombios_withheader(struct dell_smi_obj *, u8 argno, size_t size);
DLL.dell_smi_obj_make_buffer_frombios_withheader.argtypes = [ ctypes.POINTER(_DellSmi), ctypes.c_uint8, ctypes.c_size_t ]
DLL.dell_smi_obj_make_buffer_frombios_withheader.restype = ctypes.c_void_p
DLL.dell_smi_obj_make_buffer_frombios_withheader.errcheck = errorOnNullPtrFN(lambda r,f,a: SmiBufferCreateError(_obj_strerror(a[0])))

#u8  *dell_smi_obj_make_buffer_frombios_withoutheader(struct dell_smi_obj *, u8 argno, size_t size);
DLL.dell_smi_obj_make_buffer_frombios_withoutheader.argtypes = [ ctypes.POINTER(_DellSmi), ctypes.c_uint8, ctypes.c_size_t ]
DLL.dell_smi_obj_make_buffer_frombios_withoutheader.restype = ctypes.c_void_p
DLL.dell_smi_obj_make_buffer_frombios_withoutheader.errcheck = errorOnNullPtrFN(lambda r,f,a: SmiBufferCreateError(_obj_strerror(a[0])))

#u8  *dell_smi_obj_make_buffer_tobios(struct dell_smi_obj *, u8 argno, size_t size);
DLL.dell_smi_obj_make_buffer_tobios.argtypes = [ ctypes.POINTER(_DellSmi), ctypes.c_uint8, ctypes.c_size_t ]
DLL.dell_smi_obj_make_buffer_tobios.restype = ctypes.c_void_p
DLL.dell_smi_obj_make_buffer_tobios.errcheck = errorOnNullPtrFN(lambda r,f,a: SmiBufferCreateError(_obj_strerror(a[0])))

#void dell_smi_obj_execute(struct dell_smi_obj *);
DLL.dell_smi_obj_execute.argtypes = [ ctypes.POINTER(_DellSmi) ]
DLL.dell_smi_obj_execute.restype = ctypes.c_int
# dont use error check since class does this
#DLL.dell_smi_obj_execute.errcheck = errorOnNegativeFN(lambda r,f,a: SMIExecutionError(_obj_strerror(a[0])))



# for testing only. It only does en_US, which is just wrong.
decorate(traceLog())
def asc_to_scancode(s):
    return "".join([ chr(asc_to_scancode_map[ord(i)]) for i in s ])

asc_to_scancode_map = [
        0x03, 0x1E, 0x30, 0x46,  0x20, 0x12, 0x21, 0x22,
        0x0E, 0x0F, 0x1C, 0x25,  0x26, 0x1C, 0x31, 0x18,
        0x19, 0x10, 0x13, 0x1F,  0x14, 0x16, 0x2F, 0x11,
        0x2D, 0x15, 0x2C, 0x1A,  0x2B, 0x1B, 0x07, 0x0C,
        0x39, 0x02, 0x28, 0x04,  0x05, 0x06, 0x08, 0x28,
        0x0A, 0x0B, 0x09, 0x0D,  0x33, 0x0C, 0x34, 0x35,
        0x0B, 0x02, 0x03, 0x04,  0x05, 0x06, 0x07, 0x08,
        0x09, 0x0A, 0x27, 0x27,  0x33, 0x0D, 0x34, 0x35,
        0x03, 0x1E, 0x30, 0x2E,  0x20, 0x12, 0x21, 0x22,
        0x23, 0x17, 0x24, 0x25,  0x26, 0x32, 0x31, 0x18,
        0x19, 0x10, 0x13, 0x1F,  0x14, 0x16, 0x2F, 0x11,
        0x2D, 0x15, 0x2C, 0x1A,  0x2B, 0x1B, 0x07, 0x0C,
        0x29, 0x1E, 0x30, 0x2E,  0x20, 0x12, 0x21, 0x22,
        0x23, 0x17, 0x24, 0x25,  0x26, 0x32, 0x31, 0x18,
        0x19, 0x10, 0x13, 0x1F,  0x14, 0x16, 0x2F, 0x11,
        0x2D, 0x15, 0x2C, 0x1A,  0x2B, 0x1B, 0x29, 0x0E,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,  0x00, 0x00, 0x00, 0x00
    ]
