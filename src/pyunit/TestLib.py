#! /usr/bin/env python2
# VIM declarations
# vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=python:

#alphabetical order
import os
import unittest

def usage():
    print "wrong command line options." #need better help eventually

def parseOptions(*args):
    pass

def runTests( testCase ):
    testToRun = 'test'

    myTestSuite = unittest.TestSuite()
    for i in testCase:
        try:
            temp = unittest.makeSuite( i, testToRun )
            myTestSuite.addTest(temp)
        except ValueError:
            pass

    runner = unittest.TextTestRunner( verbosity=3 )
    retval = runner.run(myTestSuite)

    return retval.wasSuccessful()

    #END main( testCase ):

main = runTests

def areFilesDifferent( orig, copy ):
    ret = os.system( "diff -q %s %s >/dev/null 2>&1" % (orig, copy) ) >> 8
    if ret == 0:
        return 0
    if ret == 1:
        return 1
    raise "something bad happened"

def copyFile( source, dest ):
    fileOrig = open( source, "r" )
    fileCopy = open( dest, "w" )

    line = fileOrig.read( 512 )
    while line != "":
        fileCopy.write(line)
        line = fileOrig.read( 512 )

    fileOrig.close()
    fileCopy.close()

if __name__ == "__main__":
    print "This file is not executable."
