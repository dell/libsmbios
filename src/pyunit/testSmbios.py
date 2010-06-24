#!/usr/bin/python2
# vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=python:
"""
"""

from __future__ import generators

import os
import sys
import xml.dom.minidom
import HelperXml
import TestLib

pagesize = (4096 * 16)

class SkipTest(Exception): pass

def getTempDir():
    import sys
    return sys.argv[1]

def getTestDir():
    import sys
    return sys.argv[2]

class TestCase(TestLib.TestCase):
    def checkSkip(self):
        for testElem in HelperXml.iterNodeElement(self.dom, "TESTINPUT", "testsToSkip"):
            if self._testMethodName == HelperXml.getNodeAttribute(testElem, "name"):
                self.skip=1

    def setUp(self):
        self.skip=0
        self.testInput = "%s/testInput.xml" % getTempDir()
        if os.path.exists(self.testInput):
            self.dom = xml.dom.minidom.parse(self.testInput)
            self.checkSkip()
        else:
            self.skip=1

        import libsmbios_c.memory as m
        import libsmbios_c.cmos as c
        import libsmbios_c.smbios as s

        # initialize global singletons
        m.MemoryAccess(m.MEMORY_GET_SINGLETON | m.MEMORY_UNIT_TEST_MODE, "%s/memdump.dat" % getTempDir())
        c.CmosAccess(c.CMOS_GET_SINGLETON | c.CMOS_UNIT_TEST_MODE, "%s/cmos.dat" % getTempDir())
        s.SmbiosTable(s.SMBIOS_GET_SINGLETON | s.SMBIOS_UNIT_TEST_MODE)

        # use private copies for testing
        self.memObj = m.MemoryAccess(m.MEMORY_GET_NEW | m.MEMORY_UNIT_TEST_MODE, "%s/memdump.dat" % getTempDir())
        self.cmosObj = c.CmosAccess(c.CMOS_GET_NEW | c.CMOS_UNIT_TEST_MODE, "%s/cmos.dat" % getTempDir())
        self.tableObj =  s.SmbiosTable(s.SMBIOS_GET_NEW | s.SMBIOS_UNIT_TEST_MODE)

    def tearDown(self):
        del(self.cmosObj)
        del(self.memObj)

    def testVariousAccessors(self):
        try:
            if self.skip: raise SkipTest()
            biosVendorStr = HelperXml.getNodeText( self.dom, "TESTINPUT", "smbios", "biosInformation", "vendor")
            versionStr = HelperXml.getNodeText( self.dom, "TESTINPUT", "smbios", "biosInformation", "version")
            releaseStr = HelperXml.getNodeText( self.dom, "TESTINPUT", "smbios", "biosInformation", "release")

            biosStruct = self.tableObj.getStructureByType(0) # BIOS Table type
            if len(biosVendorStr):
                self.assertEquals( biosVendorStr, biosStruct.getString(4) ) # BIOS VENDOR
            if len(versionStr):
                self.assertEquals( versionStr, biosStruct.getString(5) ) # BIOS VERSION
            if len(releaseStr):
                self.assertEquals( releaseStr, biosStruct.getString(8) ) # RELEASE DATE

            import libsmbios_c.system_info as si
            self.assertEquals( si.get_bios_version(), biosStruct.getString(5) )
        except SkipTest, e:
            print "skip ",

    def testIdByte(self):
        try:
            if self.skip: raise SkipTest()
            expected = int(HelperXml.getNodeText( self.dom, "TESTINPUT", "systemInfo", "idByte"), 0)

            import libsmbios_c.system_info as si
            self.assertEquals( si.get_dell_system_id(), expected )
        except SkipTest, e:
            print "skip ",

    def testServiceTag(self):
        try:
            if self.skip: raise SkipTest()
            expected = HelperXml.getNodeText( self.dom, "TESTINPUT", "systemInfo", "serviceTag")

            import libsmbios_c.system_info as si
            self.assertEquals( si.get_service_tag(), expected )
        except SkipTest, e:
            print "skip ",

if __name__ == "__main__":
    sys.exit(not TestLib.runTests( [TestCase] ))
