# vim:noexpandtab:autoindent:tabstop=8:shiftwidth=8:filetype=make:nocindent:tw=0:
CLEANFILES+= testResults.xml src/cppunit/.libs

EXTRA_DIST += \
 	src/cppunit/runtests.sh	\
	src/cppunit/test_data	\
	src/cppunit/system_dumps

if BUILD_LIBSMBIOS_CXX
TESTS += src/cppunit/runtests.sh

UT_CFLAGS=
if BUILD_LINUX
UT_CFLAGS += -DBUILD_LINUX
endif
if BUILD_WINDOWS
UT_CFLAGS += -DBUILD_WINDOWS
endif

TEST_CPPFLAGS = $(AM_CPPFLAGS) $(CPPUNIT_CFLAGS) $(UT_CFLAGS) $(LIBXML2_CFLAGS) -I$(top_srcdir)/src/libsmbios_c++/common

# convenience library for test stuff
check_LTLIBRARIES = out/libtestmain.la
out_libtestmain_la_SOURCES = src/cppunit/outputctl.h src/cppunit/main.cpp src/cppunit/main.h
out_libtestmain_la_LIBADD =  $(CPPUNIT_LIBS) out/libsmbios_c.la out/libsmbios.la
out_libtestmain_la_LDFLAGS = $(AM_LDFLAGS) -static

# convenience library for XML stuff
check_LTLIBRARIES += out/libxmlutil.la
out_libxmlutil_la_SOURCES = src/cppunit/XmlUtils.cpp src/cppunit/XmlUtils.h
out_libxmlutil_la_LIBADD =  $(LIBXML2_LIBS)
out_libxmlutil_la_CPPFLAGS = $(AM_CPPFLAGS) $(LIBXML2_CFLAGS) -I$(top_srcdir)/src/libsmbios_c++/common
out_libxmlutil_la_LDFLAGS = $(AM_LDFLAGS) -static

check_PROGRAMS += out/testPlatform
out_testPlatform_SOURCES = src/cppunit/testPlatform.h src/cppunit/testPlatform.cpp
out_testPlatform_LDADD = out/libxmlutil.la out/libtestmain.la $(AM_LDADD)
out_testPlatform_LDFLAGS = $(AM_LDFLAGS) -static
out_testPlatform_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += out/testStandalone
out_testStandalone_SOURCES = src/cppunit/testStandalone.h src/cppunit/testStandalone.cpp
out_testStandalone_LDADD = out/libxmlutil.la out/libtestmain.la $(AM_LDADD)
out_testStandalone_LDFLAGS = $(AM_LDFLAGS) -static
out_testStandalone_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += out/testC_smbios
out_testC_smbios_SOURCES = src/cppunit/testC_smbios.h src/cppunit/testC_smbios.cpp
out_testC_smbios_LDADD = out/libxmlutil.la out/libtestmain.la $(AM_LDADD)
out_testC_smbios_LDFLAGS = $(AM_LDFLAGS) -static
out_testC_smbios_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += out/testC_token
out_testC_token_SOURCES = src/cppunit/testC_token.h src/cppunit/testC_token.cpp
out_testC_token_LDADD = out/libxmlutil.la out/libtestmain.la $(AM_LDADD)
out_testC_token_LDFLAGS = $(AM_LDFLAGS) -static
out_testC_token_CPPFLAGS = $(TEST_CPPFLAGS)

check_PROGRAMS += out/testC_smi
out_testC_smi_SOURCES = src/cppunit/testC_smi.h src/cppunit/testC_smi.cpp
out_testC_smi_LDADD = out/libtestmain.la $(AM_LDADD)
out_testC_smi_LDFLAGS = $(AM_LDFLAGS) -static
out_testC_smi_CPPFLAGS = $(CPPUNIT_CFLAGS) $(AM_CPPFLAGS) $(UT_CFLAGS)

check_PROGRAMS += out/testC_memory_cmos
out_testC_memory_cmos_SOURCES = src/cppunit/testC_memory_cmos.h src/cppunit/testC_memory_cmos.cpp
out_testC_memory_cmos_LDADD = out/libtestmain.la $(AM_LDADD)
out_testC_memory_cmos_LDFLAGS = $(AM_LDFLAGS) -static
out_testC_memory_cmos_CPPFLAGS = $(CPPUNIT_CFLAGS) $(AM_CPPFLAGS) $(UT_CFLAGS)
endif
