# vim:noexpandtab:autoindent:tabstop=8:shiftwidth=8:filetype=make:nocindent:tw=0:


CLEANFILES+= testResults.xml src/cppunit/.libs

EXTRA_DIST += \
 	src/cppunit/runtests.sh	\
	src/cppunit/test_data

TESTS = src/cppunit/runtests.sh
check_PROGRAMS = \
	out/testRbu \
	out/testPlatform \
	out/testStandalone \
	out/testC_memory_cmos \
	out/testC_smbios \
	out/testC_token	\
	out/testC_smi

out_testRbu_LDFLAGS = @LIBXML2_LIBS@ -lcppunit -ldl -Lout -lsmbios $(CPPUNIT_LIBS) $(AM_LDFLAGS)
out_testRbu_CXXFLAGS = @LIBXML2_CFLAGS@ $(CPPUNIT_CXXFLAGS) $(AM_CXXFLAGS) -I@top_srcdir@/src/libsmbios/common  -I@top_srcdir@/src/cppunit/
out_testRbu_SOURCES = \
    src/cppunit/outputctl.h	\
    src/cppunit/main.cpp  \
    src/cppunit/main.h  \
    src/cppunit/XmlUtils.cpp	\
    src/cppunit/XmlUtils.h	\
    src/cppunit/testRbu.h	\
    src/cppunit/testRbu.cpp

out_testPlatform_LDFLAGS = @LIBXML2_LIBS@ -lcppunit -ldl -Lout -lsmbios $(CPPUNIT_LIBS) $(AM_LDFLAGS)
out_testPlatform_CXXFLAGS = @LIBXML2_CFLAGS@ $(CPPUNIT_CXXFLAGS) $(AM_CXXFLAGS) -I@top_srcdir@/src/libsmbios/common  -I@top_srcdir@/src/cppunit/
out_testPlatform_SOURCES = \
    src/cppunit/outputctl.h	\
    src/cppunit/main.cpp  \
    src/cppunit/main.h  \
    src/cppunit/XmlUtils.cpp	\
    src/cppunit/XmlUtils.h	\
    src/cppunit/testPlatform.h	\
    src/cppunit/testPlatform.cpp

out_testStandalone_LDFLAGS = @LIBXML2_LIBS@ -lcppunit -ldl -Lout -lsmbios $(CPPUNIT_LIBS) $(AM_LDFLAGS)
out_testStandalone_CXXFLAGS = @LIBXML2_CFLAGS@ $(CPPUNIT_CXXFLAGS) $(AM_CXXFLAGS) -I@top_srcdir@/src/libsmbios/common  -I@top_srcdir@/src/cppunit/
out_testStandalone_SOURCES = \
    src/cppunit/outputctl.h	\
    src/cppunit/main.cpp  \
    src/cppunit/main.h  \
    src/cppunit/XmlUtils.cpp	\
    src/cppunit/XmlUtils.h	\
    src/cppunit/testStandalone.h	\
    src/cppunit/testStandalone.cpp

out_testC_memory_cmos_LDFLAGS = -lcppunit -ldl -Lout -lsmbios_c $(CPPUNIT_LIBS) $(AM_LDFLAGS)
out_testC_memory_cmos_CXXFLAGS = $(CPPUNIT_CXXFLAGS) $(AM_CXXFLAGS) -I@top_srcdir@/src/cppunit/
out_testC_memory_cmos_SOURCES = \
    src/cppunit/outputctl.h	\
    src/cppunit/main.cpp  \
    src/cppunit/main.h  \
    src/cppunit/testC_memory_cmos.h	\
    src/cppunit/testC_memory_cmos.cpp

out_testC_smbios_LDFLAGS = @LIBXML2_LIBS@ -lcppunit -ldl -Lout -lsmbios_c $(CPPUNIT_LIBS) $(AM_LDFLAGS)
out_testC_smbios_CXXFLAGS = @LIBXML2_CFLAGS@ $(CPPUNIT_CXXFLAGS) $(AM_CXXFLAGS) -I@top_srcdir@/src/cppunit/ -I@top_srcdir@/src/libsmbios/common
out_testC_smbios_SOURCES = \
    src/cppunit/outputctl.h	\
    src/cppunit/main.cpp  \
    src/cppunit/main.h  \
    src/cppunit/XmlUtils.cpp	\
    src/cppunit/XmlUtils.h	\
    src/cppunit/testC_smbios.h	\
    src/cppunit/testC_smbios.cpp

out_testC_token_LDFLAGS = @LIBXML2_LIBS@ -lcppunit -ldl -Lout -lsmbios_c $(CPPUNIT_LIBS) $(AM_LDFLAGS)
out_testC_token_CXXFLAGS = @LIBXML2_CFLAGS@ $(CPPUNIT_CXXFLAGS) $(AM_CXXFLAGS) -I@top_srcdir@/src/cppunit/ -I@top_srcdir@/src/libsmbios/common
out_testC_token_SOURCES = \
    src/cppunit/outputctl.h	\
    src/cppunit/main.cpp  \
    src/cppunit/main.h  \
    src/cppunit/XmlUtils.cpp	\
    src/cppunit/XmlUtils.h	\
    src/cppunit/testC_token.h	\
    src/cppunit/testC_token.cpp

out_testC_smi_LDFLAGS = -lcppunit -ldl -Lout -lsmbios_c $(CPPUNIT_LIBS) $(AM_LDFLAGS)
out_testC_smi_CXXFLAGS = $(CPPUNIT_CXXFLAGS) $(AM_CXXFLAGS) -I@top_srcdir@/src/cppunit/
out_testC_smi_SOURCES = \
    src/cppunit/outputctl.h	\
    src/cppunit/main.cpp  \
    src/cppunit/main.h  \
    src/cppunit/testC_smi.h	\
    src/cppunit/testC_smi.cpp

