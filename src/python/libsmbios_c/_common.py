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

__all__ = ["freeLibStringFN", "errorOnZeroFN", "errorOnNegativeFN"]

class Exception(exceptions.Exception): pass

def freeLibStringFN(free_fn, exception_fn=None):
    def _fn(result, func, args):
        pystr = ctypes.cast(result, ctypes.c_char_p).value
        if pystr is None:
            if exception_fn is not None:
                raise exception_fn()
            else:
                raise Exception("null string returned")

        free_fn(result)
        return pystr
    return _fn

def errorOnZeroFN(exception_fn=None):
    def _fn(result, func, args):
        if result is None or result == 0:
            if exception_fn is not None:
                raise exception_fn(result, func, args)
            else:
                raise Exception, ("returned error")
        return result
    return _fn
 
def errorOnNegativeFN(exception_fn=None):
    def _fn(result, func, args):
        if result is None or result < 0:
            if exception_fn is not None:
                raise exception_fn(result, func, args)
            else:
                raise Exception, ("returned error")
        return result
    return _fn


