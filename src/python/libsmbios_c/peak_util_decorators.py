from types import ClassType, FunctionType
import sys, os

__all__ = [
    'decorate_class', 'metaclass_is_decorator', 'metaclass_for_bases',
    'frameinfo', 'decorate_assignment', 'decorate', 'struct', 'classy',
    'template_function', 'rewrap', 'cache_source', 'enclosing_frame',
]

def decorate(*decorators):
    """Use Python 2.4 decorators w/Python 2.3+

    Example::

        class Foo(object):
            decorate(classmethod)
            def something(cls,etc):
                \"""This is a classmethod\"""

    You can pass in more than one decorator, and they are applied in the same
    order that would be used for ``@`` decorators in Python 2.4.

    This function can be used to write decorator-using code that will work with
    both Python 2.3 and 2.4 (and up).
    """
    if len(decorators)>1:
        decorators = list(decorators)
        decorators.reverse()

    def callback(frame,k,v,old_locals):
        for d in decorators:
            v = d(v)
        return v
    return decorate_assignment(callback)

def enclosing_frame(frame=None, level=3):
    """Get an enclosing frame that skips DecoratorTools callback code"""
    frame = frame or sys._getframe(level)
    while frame.f_globals.get('__name__')==__name__: frame = frame.f_back
    return frame

def name_and_spec(func):
    from inspect import formatargspec, getargspec
    funcname = func.__name__
    if funcname=='<lambda>':
        funcname = "anonymous"
    args, varargs, kwargs, defaults = getargspec(func)
    return funcname, formatargspec(args, varargs, kwargs)[1:-1]


def qname(func):
    m = func.__module__
    return m and m+'.'+func.__name__ or func.__name__


def apply_template(wrapper, func, *args, **kw):
    funcname, argspec  = name_and_spec(func)
    wrapname, wrapspec = name_and_spec(wrapper)
    body = wrapper.__doc__.replace('%','%%').replace('$args','%(argspec)s')
    d ={}
    body = """
def %(wrapname)s(%(wrapspec)s):
 def %(funcname)s(%(argspec)s): """+body+"""
 return %(funcname)s
"""
    body %= locals()
    filename = "<%s wrapping %s at 0x%08X>" % (qname(wrapper), qname(func), id(func))
    exec compile(body, filename, "exec") in func.func_globals, d

    f = d[wrapname](func, *args, **kw)
    cache_source(filename, body, f)

    f.func_defaults = func.func_defaults
    f.__doc__  = func.__doc__
    f.__dict__ = func.__dict__
    return f






def rewrap(func, wrapper):
    """Create a wrapper with the signature of `func` and a body of `wrapper`

    Example::

        def before_and_after(func):
            def decorated(*args, **kw):
                print "before"
                try:
                    return func(*args, **kw)
                finally:
                    print "after"
            return rewrap(func, decorated)

    The above function is a normal decorator, but when users run ``help()``
    or other documentation tools on the returned wrapper function, they will
    see a function with the original function's name, signature, module name,
    etc.

    This function is similar in use to the ``@template_function`` decorator,
    but rather than generating the entire decorator function in one calling
    layer, it simply generates an extra layer for signature compatibility.

    NOTE: the function returned from ``rewrap()`` will have the same attribute
    ``__dict__`` as the original function, so if you need to set any function
    attributes you should do so on the function returned from ``rewrap()``
    (or on the original function), and *not* on the wrapper you're passing in
    to ``rewrap()``.
    """
    def rewrap(__original, __decorated):
        """return __decorated($args)"""
    return apply_template(rewrap, func, wrapper)









if sys.version<"2.5":
    # We'll need this for monkeypatching linecache
    def checkcache(filename=None):
        """Discard cache entries that are out of date.
        (This is not checked upon each call!)"""
        if filename is None:
            filenames = linecache.cache.keys()
        else:
            if filename in linecache.cache:
                filenames = [filename]
            else:
                return
        for filename in filenames:
            size, mtime, lines, fullname = linecache.cache[filename]
            if mtime is None:
                continue   # no-op for files loaded via a __loader__
            try:
                stat = os.stat(fullname)
            except os.error:
                del linecache.cache[filename]
                continue
            if size != stat.st_size or mtime != stat.st_mtime:
                del linecache.cache[filename]


def _cache_lines(filename, lines, owner=None):
    if owner is None:
        owner = filename
    else:
        from weakref import ref
        owner = ref(owner, lambda r: linecache and linecache.cache.__delitem__(filename))
    global linecache; import linecache
    if sys.version<"2.5" and linecache.checkcache.__module__!=__name__:
        linecache.checkcache = checkcache
    linecache.cache[filename] = 0, None, lines, owner

def cache_source(filename, source, owner=None):
    _cache_lines(filename, source.splitlines(True), owner)



def template_function(wrapper=None):
    """Decorator that uses its wrapped function's docstring as a template

    Example::

        def before_and_after(func):
            @template_function
            def wrap(__func, __message):
                '''
                print "before", __message
                try:
                    return __func($args)
                finally:
                    print "after", __message
                '''
            return wrap(func, "test")

    The above code will return individually-generated wrapper functions whose
    signature, defaults, ``__name__``, ``__module__``, and ``func_globals``
    match those of the wrapped functions.

    You can use define any arguments you wish in the wrapping function, as long
    as the first argument is the function to be wrapped, and the arguments are
    named so as not to conflict with the arguments of the function being
    wrapped.  (i.e., they should have relatively unique names.)

    Note that the function body will *not* have access to the globals of the
    calling module, as it is compiled with the globals of the *wrapped*
    function!  Thus, any non-builtin values that you need in the wrapper should
    be passed in as arguments to the template function.
    """
    if wrapper is None:
        return decorate_assignment(lambda f,k,v,o: template_function(v))
    return apply_template.__get__(wrapper)







def struct(*mixins, **kw):
    """Turn a function into a simple data structure class

    This decorator creates a tuple subclass with the same name and docstring as
    the decorated function.  The class will have read-only properties with the
    same names as the function's arguments, and the ``repr()`` of its instances
    will look like a call to the original function.  The function should return
    a tuple of values in the same order as its argument names, as it will be
    used by the class' constructor.  The function can perform validation, add
    defaults, and/or do type conversions on the values.

    If the function takes a ``*``, argument, it should flatten this argument
    into the result tuple, e.g.::

        @struct()
        def pair(first, *rest):
            return (first,) + rest

    The ``rest`` property of the resulting class will thus return a tuple for
    the ``*rest`` arguments, and the structure's ``repr()`` will reflect the
    way it was created.

    The ``struct()`` decorator takes optional mixin classes (as positional
    arguments), and dictionary entries (as keyword arguments).  The mixin
    classes will be placed before ``tuple`` in the resulting class' bases, and
    the dictionary entries will be placed in the class' dictionary.  These
    entries take precedence over any default entries (e.g. methods, properties,
    docstring, etc.) that are created by the ``struct()`` decorator.
    """
    def callback(frame, name, func, old_locals):

        def __new__(cls, *args, **kw):
            result = func(*args, **kw)
            if type(result) is tuple:
                return tuple.__new__(cls, (cls,)+result)
            else:
                return result

        def __repr__(self):
            return name+tuple.__repr__(self[1:])

        import inspect
        args, star, dstar, defaults = inspect.getargspec(func)

        d = dict(
            __new__ = __new__, __repr__ = __repr__, __doc__=func.__doc__,
            __module__ = func.__module__, __args__ = args, __star__ = star,
            __slots__ = [],
        )

        for p,a in enumerate(args):
            if isinstance(a,str):
                d[a] = property(lambda self, p=p+1: self[p])

        if star:
            d[star] = property(lambda self, p=len(args)+1: self[p:])

        d.update(kw)
        return type(name, mixins+(tuple,), d)

    return decorate_assignment(callback)





















def frameinfo(frame):
    """Return (kind, module, locals, globals) tuple for a frame

    'kind' is one of "exec", "module", "class", "function call", or "unknown".
    """
    f_locals = frame.f_locals
    f_globals = frame.f_globals

    sameNamespace = f_locals is f_globals
    hasModule = '__module__' in f_locals
    hasName = '__name__' in f_globals
    sameName = hasModule and hasName
    sameName = sameName and f_globals['__name__']==f_locals['__module__']
    module = hasName and sys.modules.get(f_globals['__name__']) or None
    namespaceIsModule = module and module.__dict__ is f_globals

    if not namespaceIsModule:
        # some kind of funky exec
        kind = "exec"
        if hasModule and not sameNamespace:
            kind="class"
    elif sameNamespace and not hasModule:
        kind = "module"
    elif sameName and not sameNamespace:
        kind = "class"
    elif not sameNamespace:
        kind = "function call"
    else:
        # How can you have f_locals is f_globals, and have '__module__' set?
        # This is probably module-level code, but with a '__module__' variable.
        kind = "unknown"

    return kind,module,f_locals,f_globals








def decorate_class(decorator, depth=2, frame=None, allow_duplicates=False):

    """Set up `decorator` to be passed the containing class upon creation

    This function is designed to be called by a decorator factory function
    executed in a class suite.  The factory function supplies a decorator that
    it wishes to have executed when the containing class is created.  The
    decorator will be given one argument: the newly created containing class.
    The return value of the decorator will be used in place of the class, so
    the decorator should return the input class if it does not wish to replace
    it.

    The optional `depth` argument to this function determines the number of
    frames between this function and the targeted class suite.  `depth`
    defaults to 2, since this skips the caller's frame.  Thus, if you call this
    function from a function that is called directly in the class suite, the
    default will be correct, otherwise you will need to determine the correct
    depth value yourself.  Alternately, you can pass in a `frame` argument to
    explicitly indicate what frame is doing the class definition.

    This function works by installing a special class factory function in
    place of the ``__metaclass__`` of the containing class.  Therefore, only
    decorators *after* the last ``__metaclass__`` assignment in the containing
    class will be executed.  Thus, any classes using class decorators should
    declare their ``__metaclass__`` (if any) *before* specifying any class
    decorators, to ensure that all class decorators will be applied."""

    frame = enclosing_frame(frame, depth+1)
    kind, module, caller_locals, caller_globals = frameinfo(frame)

    if kind != "class":
        raise SyntaxError(
            "Class decorators may only be used inside a class statement"
        )
    elif not allow_duplicates and has_class_decorator(decorator, None, frame):
        return

    previousMetaclass = caller_locals.get('__metaclass__')
    defaultMetaclass  = caller_globals.get('__metaclass__', ClassType)


    def advise(name,bases,cdict):

        if '__metaclass__' in cdict:
            del cdict['__metaclass__']

        if previousMetaclass is None:
             if bases:
                 # find best metaclass or use global __metaclass__ if no bases
                 meta = metaclass_for_bases(bases)
             else:
                 meta = defaultMetaclass

        elif metaclass_is_decorator(previousMetaclass):
            # special case: we can't compute the "true" metaclass here,
            # so we need to invoke the previous metaclass and let it
            # figure it out for us (and apply its own advice in the process)
            meta = previousMetaclass

        else:
            meta = metaclass_for_bases(bases, previousMetaclass)

        newClass = meta(name,bases,cdict)

        # this lets the decorator replace the class completely, if it wants to
        return decorator(newClass)

    # introspection data only, not used by inner function
    # Note: these attributes cannot be renamed or it will break compatibility
    # with zope.interface and any other code that uses this decoration protocol
    advise.previousMetaclass = previousMetaclass
    advise.callback = decorator

    # install the advisor
    caller_locals['__metaclass__'] = advise


def metaclass_is_decorator(ob):
    """True if 'ob' is a class advisor function"""
    return isinstance(ob,FunctionType) and hasattr(ob,'previousMetaclass')


def iter_class_decorators(depth=2, frame=None):
    frame = enclosing_frame(frame, depth+1)
    m = frame.f_locals.get('__metaclass__')
    while metaclass_is_decorator(m):
        yield getattr(m, 'callback', None)
        m = m.previousMetaclass

def has_class_decorator(decorator, depth=2, frame=None):
    return decorator in iter_class_decorators(0, frame or sys._getframe(depth))
































def metaclass_for_bases(bases, explicit_mc=None):
    """Determine metaclass from 1+ bases and optional explicit __metaclass__"""

    meta = [getattr(b,'__class__',type(b)) for b in bases]

    if explicit_mc is not None:
        # The explicit metaclass needs to be verified for compatibility
        # as well, and allowed to resolve the incompatible bases, if any
        meta.append(explicit_mc)

    if len(meta)==1:
        # easy case
        return meta[0]

    classes = [c for c in meta if c is not ClassType]
    candidates = []

    for m in classes:
        for n in classes:
            if issubclass(n,m) and m is not n:
                break
        else:
            # m has no subclasses in 'classes'
            if m in candidates:
                candidates.remove(m)    # ensure that we're later in the list
            candidates.append(m)

    if not candidates:
        # they're all "classic" classes
        return ClassType

    elif len(candidates)>1:
        # We could auto-combine, but for now we won't...
        raise TypeError("Incompatible metatypes",bases)

    # Just one, return it
    return candidates[0]




def decorate_assignment(callback, depth=2, frame=None):
    """Invoke 'callback(frame,name,value,old_locals)' on next assign in 'frame'

    The frame monitored is determined by the 'depth' argument, which gets
    passed to 'sys._getframe()'.  When 'callback' is invoked, 'old_locals'
    contains a copy of the frame's local variables as they were before the
    assignment took place, allowing the callback to access the previous value
    of the assigned variable, if any.  The callback's return value will become
    the new value of the variable.  'name' is the name of the variable being
    created or modified, and 'value' is its value (the same as
    'frame.f_locals[name]').

    This function also returns a decorator function for forward-compatibility
    with Python 2.4 '@' syntax.  Note, however, that if the returned decorator
    is used with Python 2.4 '@' syntax, the callback 'name' argument may be
    'None' or incorrect, if the 'value' is not the original function (e.g.
    when multiple decorators are used).
    """
    frame = enclosing_frame(frame, depth+1)
    oldtrace = [frame.f_trace]
    old_locals = frame.f_locals.copy()

    def tracer(frm,event,arg):
        if event=='call':
            # We don't want to trace into any calls
            if oldtrace[0]:
                # ...but give the previous tracer a chance to, if it wants
                return oldtrace[0](frm,event,arg)
            else:
                return None

        try:
            if frm is frame and event !='exception':
                # Aha, time to check for an assignment...
                for k,v in frm.f_locals.items():
                    if k not in old_locals or old_locals[k] is not v:
                        break
                else:
                    # No luck, keep tracing
                    return tracer

                # Got it, fire the callback, then get the heck outta here...
                frm.f_locals[k] = callback(frm,k,v,old_locals)

        finally:
            # Give the previous tracer a chance to run before we return
            if oldtrace[0]:
                # And allow it to replace our idea of the "previous" tracer
                oldtrace[0] = oldtrace[0](frm,event,arg)

        uninstall()
        return oldtrace[0]

    def uninstall():
        # Unlink ourselves from the trace chain.
        frame.f_trace = oldtrace[0]
        sys.settrace(oldtrace[0])

    # Install the trace function
    frame.f_trace = tracer
    sys.settrace(tracer)

    def do_decorate(f):
        # Python 2.4 '@' compatibility; call the callback
        uninstall()
        frame = sys._getframe(1)
        return callback(
            frame, getattr(f,'__name__',None), f, frame.f_locals
        )

    return do_decorate











def super_next(cls, attr):
    for c in cls.__mro__:
        if attr in c.__dict__:
            yield getattr(c, attr).im_func

class classy_class(type):
    """Metaclass that delegates selected operations back to the class"""

    def __new__(meta, name, bases, cdict):
        cls = super(classy_class, meta).__new__(meta, name, bases, cdict)
        supr = super_next(cls, '__class_new__').next
        return supr()(meta, name, bases, cdict, supr)

    def __init__(cls, name, bases, cdict):
        supr = super_next(cls, '__class_init__').next
        return supr()(cls, name, bases, cdict, supr)

    def __call__(cls, *args, **kw):
        return cls.__class_call__.im_func(cls, *args, **kw)


class classy(object):
    """Base class for classes that want to be their own metaclass"""
    __metaclass__ = classy_class
    __slots__ = ()

    def __class_new__(meta, name, bases, cdict, supr):
        return type.__new__(meta, name, bases, cdict)

    def __class_init__(cls, name, bases, cdict, supr):
        return type.__init__(cls, name, bases, cdict)

    def __class_call__(cls, *args, **kw):
        return type.__call__(cls, *args, **kw)
    __class_call__ = classmethod(__class_call__)






