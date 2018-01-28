#!/usr/bin/python3
# vim:expandtab:autoindent:tabstop=4:shiftwidth=4:filetype=python:
"""
"""



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
        filename = "%s/memdump.dat" % getTempDir()
        filename = filename.encode('utf-8')
        m.MemoryAccess(m.MEMORY_GET_SINGLETON | m.MEMORY_UNIT_TEST_MODE, filename)
        filename = "%s/cmos.dat" % getTempDir()
        filename = filename.encode('utf-8')
        c.CmosAccess(c.CMOS_GET_SINGLETON | c.CMOS_UNIT_TEST_MODE, filename)
        if os.path.exists(os.path.join(getTempDir(), "DMI")):
            filename = "%s" % getTempDir()
            filename = filename.encode('utf-8')
            s.SmbiosTable(s.SMBIOS_GET_SINGLETON | s.SMBIOS_UNIT_TEST_MODE, filename)
        else:
            s.SmbiosTable(s.SMBIOS_GET_SINGLETON)

        # use private copies for testing
        filename = "%s/memdump.dat" % getTempDir()
        filename = filename.encode('utf-8')
        self.memObj = m.MemoryAccess(m.MEMORY_GET_NEW | m.MEMORY_UNIT_TEST_MODE, filename)
        filename = "%s/cmos.dat" % getTempDir()
        filename = filename.encode('utf-8')
        self.cmosObj = c.CmosAccess(c.CMOS_GET_NEW | c.CMOS_UNIT_TEST_MODE, filename)
        if os.path.exists(os.path.join(getTempDir(), "DMI")):
            filename = "%s" % getTempDir()
            filename = filename.encode('utf-8')
            self.tableObj = s.SmbiosTable(s.SMBIOS_GET_SINGLETON | s.SMBIOS_UNIT_TEST_MODE, filename)
        else:
            self.tableObj = s.SmbiosTable(s.SMBIOS_GET_SINGLETON)

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
                self.assertEqual( biosVendorStr, biosStruct.getString(4) ) # BIOS VENDOR
            if len(versionStr):
                self.assertEqual( versionStr, biosStruct.getString(5) ) # BIOS VERSION
            if len(releaseStr):
                self.assertEqual( releaseStr, biosStruct.getString(8) ) # RELEASE DATE

            import libsmbios_c.system_info as si
            self.assertEqual( si.get_bios_version(), biosStruct.getString(5) )
        except SkipTest as e:
            print("skip ", end=' ')

    def testIdByte(self):
        try:
            if self.skip: raise SkipTest()
            expected = int(HelperXml.getNodeText( self.dom, "TESTINPUT", "systemInfo", "idByte"), 0)

            import libsmbios_c.system_info as si
            self.assertEqual( si.get_dell_system_id(), expected )
        except SkipTest as e:
            print("skip ", end=' ')

    def testServiceTag(self):
        try:
            if self.skip: raise SkipTest()
            expected = HelperXml.getNodeText( self.dom, "TESTINPUT", "systemInfo", "serviceTag")

            import libsmbios_c.system_info as si
            self.assertEqual( si.get_service_tag(), expected )
        except SkipTest as e:
            print("skip ", end=' ')

if __name__ == "__main__":
    sys.exit(not TestLib.runTests( [TestCase] ))
