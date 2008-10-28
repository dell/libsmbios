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
    def getId(self):
        return _libsmbios_c.token_obj_get_id(self)

    def getType(self):
        return _libsmbios_c.token_obj_get_type(self)

    def isBool(self):
        return _libsmbios_c.token_obj_is_bool(self)

    def isActive(self):
        return _libsmbios_c.token_obj_is_active(self)

    def activate(self):
        return _libsmbios_c.token_obj_activate(self)

    def isString(self):
        return _libsmbios_c.token_obj_is_string(self)

    def getString(self):
        len = ctypes.c_size_t()
        retstr = _libsmbios_c.token_obj_get_string(self, ctypes.byref(len))
        # not usually null-terminated, so use string_at with len
        if bool(retstr): # avoid null-ptr deref
            return ctypes.string_at( retstr, len.value )
        else:
            return None

    def setString(self, newstr):
        return _libsmbios_c.token_obj_set_string(self, newstr, len(newstr))

    def tryPassword(self, pass_ascii, pass_scancode):
        return _libsmbios_c.token_obj_try_password(self, pass_ascii, pass_scancode)

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
        cur = ctypes.POINTER(Token)()
        while 1:
            cur =_libsmbios_c.token_table_get_next( self._tableobj, cur )
            if bool(cur):
                yield cur.contents
            else:
                raise exceptions.StopIteration( _("hit end of table.") )

    def __getitem__(self, id):
        cur = ctypes.POINTER(Token)()
        cur =_libsmbios_c.token_table_get_next_by_id( self._tableobj, cur, id )
        if bool(cur):
            return cur.contents
        else:
            raise exceptions.IndexError( _("no such ID") )


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
def _table_strerror(obj):
    return Exception(_libsmbios_c.token_table_strerror(obj))

#const char *token_obj_strerror(const struct token_table *m);
# define strerror first so we can use it in error checking other functions.
_libsmbios_c.token_obj_strerror.argtypes = [ ctypes.POINTER(Token) ]
_libsmbios_c.token_obj_strerror.restype = ctypes.c_char_p
def _obj_strerror(obj):
    return Exception(_libsmbios_c.token_obj_strerror(obj))

#struct token_table *token_table_factory(int flags, ...);
# dont define argtypes because this is a varargs function...
#_libsmbios_c.token_table_factory.argtypes = [ctypes.c_int, ]
_libsmbios_c.token_table_factory.restype = ctypes.POINTER(_token_table)
_libsmbios_c.token_table_factory.errcheck = errorOnNullPtrFN(lambda r,f,a: _table_strerror(r))

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
_libsmbios_c.token_obj_is_active.restype = ctypes.c_int
_libsmbios_c.token_obj_is_active.errcheck = errorOnNegativeFN(lambda r,f,a: _table_strerror(a[0]))

#int  DLL_SPEC token_obj_activate(const struct token_obj *);
_libsmbios_c.token_obj_activate.argtypes = [ ctypes.POINTER(Token) ]
_libsmbios_c.token_obj_activate.restype = ctypes.c_int
_libsmbios_c.token_obj_activate.errcheck = errorOnNegativeFN(lambda r,f,a: _table_strerror(a[0]))

#bool  DLL_SPEC token_obj_is_string(const struct token_obj *);
_libsmbios_c.token_obj_is_string.argtypes = [ ctypes.POINTER(Token) ]
_libsmbios_c.token_obj_is_string.restype = ctypes.c_bool

#char*  DLL_SPEC token_obj_get_string(const struct token_obj *, size_t *len);
_libsmbios_c.token_obj_get_string.argtypes = [ ctypes.POINTER(Token), ctypes.POINTER(ctypes.c_size_t) ]
_libsmbios_c.token_obj_get_string.restype = ctypes.c_void_p
_libsmbios_c.token_obj_get_string.errcheck = errorOnNullPtrFN(lambda r,f,a: _table_strerror(a[0]))

#int  DLL_SPEC token_obj_set_string(const struct token_obj *, const char *, size_t size);
_libsmbios_c.token_obj_set_string.argtypes = [ ctypes.POINTER(Token), ctypes.c_char_p, ctypes.POINTER(ctypes.c_size_t) ]
_libsmbios_c.token_obj_set_string.restype = ctypes.c_int
_libsmbios_c.token_obj_set_string.errcheck = errorOnNegativeFN(lambda r,f,a: _table_strerror(a[0]))

#const struct smbios_struct * DLL_SPEC token_obj_get_smbios_struct(const struct token_obj *);
#_libsmbios_c.token_obj_get_smbios_struct.argtypes = [ ctypes.POINTER(Token), ]
#_libsmbios_c.token_obj_get_smbios_struct.restype = ctypes.c_int

#int  DLL_SPEC token_obj_try_password(const struct token_obj *, const char *pass_ascii, const char *pass_scancode);
_libsmbios_c.token_obj_get_smbios_struct.argtypes = [ ctypes.POINTER(Token), ctypes.c_char_p, ctypes.c_char_p ]
_libsmbios_c.token_obj_get_smbios_struct.restype = ctypes.c_int

#const void * DLL_SPEC token_obj_get_ptr(const struct token_obj *t);
_libsmbios_c.token_obj_get_ptr.argtypes = [ ctypes.POINTER(Token), ]
_libsmbios_c.token_obj_get_ptr.restype = ctypes.c_void_p




