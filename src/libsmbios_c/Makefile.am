# vim:noexpandtab:autoindent:tabstop=8:shiftwidth=8:filetype=make:nocindent:tw=0:

lib_LTLIBRARIES += out/libsmbios_c.la
out_libsmbios_c_la_LDFLAGS := -version-info $(LIBTOOL_CURRENT):$(LIBTOOL_REVISION):$(LIBTOOL_AGE)
out_libsmbios_c_la_CXXFLAGS := $(AM_CXXFLAGS)

EXTRA_DIST += src/include/smbios_c

out_libsmbios_c_la_SOURCES = \
    src/libsmbios_c/cmos/cmos_impl.h			\
    src/libsmbios_c/cmos/cmos.c				\
    src/libsmbios_c/memory/memory_impl.h		\
    src/libsmbios_c/memory/memory_linux.c		\
    src/libsmbios_c/memory/memory.c			\
    src/libsmbios_c/smbios/smbios_impl.h		\
    src/libsmbios_c/smbios/smbios_linux.c		\
    src/libsmbios_c/smbios/smbios.c			\
    src/libsmbios_c/system_info/id_byte.c		\
    src/libsmbios_c/system_info/asset_tag.c		\
    src/libsmbios_c/system_info/service_tag.c		\
    src/libsmbios_c/system_info/system_info.c
