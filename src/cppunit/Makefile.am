# vim:noexpandtab:autoindent:tabstop=8:shiftwidth=8:filetype=make:nocindent:tw=0:


CLEANFILES+= testResults.xml src/cppunit/.libs

EXTRA_DIST += \
 	src/cppunit/runtests.sh	\
	src/cppunit/platform

TESTS = src/cppunit/runtests.sh
check_PROGRAMS = out/testRbu out/testPlatform out/testStandalone out/testC_memory_cmos out/testC_smbios

out_testRbu_LDFLAGS = @LIBXML2_LIBS@ -lcppunit -ldl -Lout -lsmbios $(CPPUNIT_LIBS) $(AM_LDFLAGS)
out_testRbu_CXXFLAGS = @LIBXML2_CFLAGS@ $(CPPUNIT_CXXFLAGS) $(AM_CXXFLAGS) -I@top_srcdir@/src/libsmbios/common  -I@top_srcdir@/src/cppunit/
out_testRbu_SOURCES = \
    src/cppunit/outputctl.h	\
    src/cppunit/main.cpp  \
    src/cppunit/interface/XmlUtils.cpp	\
    src/cppunit/interface/XmlUtils.h	\
    src/cppunit/interface/testRbu.h	\
    src/cppunit/interface/testRbu.cpp

out_testPlatform_LDFLAGS = @LIBXML2_LIBS@ -lcppunit -ldl -Lout -lsmbios $(CPPUNIT_LIBS) $(AM_LDFLAGS)
out_testPlatform_CXXFLAGS = @LIBXML2_CFLAGS@ $(CPPUNIT_CXXFLAGS) $(AM_CXXFLAGS) -I@top_srcdir@/src/libsmbios/common  -I@top_srcdir@/src/cppunit/
out_testPlatform_SOURCES = \
    src/cppunit/outputctl.h	\
    src/cppunit/main.cpp  \
    src/cppunit/interface/XmlUtils.cpp	\
    src/cppunit/interface/XmlUtils.h	\
    src/cppunit/interface/testPlatform.h	\
    src/cppunit/interface/testPlatform.cpp

out_testStandalone_LDFLAGS = @LIBXML2_LIBS@ -lcppunit -ldl -Lout -lsmbios $(CPPUNIT_LIBS) $(AM_LDFLAGS)
out_testStandalone_CXXFLAGS = @LIBXML2_CFLAGS@ $(CPPUNIT_CXXFLAGS) $(AM_CXXFLAGS) -I@top_srcdir@/src/libsmbios/common  -I@top_srcdir@/src/cppunit/
out_testStandalone_SOURCES = \
    src/cppunit/outputctl.h	\
    src/cppunit/main.cpp  \
    src/cppunit/interface/XmlUtils.cpp	\
    src/cppunit/interface/XmlUtils.h	\
    src/cppunit/interface/testStandalone.h	\
    src/cppunit/interface/testStandalone.cpp

out_testC_memory_cmos_LDFLAGS = -lcppunit -ldl -Lout -lsmbios_c $(CPPUNIT_LIBS) $(AM_LDFLAGS)
out_testC_memory_cmos_CXXFLAGS = $(CPPUNIT_CXXFLAGS) $(AM_CXXFLAGS) -I@top_srcdir@/src/cppunit/
out_testC_memory_cmos_SOURCES = \
    src/cppunit/outputctl.h	\
    src/cppunit/main.cpp  \
    src/cppunit/interface/testC_memory_cmos.h	\
    src/cppunit/interface/testC_memory_cmos.cpp

out_testC_smbios_LDFLAGS = -lcppunit -ldl -Lout -lsmbios_c $(CPPUNIT_LIBS) $(AM_LDFLAGS)
out_testC_smbios_CXXFLAGS = $(CPPUNIT_CXXFLAGS) $(AM_CXXFLAGS) -I@top_srcdir@/src/cppunit/
out_testC_smbios_SOURCES = \
    src/cppunit/outputctl.h	\
    src/cppunit/main.cpp  \
    src/cppunit/interface/testC_smbios.h	\
    src/cppunit/interface/testC_smbios.cpp

