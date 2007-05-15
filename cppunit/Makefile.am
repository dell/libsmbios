# vim:noexpandtab:autoindent:tabstop=8:shiftwidth=8:filetype=make:nocindent:

AM_LDFLAGS = -L@top_builddir@/libraries/ -lsmbios -lsmbiosxml
INCLUDES = -I@top_builddir@/include -I@top_builddir@/libraries/common -I@top_srcdir@/include -I@top_srcdir@/libraries/common -I@top_srcdir@/libraries/xml_libxml2 -I@top_srcdir@/cppunit/

CLEANFILES=testResults.xml

EXTRA_DIST = \
 	runtests.sh	\
	platform	\
	outputctl.h	\
	./interface/testPlatform.h	\
	./interface/testRbu.h	\
	./interface/testStandalone.h

TESTS = runtests.sh
check_PROGRAMS = testRbu testPlatform testStandalone

testRbu_LDFLAGS = $(shell xml2-config --libs) -lcppunit -ldl $(CPPUNIT_LIBS)
testRbu_CXXFLAGS = $(shell xml2-config --cflags) -DLIBXML2=1 -DLIBXERCES=2 -DXMLUTILS=1 $(CPPUNIT_CXXFLAGS)
testRbu_SOURCES = \
    ./main.cpp  \
    ./interface/testRbu.cpp

testPlatform_LDFLAGS = $(shell xml2-config --libs) -lcppunit -ldl $(CPPUNIT_LIBS)
testPlatform_CXXFLAGS = $(shell xml2-config --cflags) -DLIBXML2=1 -DLIBXERCES=2 -DXMLUTILS=1 $(CPPUNIT_CXXFLAGS)
testPlatform_SOURCES = \
    ./main.cpp  \
    ./interface/testPlatform.cpp 

testStandalone_LDFLAGS = $(shell xml2-config --libs) -lcppunit -ldl $(CPPUNIT_LIBS)
testStandalone_CXXFLAGS = $(shell xml2-config --cflags) -DLIBXML2=1 -DLIBXERCES=2 -DXMLUTILS=1 $(CPPUNIT_CXXFLAGS)
testStandalone_SOURCES = \
    ./main.cpp  \
    ./interface/testStandalone.cpp 

