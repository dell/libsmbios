# vim:tw=0:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=python:

  #############################################################################
  #
  # Copyright (c) 2005 Dell Computer Corporation
  # Dual Licenced under GNU GPL and OSL
  #
  #############################################################################
"""
token:
    python interface to functions in libsmbios_c  obj/token.h
"""

# imports (alphabetical)
import ctypes
import exceptions

from _common import *

__all__ = ["TokenTable", "TOKEN_DEFAULTS", "TOKEN_GET_SINGLETON", "TOKEN_GET_NEW", "TOKEN_UNIT_TEST_MODE"]

TOKEN_DEFAULTS      =0x0000
TOKEN_GET_SINGLETON =0x0001
TOKEN_GET_NEW       =0x0002
TOKEN_UNIT_TEST_MODE=0x0004

class Token(ctypes.Structure): 
    def getID(self):
        pass
    def getType(self):
        pass
    def isBool(self):
        pass
    def isActive(self):
        pass
    def activate(self):
        pass
    def isString(self):
        pass
    def getString(self):
        pass
    def setString(self, newstr):
        pass
    def tryPassword(self, pass_ascii, pass_scancode):
        pass

def TokenTable(flags=TOKEN_GET_SINGLETON, factory_args=None):
    if factory_args is None: factory_args = []
    if _TokenTable._instance is None:
        _TokenTable._instance = _TokenTable( flags, *factory_args)
    return _TokenTable._instance

class _TokenTable(object):
    _instance = None
    def __init__(self, *args):
        self._tableobj = None
        self._tableobj = _libsmbios_c.token_table_factory(*args)

    def __del__(self):
        _libsmbios_c.token_table_free(self._tableobj)

    def __iter__(self):
        pass

    def getID(self, id):
        pass

# initialize libsmbios lib
_libsmbios_c = ctypes.cdll.LoadLibrary("libsmbios_c.so.2")

# check ctypes bool support. If it doesnt exist, fake it.
if not getattr(ctypes, "c_bool", None):
    ctypes.c_bool = ctypes.c_uint8

#struct token_table;
class _token_table(ctypes.Structure): pass

#// format error string
#const char *token_table_strerror(const struct token_table *m);
# define strerror first so we can use it in error checking other functions.
_libsmbios_c.token_table_strerror.argtypes = [ ctypes.POINTER(_token_table) ]
_libsmbios_c.token_table_strerror.restype = ctypes.c_char_p
def _strerror(obj):
    return Exception(_libsmbios_c.token_table_strerror(obj))

#struct token_table *token_table_factory(int flags, ...);
# dont define argtypes because this is a varargs function...
#_libsmbios_c.token_table_factory.argtypes = [ctypes.c_int, ]
_libsmbios_c.token_table_factory.restype = ctypes.POINTER(_token_table)
_libsmbios_c.token_table_factory.errcheck = errorOnNullPtrFN(lambda r,f,a: _strerror(r))

#void   token_table_free(struct token_table *);
_libsmbios_c.token_table_free.argtypes = [ ctypes.POINTER(_token_table) ]
_libsmbios_c.token_table_free.restype = None

#const struct token_obj * DLL_SPEC token_table_get_next(const struct token_table *, const struct token_obj *cur);
_libsmbios_c.token_table_get_next.argtypes = [ ctypes.POINTER(_token_table), ctypes.POINTER(Token) ]
_libsmbios_c.token_table_get_next.restype = ctypes.POINTER(Token)

#const struct token_obj * DLL_SPEC token_table_get_next_by_id(const struct token_table *, const struct token_obj *cur, u16 id);
_libsmbios_c.token_table_get_next_by_id.argtypes = [ ctypes.POINTER(_token_table), ctypes.POINTER(Token), ctypes.c_uint16 ]
_libsmbios_c.token_table_get_next_by_id.restype = ctypes.POINTER(Token)

#u16  DLL_SPEC token_obj_get_id(const struct token_obj *);
_libsmbios_c.token_obj_get_id.argtypes = [ ctypes.POINTER(Token) ]
_libsmbios_c.token_obj_get_id.restype = ctypes.c_uint16

#int  DLL_SPEC token_obj_get_type(const struct token_obj *);
_libsmbios_c.token_obj_get_type.argtypes = [ ctypes.POINTER(Token) ]
_libsmbios_c.token_obj_get_type.restype = ctypes.c_int

#bool  DLL_SPEC token_obj_is_bool(const struct token_obj *);
_libsmbios_c.token_obj_is_bool.argtypes = [ ctypes.POINTER(Token) ]
_libsmbios_c.token_obj_is_bool.restype = ctypes.c_bool

#bool  DLL_SPEC token_obj_is_active(const struct token_obj *);
_libsmbios_c.token_obj_is_active.argtypes = [ ctypes.POINTER(Token) ]
_libsmbios_c.token_obj_is_active.restype = ctypes.c_bool

#int  DLL_SPEC token_obj_activate(const struct token_obj *);
_libsmbios_c.token_obj_activate.argtypes = [ ctypes.POINTER(Token) ]
_libsmbios_c.token_obj_activate.restype = ctypes.c_int

#bool  DLL_SPEC token_obj_is_string(const struct token_obj *);
_libsmbios_c.token_obj_is_string.argtypes = [ ctypes.POINTER(Token) ]
_libsmbios_c.token_obj_is_string.restype = ctypes.c_bool

#char*  DLL_SPEC token_obj_get_string(const struct token_obj *, size_t *len);

#int  DLL_SPEC token_obj_set_string(const struct token_obj *, const char *, size_t size);

#const struct smbios_struct * DLL_SPEC token_obj_get_smbios_struct(const struct token_obj *);

#int  DLL_SPEC token_obj_try_password(const struct token_obj *, const char *pass_ascii, const char *pass_scancode);

#const void * DLL_SPEC token_obj_get_ptr(const struct token_obj *t);




