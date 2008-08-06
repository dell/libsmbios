# vim:noexpandtab:autoindent:tabstop=8:shiftwidth=8:filetype=make:nocindent:tw=0:

lib_LTLIBRARIES += out/libsmbios_c.la
out_libsmbios_c_la_LDFLAGS := -version-info $(LIBTOOL_CURRENT):$(LIBTOOL_REVISION):$(LIBTOOL_AGE)
out_libsmbios_c_la_CXXFLAGS := $(AM_CXXFLAGS)

EXTRA_DIST += src/include/smbios_c

out_libsmbios_c_la_SOURCES = \
    src/libsmbios_c/memory/memory_impl.h		\
    src/libsmbios_c/memory/memory_linux.c		\
    src/libsmbios_c/memory/memory.c
