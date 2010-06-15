#! /usr/bin/env python2
# VIM declarations
# vim:tw=0:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=python:

#alphabetical
import os
import sys
import glob

# all of the variables below are substituted by the build system
__VERSION__="1.0.0"

import TestLib

exeName = os.path.realpath(sys.argv[0])
top_srcdir = os.path.join(os.path.dirname(exeName), "..")
top_builddir = os.getcwd()

sys.path.insert(0,top_srcdir)

# runs all modules TestCase() classes in files that match test*.py
if __name__ == "__main__":
    testModulePath="%s/pyunit/" % top_srcdir

    moduleNames = glob.glob( "%s/test*.py" % testModulePath )
    moduleNames = [ m[len(testModulePath):-3] for m in moduleNames ]

    tests = []
    for moduleName in moduleNames:
        if "testAll" in moduleName:
            continue
        module = __import__(moduleName, globals(), locals(), [])
        module.TestCase.top_srcdir=top_srcdir
        module.TestCase.top_builddir=top_builddir
        tests.append(module.TestCase)

    retval = 1
    if tests:
        retval = TestLib.runTests( tests )

    sys.exit( not retval )
