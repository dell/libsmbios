#!/usr/bin/python3
# vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=python:
"""
"""



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
        for i in range(pagesize*4 + 1):
            fd.write("j")
        fd.close()
        self.testfile = self.testfile.encode('utf-8')

        import libsmbios_c.memory as m
        self.memObj = m.MemoryAccess(m.MEMORY_GET_NEW | m.MEMORY_UNIT_TEST_MODE, self.testfile)

        import libsmbios_c.cmos as c
        self.cmosObj = c.CmosAccess(c.CMOS_GET_NEW | c.CMOS_UNIT_TEST_MODE, self.testfile)

        # replace first 26 bytes with alphabet
        for i in range(26):
            self.memObj.write(chr(ord("a") + i).encode('utf-8'), i)

        # write pages of '0', '1', and '2'
        for i in range(3):
            self.memObj.write(chr(ord("0") + i).encode('utf-8') * pagesize, 26 + (pagesize * i))

        self.memObj.close_hint(1)

    def tearDown(self):
        del(self.cmosObj)
        del(self.memObj)

    def testForLeaks(self):
        # create/destroy lots of objects to see if we leak
        import libsmbios_c.memory as m
        import libsmbios_c.cmos as c
        for i in range(1000):
            mObj = m.MemoryAccess(m.MEMORY_GET_NEW | m.MEMORY_UNIT_TEST_MODE, "/dev/null")
            cObj = c.CmosAccess(c.CMOS_GET_NEW | c.CMOS_UNIT_TEST_MODE, "/dev/null")
            del(mObj)
            del(cObj)

    def testMemoryWrite(self):
        # re-write first 26 bytes. starts with a-z, ends with A-z
        for i in range(26):
            b = self.memObj.read(i, 1)
            self.assertEqual( b[0].decode('utf-8'), chr(ord("a") + i) )
            self.memObj.write( chr(ord(b[0]) + ord("A") - ord("a")), i)
            c = self.memObj.read(i, 1)
            self.assertEqual( c[0].decode('utf-8'), chr(ord("A") + i) )

    def testMemoryReadMultipage(self):
        buf = self.memObj.read( 26, pagesize *3 )
        for i in range(3):
            for j in range(pagesize):
                self.assertEqual( buf[ i*pagesize + j ], chr(ord("0")+i).encode('utf-8') )

    def testMemorySearch(self):
        ret = self.memObj.search("abc".encode("utf-8"), 0, 4096, 1);
        self.assertEqual( 0, ret );

        ret = self.memObj.search("de".encode("utf-8"), 0, 4096, 1);
        self.assertEqual( 3, ret );

        ret = self.memObj.search("00000000".encode("utf-8"), 0, 4096, 1);
        self.assertEqual( 26, ret );

        self.assertRaises(Exception, self.memObj.search, "nonexistent", 0, 4096, 1);

    def testCmosRead(self):
        for i in range(26):
            # index/data ports to 0 for unit testing
            b = self.cmosObj.readByte(0, 0, i)
            self.assertEqual( ord('a') + i, b )

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

        for i in range(26):
            # index/data ports to 0 for unit testing
            b = cObj.readByte(0, 0, i)
            self.assertEqual( ord('a') + i, b )
            cObj.writeByte( b + ord('A') - ord('a'), 0, 0, i )
            b = cObj.readByte(0, 0, i)
            self.assertEqual( ord('A') + i, b )

        self.assertEqual(int.value, 26)

        # index port 1 (offset 512 + i) should be '0'
        for i in range(26):
            c = cObj.readByte(1, 0, i)
            self.assertEqual(c, ord('0'))




if __name__ == "__main__":
    import TestLib
    sys.exit(not TestLib.runTests( [TestCase] ))
