#! /usr/bin/env python
# VIM declarations
# vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=python:

#must be first
from __future__ import division

import re
import sys
import glob
import os

def processFile( filename ):
    pct = 0
    lineCount = 0
    linesOfCode = 0
    statementsExecuted = 0

    fd = file( filename, "r" )
    while 1:
        line = fd.readline()
        if line == "":
            break

        lineCount = lineCount + 1

        p = re.match( "^\s*-:", line )
        if p is not None:
            continue

        p = re.match( "^\s{0,9}\d+:", line )
        if p is not None:
            linesOfCode = linesOfCode + 1
            statementsExecuted = statementsExecuted + 1
            continue

        p = re.match( "^    #####:", line )
        if p is not None:
            linesOfCode = linesOfCode + 1

    if linesOfCode == 0:
        pct = 1
    else:
        pct = statementsExecuted / linesOfCode

    return ( pct, lineCount, linesOfCode, statementsExecuted )


def main():
    totalLines = 0
    totalLoc = 0
    totalExc = 0
    path = os.path.join(sys.argv[1],  "*.gcov")

    fileList = glob.glob( path )
    fileList.sort()

    print " PCT\tLINES\tCODE\tEXEC\tFILENAME"
    for file in fileList:
        (pct, lines, loc, exc) = processFile( file )
        totalLines = totalLines + lines
        totalLoc = totalLoc + loc
        totalExc = totalExc + exc
        file = os.path.basename(file)
        file = file[:-5]
        print "%3.0f%%\t%d\t%d\t%d\t%s" % ( (pct*100), lines, loc, exc, file )

    if totalLoc == 0:
        pct = 1
    else:
        pct = totalExc / totalLoc
    print
    print "%3.0f%%\t%d\t%d\t%d\t%s" % ( (pct*100), totalLines, totalLoc, totalExc, "TOTAL" )

if __name__ == "__main__":
    main()
