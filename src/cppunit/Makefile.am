# vim:noexpandtab:autoindent:tabstop=8:shiftwidth=8:filetype=make:nocindent:tw=0:


CLEANFILES+= testResults.xml src/cppunit/.libs

EXTRA_DIST += \
 	src/cppunit/runtests.sh	\
	src/cppunit/platform

TESTS = src/cppunit/runtests.sh
check_PROGRAMS = src/testRbu src/testPlatform src/testStandalone src/testCInterface

src_testRbu_LDFLAGS = @LIBXML2_LIBS@ -lcppunit -ldl -Lsrc -lsmbios $(CPPUNIT_LIBS)
src_testRbu_CXXFLAGS = @LIBXML2_CFLAGS@ $(CPPUNIT_CXXFLAGS) -I@top_builddir@/include -I@top_builddir@/src/libsmbios/common -I@top_srcdir@/include -I@top_srcdir@/src/libsmbios/common -I@top_srcdir@/src/cppunit/
src_testRbu_SOURCES = \
    src/cppunit/outputctl.h	\
    src/cppunit/main.cpp  \
    src/cppunit/interface/XmlUtils.cpp	\
    src/cppunit/interface/XmlUtils.h	\
    src/cppunit/interface/testRbu.h	\
    src/cppunit/interface/testRbu.cpp

src_testPlatform_LDFLAGS = @LIBXML2_LIBS@ -lcppunit -ldl -Lsrc -lsmbios $(CPPUNIT_LIBS)
src_testPlatform_CXXFLAGS = @LIBXML2_CFLAGS@ $(CPPUNIT_CXXFLAGS) -I@top_builddir@/include -I@top_builddir@/src/libsmbios/common -I@top_srcdir@/include -I@top_srcdir@/src/libsmbios/common -I@top_srcdir@/src/cppunit/
src_testPlatform_SOURCES = \
    src/cppunit/outputctl.h	\
    src/cppunit/main.cpp  \
    src/cppunit/interface/XmlUtils.cpp	\
    src/cppunit/interface/XmlUtils.h	\
    src/cppunit/interface/testPlatform.h	\
    src/cppunit/interface/testPlatform.cpp

src_testStandalone_LDFLAGS = @LIBXML2_LIBS@ -lcppunit -ldl -Lsrc -lsmbios $(CPPUNIT_LIBS)
src_testStandalone_CXXFLAGS = @LIBXML2_CFLAGS@ $(CPPUNIT_CXXFLAGS) -I@top_builddir@/include -I@top_builddir@/src/libsmbios/common -I@top_srcdir@/include -I@top_srcdir@/src/libsmbios/common -I@top_srcdir@/src/cppunit/
src_testStandalone_SOURCES = \
    src/cppunit/outputctl.h	\
    src/cppunit/main.cpp  \
    src/cppunit/interface/XmlUtils.cpp	\
    src/cppunit/interface/XmlUtils.h	\
    src/cppunit/interface/testStandalone.h	\
    src/cppunit/interface/testStandalone.cpp

src_testCInterface_LDFLAGS = -lcppunit -ldl -Lsrc/.libs -lsmbios_c $(CPPUNIT_LIBS)
src_testCInterface_CXXFLAGS = $(CPPUNIT_CXXFLAGS) -I@top_builddir@/include -I@top_srcdir@/include -I@top_srcdir@/src/cppunit/
src_testCInterface_SOURCES = \
    src/cppunit/outputctl.h	\
    src/cppunit/main.cpp  \
    src/cppunit/interface/testCInterface.h	\
    src/cppunit/interface/testCInterface.cpp

