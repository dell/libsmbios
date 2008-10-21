# vim:tw=0:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=python:

  #############################################################################
  #
  # Copyright (c) 2005 Dell Computer Corporation
  # Dual Licenced under GNU GPL and OSL
  #
  #############################################################################
"""
_common:
    
"""

# imports (alphabetical)
import exceptions

__all__ = ["freeLibStringFN", "errorOnZeroFN", "errorOnNegativeFN", "errorOnNullPtrFN"]

class Exception(exceptions.Exception): pass

def _doExc(exception_fn, r, f, a, msg):
    if exception_fn is not None:
        raise exception_fn(r, f, a)
    else:
        raise Exception("null string returned")

def freeLibStringFN(free_fn, exception_fn=None):
    def _fn(result, func, args):
        pystr = ctypes.cast(result, ctypes.c_char_p).value
        if pystr is None:
            _doExc(exception_fn, result, func, args, "null string returned")

        free_fn(result)
        return pystr
    return _fn

def errorOnNullPtrFN(exception_fn=None):
    def _fn(result, func, args):
        if not bool(result): # check for null pointer
            _doExc(exception_fn, result, func, args, "null string returned")
        return result
    return _fn


def errorOnZeroFN(exception_fn=None):
    def _fn(result, func, args):
        if result is None or result == 0:
            _doExc(exception_fn, result, func, args, "function returned error value of zero")
        return result
    return _fn
 
def errorOnNegativeFN(exception_fn=None):
    def _fn(result, func, args):
        if result is None or result < 0:
            _doExc(exception_fn, result, func, args, "function returned negative error code")
        return result
    return _fn


