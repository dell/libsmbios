#!/usr/bin/python2
# vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=python:
"""
"""

from __future__ import generators

import sys
import TestLib

pagesize = (4096 * 16)

def getTempDir():
    import sys
    return sys.argv[1]

def getTestDir():
    import sys
    return sys.argv[2]

class TestCase(TestLib.TestCase):
    def setUp(self):
        # write some values to a test file that we will use for both cmos and mem tests
        self.testfile = "%s/%s-testmem.dat" % (getTempDir(), self._testMethodName)
        fd = open(self.testfile, "w+")
        for i in xrange(pagesize*4 + 1):
            fd.write("j")
        fd.close()

        import libsmbios_c.memory as m
        self.memObj = m.MemoryAccess(m.MEMORY_GET_NEW | m.MEMORY_UNIT_TEST_MODE, self.testfile)

        import libsmbios_c.cmos as c
        self.cmosObj = c.CmosAccess(c.CMOS_GET_NEW | c.CMOS_UNIT_TEST_MODE, self.testfile)

        # replace first 26 bytes with alphabet
        for i in xrange(26):
            self.memObj.write(chr(ord("a") + i), i)

        # write pages of '0', '1', and '2'
        for i in xrange(3):
            self.memObj.write(chr(ord("0") + i) * pagesize, 26 + (pagesize * i))

        self.memObj.close_hint(1)

    def tearDown(self):
        del(self.cmosObj)
        del(self.memObj)

    def testForLeaks(self):
        # create/destroy lots of objects to see if we leak
        import libsmbios_c.memory as m
        import libsmbios_c.cmos as c
        for i in xrange(1000):
            mObj = m.MemoryAccess(m.MEMORY_GET_NEW | m.MEMORY_UNIT_TEST_MODE, "/dev/null")
            cObj = c.CmosAccess(c.CMOS_GET_NEW | c.CMOS_UNIT_TEST_MODE, "/dev/null")
            del(mObj)
            del(cObj)

    def testMemoryWrite(self):
        # re-write first 26 bytes. starts with a-z, ends with A-z
        for i in xrange(26):
            b = self.memObj.read(i, 1)
            self.assertEquals( b[0], chr(ord("a") + i) )
            self.memObj.write( chr(ord(b[0]) + ord("A") - ord("a")), i)
            c = self.memObj.read(i, 1)
            self.assertEquals( c[0], chr(ord("A") + i) )

    def testMemoryReadMultipage(self):
        buf = self.memObj.read( 26, pagesize *3 )
        for i in xrange(3):
            for j in xrange(pagesize):
                self.assertEquals( buf[ i*pagesize + j ], chr(ord("0")+i) )

    def testMemorySearch(self):
        ret = self.memObj.search("abc", 0, 4096, 1);
        self.assertEquals( 0, ret );

        ret = self.memObj.search("de", 0, 4096, 1);
        self.assertEquals( 3, ret );

        ret = self.memObj.search("00000000", 0, 4096, 1);
        self.assertEquals( 26, ret );

        self.assertRaises(Exception, self.memObj.search, "nonexistent", 0, 4096, 1);

    def testCmosRead(self):
        for i in xrange(26):
            # index/data ports to 0 for unit testing
            b = self.cmosObj.readByte(0, 0, i)
            self.assertEquals( ord('a') + i, b )

    def testCmosWrite(self):
        import libsmbios_c.cmos as c
        import ctypes
        cObj = c._CmosAccess(c.CMOS_GET_NEW | c.CMOS_UNIT_TEST_MODE, self.testfile)

        # a test callback that increments a counter each time it is called
        def _test_cb(cmosObj, do_update, userdata):
            i = ctypes.cast(userdata, ctypes.POINTER(ctypes.c_uint16))
            i[0] = i[0] + 1
            return 1

        int = ctypes.c_uint16(0)
        cObj.registerCallback(_test_cb, ctypes.pointer(int), None)

        for i in xrange(26):
            # index/data ports to 0 for unit testing
            b = cObj.readByte(0, 0, i)
            self.assertEquals( ord('a') + i, b )
            cObj.writeByte( b + ord('A') - ord('a'), 0, 0, i )
            b = cObj.readByte(0, 0, i)
            self.assertEquals( ord('A') + i, b )

        self.assertEquals(int.value, 26)

        # index port 1 (offset 512 + i) should be '0'
        for i in xrange(26):
            c = cObj.readByte(1, 0, i)
            self.assertEquals(c, ord('0'))




if __name__ == "__main__":
    import TestLib
    sys.exit(not TestLib.runTests( [TestCase] ))
