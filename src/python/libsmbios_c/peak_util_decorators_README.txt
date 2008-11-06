Class, Function, and Assignment Decorators for Python 2.3+
==========================================================

Want to use decorators, but still need to support Python 2.3?  Wish you could
have class decorators, decorate arbitrary assignments, or match decorated
function signatures to their original functions?  Then you need
"DecoratorTools".  Some quick examples::

    # Method decorator example
    from peak.util.decorators import decorate

    class Demo1(object):
        decorate(classmethod)   # equivalent to @classmethod
        def example(cls):
            print "hello from", cls


    # Class decorator example
    from peak.util.decorators import decorate_class

    def my_class_decorator():
        def decorator(cls):
            print "decorating", cls
            return cls
        decorate_class(decorator)

    class Demo2:
        my_class_decorator()

    # "decorating <class Demo2>" will be printed when execution gets here


Installing DecoratorTools (using ``"easy_install DecoratorTools"`` or
``"setup.py install"``) gives you access to the ``peak.util.decorators``
module.  The tools in this module have been bundled for years inside of PEAK,
PyProtocols, RuleDispatch, and the zope.interface package, so they have been
widely used and tested.  (Unit tests are also included, of course.)

This standalone version is backward-compatible with the bundled versions, so you
can mix and match decorators from this package with those provided by
zope.interface, TurboGears, etc.

For complete documentation, see the `DecoratorTools manual`_.

Changes since version 1.5:

  * Added ``classy`` base class that allows you to do the most often-needed
    metaclass behviors *without* needing an actual metaclass.

Changes since version 1.4:

  * Added ``enclosing_frame()`` function, so that complex decorators that call
    DecoratorTools functions while being called *by* DecoratorTools functions,
    will work correctly.

Changes since version 1.3:

  * Added support for debugging generated code, including the code generated
    by ``rewrap()`` and ``template_function``.

Changes since version 1.2:

  * Added ``rewrap()`` function and ``template_function`` decorator to support
    signature matching for decorated functions.  (These features are similar to
    the ones provided by Michele Simionato's "decorator" package, but do not
    require Python 2.4 and don't change the standard idioms for creating
    decorator functions.)

  * ``decorate_class()`` will no longer apply duplicate class decorator
    callbacks unless the ``allow_duplicates`` argument is true.

Changes since version 1.1:

  * Fixed a problem where instances of different struct types could equal each
    other

Changes since version 1.0:

  * The ``struct()`` decorator makes it easy to create tuple-like data
    structure types, by decorating a constructor function.

.. _DecoratorTools Manual: http://peak.telecommunity.com/DevCenter/DecoratorTools#toc

.. _toc:

.. contents:: **Table of Contents**

You may access any of the following APIs by importing them from
``peak.util.decorators``:


Simple Decorators
-----------------

decorate(\*decorators)
    Apply `decorators` to the subsequent function definition or assignment
    statement, thereby allowing you to conviently use standard decorators with
    Python 2.3 and up (i.e., no ``@`` syntax required), as shown in the
    following table of examples::

        Python 2.4+               DecoratorTools
        ------------              --------------
        @classmethod              decorate(classmethod)
        def blah(cls):            def blah(cls):
            pass                      pass

        @foo
        @bar(baz)                 decorate(foo, bar(baz))
        def spam(bing):           def spam(bing):
            """whee"""                """whee"""

decorate_class(decorator [, depth=2, frame=None])
    Set up `decorator` to be passed the containing class after its creation.

    This function is designed to be called by a decorator factory function
    executed in a class suite.  It is not used directly; instead you simply
    give your users a "magic function" to call in the body of the appropriate
    class.  Your "magic function" (i.e. a decorator factory function) then
    calls ``decorate_class`` to register the decorator to be called when the
    class is created.  Multiple decorators may be used within a single class,
    although they must all appear *after* the ``__metaclass__`` declaration, if
    there is one.

    The registered decorator will be given one argument: the newly created
    containing class.  The return value of the decorator will be used in place
    of the original class, so the decorator should return the input class if it
    does not wish to replace it.  Example::

        >>> from peak.util.decorators import decorate_class

        >>> def demo_class_decorator():
        ...     def decorator(cls):
        ...         print "decorating", cls
        ...         return cls
        ...     decorate_class(decorator)

        >>> class Demo:
        ...     demo_class_decorator()
        decorating __builtin__.Demo

    In the above example, ``demo_class_decorator()`` is the decorator factory
    function, and its inner function ``decorator`` is what gets called to
    actually decorate the class.  Notice that the factory function has to be
    called within the class body, even if it doesn't take any arguments.

    If you are just creating simple class decorators, you don't need to worry
    about the `depth` or `frame` arguments here.  However, if you are creating
    routines that are intended to be used within other class or method
    decorators, you will need to pay attention to these arguments to ensure
    that ``decorate_class()`` can find the frame where the class is being
    defined.  In general, the simplest way to do this is for the function
    that's called in the class body to get its caller's frame with
    ``sys._getframe(1)``, and then pass that frame down to whatever code will
    be calling ``decorate_class()``.  Alternately, you can specify the `depth`
    that ``decorate_class()`` should call ``sys._getframe()`` with, but this
    can be a bit trickier to compute correctly.

    Note, by the way that ``decorate_class()`` ignores duplicate callbacks::

        >>> def hello(cls):
        ...     print "decorating", cls
        ...     return cls

        >>> def do_hello():
        ...     decorate_class(hello)

        >>> class Demo:
        ...     do_hello()
        ...     do_hello()
        decorating __builtin__.Demo

    Unless the ``allow_duplicates`` argument is set to a true value::

        >>> def do_hello():
        ...     decorate_class(hello, allow_duplicates=True)

        >>> class Demo:
        ...     do_hello()
        ...     do_hello()
        decorating __builtin__.Demo
        decorating __builtin__.Demo


The ``struct()`` Decorator
--------------------------

The ``struct()`` decorator creates a tuple subclass with the same name and
docstring as the decorated function.  The class will have read-only properties
with the same names as the function's arguments, and the ``repr()`` of its
instances will look like a call to the original function::

    >>> from peak.util.decorators import struct

    >>> def X(a,b,c):
    ...     """Demo type"""
    ...     return a,b,c

    >>> X = struct()(X)    # can't use decorators above functions in doctests

    >>> v = X(1,2,3)
    >>> v
    X(1, 2, 3)
    >>> v.a
    1
    >>> v.b
    2
    >>> v.c
    3

    >>> help(X) # doctest: +NORMALIZE_WHITESPACE
    Help on class X:
    <BLANKLINE>
    class X(__builtin__.tuple)
     |  Demo type
     |
     |  Method resolution order:
     |      X
     |      __builtin__.tuple
     |      __builtin__.object
     |
     |  Methods defined here:
     |
     |  __repr__(self)
     |
     |  ----------------------------------------------------------------------
     |  Static methods defined here:
     |
     |  __new__(cls, *args, **kw)
     |
     |  ----------------------------------------------------------------------
     |  ...s defined here:
     |
     |  a...
     |
     |  b...
     |
     |  c...
     |
     |  ----------------------------------------------------------------------
     |  Data and other attributes defined here:
     |
     |  __args__ = ['a', 'b', 'c']...
     |
     |  __star__ = None
     |
     |  ...

The function should return a tuple of values in the same order as its argument
names, as it will be used by the class' constructor. The function can perform
validation, add defaults, and/or do type conversions on the values.

If the function takes a ``*``, argument, it should flatten this argument
into the result tuple, e.g.::

    >>> def pair(first, *rest):
    ...     return (first,) + rest
    >>> pair = struct()(pair)

    >>> p = pair(1,2,3,4)
    >>> p
    pair(1, 2, 3, 4)
    >>> p.first
    1
    >>> p.rest
    (2, 3, 4)

Internally, ``struct`` types are actually tuples::

    >>> print tuple.__repr__(X(1,2,3))
    (<class 'X'>, 1, 2, 3)

The internal representation contains the struct's type object, so that structs
of different types will not compare equal to each other::

    >>> def Y(a,b,c):
    ...     return a,b,c
    >>> Y = struct()(Y)

    >>> X(1,2,3) == X(1,2,3)
    True
    >>> Y(1,2,3) == Y(1,2,3)
    True
    >>> X(1,2,3) == Y(1,2,3)
    False

Note, however, that this means that if you want to unpack them or otherwise
access members directly, you must include the type entry, or use a slice::

    >>> a, b, c = X(1,2,3)  # wrong
    Traceback (most recent call last):
      ...
    ValueError: too many values to unpack

    >>> t, a, b, c = X(1,2,3)       # right
    >>> a, b, c    = X(1,2,3)[1:]   # ok, if perhaps a bit unintuitive

The ``struct()`` decorator takes optional mixin classes (as positional
arguments), and dictionary entries (as keyword arguments).  The mixin
classes will be placed before ``tuple`` in the resulting class' bases, and
the dictionary entries will be placed in the class' dictionary.  These
entries take precedence over any default entries (e.g. methods, properties,
docstring, etc.) that are created by the ``struct()`` decorator::

    >>> class Mixin(object):
    ...     __slots__ = []
    ...     def foo(self): print "bar"

    >>> def demo(a, b):
    ...     return a, b

    >>> demo = struct(Mixin, reversed=property(lambda self: self[:0:-1]))(demo)
    >>> demo(1,2).foo()
    bar
    >>> demo(3,4).reversed
    (4, 3)
    >>> demo.__mro__
    (<class 'demo'>, <class ...Mixin...>, <type 'tuple'>, <type 'object'>)

Note that using mixin classes will result in your new class' instances having
a ``__dict__`` attribute, unless they are new-style classes that set
``__slots__`` to an empty list.  And if they have any slots other than
``__weakref__`` or ``__dict__``, this will cause a type error due to layout
conflicts.  In general, it's best to use mixins only for adding methods, not
data.

Finally, note that if your function returns a non-tuple result, it will be
returned from the class' constructor.  This is sometimes useful::

    >>> def And(a, b):
    ...     if a is None: return b
    ...     return a, b
    >>> And = struct()(And)

    >>> And(1,2)
    And(1, 2)

    >>> And(None, 27)
    27


Signature Matching
------------------

One of the drawbacks to using function decorators is that using ``help()`` or
other documentation tools on a decorated function usually produces unhelpful
results::

    >>> def before_and_after(message):
    ...     def decorator(func):
    ...         def decorated(*args, **kw):
    ...             print "before", message
    ...             try:
    ...                 return func(*args, **kw)
    ...             finally:
    ...                 print "after", message
    ...         return decorated
    ...     return decorator

    >>> def foo(bar, baz):
    ...     """Here's some doc"""

    >>> foo(1,2)
    >>> help(foo)               # doctest: -NORMALIZE_WHITESPACE
    Help on function foo:
    ...
    foo(bar, baz)
        Here's some doc
    ...

    >>> decorated_foo = before_and_after("hello")(foo)
    >>> decorated_foo(1,2)
    before hello
    after hello

    >>> help(decorated_foo)     # doctest: -NORMALIZE_WHITESPACE
    Help on function decorated:
    ...
    decorated(*args, **kw)
    ...

So DecoratorTools provides you with two tools to improve this situation.
First, the ``rewrap()`` function provides a simple way to match the signature,
module, and other characteristics of the original function::

    >>> from peak.util.decorators import rewrap

    >>> def before_and_after(message):
    ...     def decorator(func):
    ...         def before_and_after(*args, **kw):
    ...             print "before", message
    ...             try:
    ...                 return func(*args, **kw)
    ...             finally:
    ...                 print "after", message
    ...         return rewrap(func, before_and_after)
    ...     return decorator

    >>> decorated_foo = before_and_after("hello")(foo)
    >>> decorated_foo(1,2)
    before hello
    after hello

    >>> help(decorated_foo)     # doctest: -NORMALIZE_WHITESPACE
    Help on function foo:
    ...
    foo(bar, baz)
        Here's some doc
    ...

The ``rewrap()`` function returns you a new function object with the same
attributes (including ``__doc__``, ``__dict__``, ``__name__``, ``__module__``,
etc.) as the original function, but which calls the decorated function.

If you want the same signature but don't want the overhead of another calling
level at runtime, you can use the ``@template_function`` decorator instead.
The downside to this approach, however, is that it is more complex to use.  So,
this approach is only recommended for more performance-intensive decorators,
that you've already debugged using the ``rewrap()`` approach.  But if you need
to use it, the appropriate usage looks something like this::

    >>> from peak.util.decorators import template_function

    >>> def before_and_after2(message):
    ...     def decorator(func):
    ...         [template_function()]   # could also be @template_function in 2.4
    ...         def before_and_after2(__func, __message):
    ...             '''
    ...             print "before", __message
    ...             try:
    ...                 return __func($args)
    ...             finally:
    ...                 print "after", __message
    ...             '''
    ...         return before_and_after2(func, message)
    ...     return decorator

    >>> decorated_foo = before_and_after2("hello")(foo)
    >>> decorated_foo(1,2)
    before hello
    after hello

    >>> help(decorated_foo)     # doctest: -NORMALIZE_WHITESPACE
    Help on function foo:
    ...
    foo(bar, baz)
        Here's some doc
    ...

As you can see, the process is somewhat more complex.  Any values you wish
the generated function to be able to access (aside from builtins) must be
declared as arguments to the decorating function, and all arguments must be
named so as not to conflict with the names of any of the decorated function's
arguments.  The docstring must either fit on one line, or begin with a newline
and have its contents indented by at least two spaces.  The string ``$args``
may be used one or more times in the docstring, whenever calling the original
function.  The first argument of the decorating function must always be the
original function.


Debugging Generated Code
------------------------

Both ``rewrap()`` and ``template_function`` are implemented using code
generation and runtime compile/exec operations.  Normally, such things are
frowned on in Python because Python's debugging tools don't work on generated
code.  In particular, tracebacks and pdb don't show the source code of
functions compiled from strings...   or do they?  Let's see::

    >>> def raiser(x, y="blah"):
    ...     raise TypeError(y)

    >>> def call_and_print_error(func, *args, **kw):
    ...     # This function is necessary because we want to test the error
    ...     # output, but doctest ignores a lot of exception detail, and
    ...     # won't show the non-errror output unless we do it this way
    ...     #
    ...     try:
    ...         func(*args, **kw)
    ...     except:
    ...         import sys, traceback
    ...         print ''.join(traceback.format_exception(*sys.exc_info()))

    >>> call_and_print_error(before_and_after("error")(raiser), 99)
    before error
    after error
    Traceback (most recent call last):
      File "<doctest README.txt[...]>", line ..., in call_and_print_error
        func(*args, **kw)
      File "<peak.util.decorators.rewrap wrapping raiser at 0x...>", line 3, in raiser
        def raiser(x, y): return __decorated(x, y)
      File ..., line ..., in before_and_after
        return func(*args, **kw)
      File "<doctest README.txt[...]>", line 2, in raiser
        raise TypeError(y)
    TypeError: blah

    >>> call_and_print_error(before_and_after2("error")(raiser), 99)
    before error
    after error
    Traceback (most recent call last):
      File "<doctest README.txt[...]>", line ..., in call_and_print_error
        func(*args, **kw)
      File "<before_and_after2 wrapping raiser at 0x...>", line 6, in raiser
        return __func(x, y)
      File "<doctest README.txt[...]>", line 2, in raiser
        raise TypeError(y)
    TypeError: blah

As you can see, both decorators' tracebacks include lines from the pseudo-files
"<peak.util.decorators.rewrap wrapping raiser at 0x...>" and "<before_and_after2
wrapping raiser at 0x...>" (the hex id's of the corresponding objects are
omitted here).  This is because DecoratorTools adds information to the Python
``linecache`` module, and tracebacks and pdb both use the ``linecache`` module
to get source lines.  Any tools that use ``linecache``, either directly or
indirectly, will therefore be able to display this information for generated
code.

If you'd like to be able to use this feature for your own code generation or
non-file-based code (e.g. Python source loaded from a database, etc.), you can
use the ``cache_source()`` function::

    >>> from peak.util.decorators import cache_source
    >>> from linecache import getline

    >>> demo_source = "line 1\nline 2\nline 3"

    >>> cache_source("<dummy filename 1>", demo_source)
    >>> getline("<dummy filename 1>", 3)
    'line 3'

The function requires a dummy filename, which must be globally unique.  An easy
way to ensure uniqueness is to include the ``id()`` of an object that will
exist at least as long as the source code being cached.

Also, if you have such an object, and it is weak-referenceable, you can supply
it as a third argument to ``cache_source()``, and when that object is garbage
collected the source will be removed from the ``linecache`` cache.  If you're
generating a function from the source, the function object itself is ideal for
this purpose (and it's what ``rewrap()`` and ``template_function`` do)::

    >>> def a_function(): pass  # just an object to "own" the source

    >>> cache_source("<dummy filename 2>", demo_source, a_function)
    >>> getline("<dummy filename 2>", 1)
    'line 1\n'

    >>> del a_function  # GC should now clean up the cache

    >>> getline("<dummy filename 2>", 1)
    ''


Advanced Decorators
-------------------

The ``decorate_assignment()`` function can be used to create standalone "magic"
decorators that work in Python 2.3 and up, and which can also be used to
decorate arbitrary assignments as well as function/method definitions.  For
example, if you wanted to create an ``info(**kwargs)`` decorator that could be
used either with or without an ``@``, you could do something like::

    from peak.util.decorators import decorate_assignment

    def info(**kw):
        def callback(frame, name, func, old_locals):
            func.__dict__.update(kw)
            return func
        return decorate_assignment(callback)

    info(foo="bar")     # will set dummy.foo="bar"; @info() would also work
    def dummy(blah):
        pass

As you can see, this ``info()`` decorator can be used without an ``@`` sign
for backward compatibility with Python 2.3.  It can also be used *with* an
``@`` sign, for forward compatibility with Python 2.4 and up.

Here's a more detailed reference for the ``decorate_assignment()`` API:

decorate_assignment(callback [, depth=2, frame=None])
    Call `callback(frame, name, value, old_locals)` on next assign in `frame`.

    If a `frame` isn't supplied, a frame is obtained using
    ``sys._getframe(depth)``.  `depth` defaults to 2 so that the correct frame
    is found when ``decorate_assignment()`` is called from a decorator factory
    that was called in the target usage context.

    When `callback` is invoked, `old_locals` contains the frame's local
    variables as they were *before* the assignment, thus allowing the callback
    to access the previous value of the assigned variable, if any.

    The callback's return value will become the new value of the variable.
    `name` will contain the name of the variable being created or modified,
    and `value` will be the thing being decorated.  `frame` is the Python frame
    in which the assignment occurred.

    This function also returns a decorator function for forward-compatibility
    with Python 2.4 ``@`` syntax.  Note, however, that if the returned decorator
    is used with Python 2.4 ``@`` syntax, the callback `name` argument may be
    ``None`` or incorrect, if the `value` is not the original function (e.g.
    when multiple decorators are used).


"Meta-less" Classes
-------------------

Sometimes, you want to create a base class in a library or program that will
use the data defined in subclasses in some way, or that needs to customize
the way instances are created (*without* overriding ``__new__``).

Since Python 2.2, the standard way to accomplish these things is by creating
a custom metaclass and overriding ``__new__``, ``__init__``, or ``__call__``.

Unfortunately, however, metaclasses don't play well with others.  If two
frameworks define independent metaclasses, and a library or application mixes
classes from those frameworks, the user will have to create a *third* metaclass
to sort out the differences.  For this reason, it's best to minimize the number
of distinct metaclasses in use.

``peak.util.decorators`` therefore provides a kind of "one-size-fits-all"
metaclass, so that most of the common use cases for metaclasses can be handled
with just one metaclass.  In PEAK and elsewhere, metaclasses are most commonly
used to perform some sort of operations during class creation (metaclass
``__new__`` and ``__init__``), or instance creation (metaclass ``__call__``,
wrapping the class-level ``__new__`` and ``__init__``).

Therefore, the ``classy`` base class allows subclasses to implement one or more
of the three classmethods ``__class_new__``, ``__class_init__``, and
``__class_call__``.  The "one-size-fits-all" metaclass delegates these
operations to the class, so that you don't need a custom metaclass for every
class with these behaviors.

Thus, as long as all your custom metaclasses derive from ``classy.__class__``,
you can avoid any metaclass conflicts during multiple inheritance.

Here's an example of ``classy`` in use::

    >>> from peak.util.decorators import classy, decorate

    >>> class Demo(classy):
    ...     """Look, ma!  No metaclass!"""
    ...
    ...     def __class_new__(meta, name, bases, cdict, supr):
    ...         cls = supr()(meta, name, bases, cdict, supr)
    ...         print "My metaclass is", meta
    ...         print "And I am", cls
    ...         return cls
    ...
    ...     def __class_init__(cls, name, bases, cdict, supr):
    ...         supr()(cls, name, bases, cdict, supr)
    ...         print "Initializing", cls
    ...
    ...     decorate(classmethod)   # could be just @classmethod for 2.4+
    ...     def __class_call__(cls, *args, **kw):
    ...         print "before creating instance"
    ...         ob = super(Demo, cls).__class_call__(*args, **kw)
    ...         print "after creating instance"
    ...         return ob
    ...
    ...     def __new__(cls, *args, **kw):
    ...         print "new called with", args, kw
    ...         return super(Demo, cls).__new__(cls, *args, **kw)
    ...
    ...     def __init__(self, *args, **kw):
    ...         print "init called with", args, kw
    My metaclass is <class 'peak.util.decorators.classy_class'>
    And I am <class 'Demo'>
    Initializing <class 'Demo'>

    >>> d = Demo(1,2,a="b")
    before creating instance
    new called with (1, 2) {'a': 'b'}
    init called with (1, 2) {'a': 'b'}
    after creating instance

Note that because ``__class_new__`` and ``__class_init__`` are called *before*
the name ``Demo`` has been bound to the class under creation, ``super()``
cannot be used in these methods.  So, they use a special calling convention,
where the last argument (``supr``) is the ``next()`` method of an iterator
that yields base class methods in mro order.  In other words, calling
``supr()(..., supr)`` invokes the previous definition of the method.  You MUST
call this exactly *once* in your methods -- no more, no less.

``__class_call__`` is different, because it is called after the class already
exists.  Thus, it can be a normal ``classmethod`` and use ``super()`` in the
standard way.

Finally, note that any given ``classy`` subclass does NOT need to define all
three methods; you can mix and match methods as needed.  Just be sure to always
use the ``supr`` argument (or ``super()`` in the case of ``__class_call__``).


Utility/Introspection Functions
-------------------------------

``peak.util.decorators`` also exposes these additional utility and
introspection functions that it uses internally:

frameinfo(frame)
    Return a ``(kind, module, locals, globals)`` tuple for a frame

    The `kind` returned is a string, with one of the following values:

    * ``"exec"``
    * ``"module"``
    * ``"class"``
    * ``"function call"``
    * ``"unknown"``

    The `module` returned is the Python module object whose globals are in
    effect for the frame, or ``None`` if the globals don't include a value for
    ``__name__``.

metaclass_is_decorator(mc)
    Return truth if the given metaclass is a class decorator metaclass inserted
    into a class by ``decorate_class()``, or by another class decorator
    implementation that follows the same protocol (such as the one in
    ``zope.interface``).

metaclass_for_bases(bases, explicit_mc=None)
    Given a sequence of 1 or more base classes and an optional explicit
    ``__metaclass__``, return the metaclass that should be used.  This
    routine basically emulates what Python does to determine the metaclass
    when creating a class, except that it does *not* take a module-level
    ``__metaclass__`` into account, only the arguments as given.  If there
    are no base classes, you should just directly use the module-level
    ``__metaclass__`` or ``types.ClassType`` if there is none.

enclosing_frame(frame=None, level=3)
    Given a frame and/or stack level, skip upward past any DecoratorTools code
    frames.  This function is used by ``decorate_class()`` and
    ``decorate_assignment()`` to ensure that any decorators calling them that
    were themselves invoked using ``decorate()``, won't end up looking at
    DecoratorTools code instead of the target.  If you have a function that
    needs to be callable via ``decorate()`` and which inspects stack frames,
    you may need to use this function to access the right frame.


Mailing List
------------

Please direct questions regarding this package to the PEAK mailing list; see
http://www.eby-sarna.com/mailman/listinfo/PEAK/ for details.
