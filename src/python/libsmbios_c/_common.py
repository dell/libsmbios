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
import ctypes

from trace_decorator import decorate, traceLog, getLog

__all__ = ["freeLibStringFN", "errorOnZeroFN", "errorOnNegativeFN", "errorOnNullPtrFN" ]

class Exception(exceptions.Exception): pass

def _doExc(exception_fn, r, f, a, msg):
    if exception_fn is not None:
        raise exception_fn(r, f, a)
    else:
        raise Exception( msg )

def freeLibStringFN(free_fn, exception_fn=None):
    decorate(traceLog())
    def _freeLibStringFN(result, func, args):
        getLog(prefix="trace.").info("RAN CTYPES FUNCTION: %s" % func.__name__)
        pystr = ctypes.cast(result, ctypes.c_char_p).value
        if pystr is None:
            pystr = ""
            #_doExc(exception_fn, result, func, args, _("null string returned") )

        free_fn(result)
        return pystr
    return _freeLibStringFN

def errorOnNullPtrFN(exception_fn=None):
    decorate(traceLog())
    def _errorOnNullPtrFN(result, func, args):
        getLog(prefix="trace.").info("RAN CTYPES FUNCTION: %s" % func.__name__)
        if not bool(result): # check for null pointer
            _doExc(exception_fn, result, func, args, _("null pointer returned") )
        return result
    return _errorOnNullPtrFN

def errorOnZeroFN(exception_fn=None):
    decorate(traceLog())
    def _errorOnZeroFN(result, func, args):
        getLog(prefix="trace.").info("RAN CTYPES FUNCTION: %s" % func.__name__)
        if result is None or result == 0:
            _doExc(exception_fn, result, func, args, _("function returned error value of zero") )
        return result
    return _errorOnZeroFN
 
def errorOnNegativeFN(exception_fn=None):
    decorate(traceLog())
    def _errorOnNegativeFN(result, func, args):
        getLog(prefix="trace.").info("RAN CTYPES FUNCTION: %s" % func.__name__)
        if result is None or result < 0:
            _doExc(exception_fn, result, func, args, _("function returned negative error code") )
        return result
    return _errorOnNegativeFN


