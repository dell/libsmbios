# vim:noexpandtab:autoindent:tabstop=8:shiftwidth=8:filetype=make:nocindent:tw=0:


CLEANFILES+= testResults.xml cppunit/.libs

EXTRA_DIST += \
 	cppunit/runtests.sh	\
	cppunit/platform

TESTS = cppunit/runtests.sh
check_PROGRAMS = cppunit/testRbu cppunit/testPlatform cppunit/testStandalone cppunit/testCInterface

cppunit_testRbu_LDFLAGS = @LIBXML2_LIBS@ -lcppunit -ldl -lsmbios -Llibraries $(CPPUNIT_LIBS)
cppunit_testRbu_CXXFLAGS = @LIBXML2_CFLAGS@ $(CPPUNIT_CXXFLAGS) -I@top_builddir@/include -I@top_builddir@/libraries/common -I@top_srcdir@/include -I@top_srcdir@/libraries/common -I@top_srcdir@/cppunit/
cppunit_testRbu_SOURCES = \
    cppunit/outputctl.h	\
    cppunit/main.cpp  \
    cppunit/interface/XmlUtils.cpp	\
    cppunit/interface/XmlUtils.h	\
    cppunit/interface/testRbu.h	\
    cppunit/interface/testRbu.cpp

cppunit_testPlatform_LDFLAGS = @LIBXML2_LIBS@ -lcppunit -ldl -lsmbios -Llibraries $(CPPUNIT_LIBS)
cppunit_testPlatform_CXXFLAGS = @LIBXML2_CFLAGS@ $(CPPUNIT_CXXFLAGS) -I@top_builddir@/include -I@top_builddir@/libraries/common -I@top_srcdir@/include -I@top_srcdir@/libraries/common -I@top_srcdir@/cppunit/
cppunit_testPlatform_SOURCES = \
    cppunit/outputctl.h	\
    cppunit/main.cpp  \
    cppunit/interface/XmlUtils.cpp	\
    cppunit/interface/XmlUtils.h	\
    cppunit/interface/testPlatform.h	\
    cppunit/interface/testPlatform.cpp

cppunit_testStandalone_LDFLAGS = @LIBXML2_LIBS@ -lcppunit -ldl -lsmbios -Llibraries $(CPPUNIT_LIBS)
cppunit_testStandalone_CXXFLAGS = @LIBXML2_CFLAGS@ $(CPPUNIT_CXXFLAGS) -I@top_builddir@/include -I@top_builddir@/libraries/common -I@top_srcdir@/include -I@top_srcdir@/libraries/common -I@top_srcdir@/cppunit/
cppunit_testStandalone_SOURCES = \
    cppunit/outputctl.h	\
    cppunit/main.cpp  \
    cppunit/interface/XmlUtils.cpp	\
    cppunit/interface/XmlUtils.h	\
    cppunit/interface/testStandalone.h	\
    cppunit/interface/testStandalone.cpp

cppunit_testCInterface_LDFLAGS = -lcppunit -ldl -lsmbios_c -Llibraries $(CPPUNIT_LIBS)
cppunit_testCInterface_CXXFLAGS = $(CPPUNIT_CXXFLAGS) -I@top_builddir@/include -I@top_srcdir@/include -I@top_srcdir@/cppunit/
cppunit_testCInterface_SOURCES = \
    cppunit/outputctl.h	\
    cppunit/main.cpp  \
    cppunit/interface/testCInterface.h	\
    cppunit/interface/testCInterface.cpp

