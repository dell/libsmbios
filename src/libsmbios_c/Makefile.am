# vim:noexpandtab:autoindent:tabstop=8:shiftwidth=8:filetype=make:nocindent:tw=0:

lib_LTLIBRARIES += out/libsmbios_c.la
out_libsmbios_c_la_LDFLAGS := $(AM_LDFLAGS) -version-info $(LIBSMBIOS_C_LIBTOOL_CURRENT):$(LIBSMBIOS_C_LIBTOOL_REVISION):$(LIBSMBIOS_C_LIBTOOL_AGE)
out_libsmbios_c_la_CPPFLAGS := $(AM_CPPFLAGS) -I$(top_srcdir)/src/libsmbios_c/common
out_libsmbios_c_la_CFLAGS := $(AM_CFLAGS) -fvisibility=hidden

EXTRA_DIST += src/include/smbios_c

# need to compile in memory_ut.c on windows (eventually)

out_libsmbios_c_la_SOURCES = \
    src/libsmbios_c/common/common.c			\
    src/libsmbios_c/common/common_internal.h		\
    src/libsmbios_c/common/strlcpy.c			\
    src/libsmbios_c/common/strlcat.c			\
    src/libsmbios_c/common/select_compiler_config.h	\
    src/libsmbios_c/common/libsmbios_c_intlize.h	\
    src/libsmbios_c/common/internal_strl.h		\
    src/libsmbios_c/common/platform/linux.h		\
    src/libsmbios_c/common/platform/win64.h		\
    src/libsmbios_c/common/platform/win32.h		\
    src/libsmbios_c/common/compiler/visualc.h		\
    src/libsmbios_c/common/compiler/gcc.h		\
    src/libsmbios_c/common/libsmbios_c_source.h		\
    src/libsmbios_c/common/select_platform_config.h	\
    src/libsmbios_c/cmos/cmos_impl.h			\
    src/libsmbios_c/cmos/cmos.c				\
    src/libsmbios_c/cmos/cmos_ut.c			\
    src/libsmbios_c/cmos/cmos_obj.c			\
    src/libsmbios_c/memory/memory_impl.h		\
    src/libsmbios_c/memory/memory_obj.c			\
    src/libsmbios_c/memory/memory.c			\
    src/libsmbios_c/smbios/smbios.c			\
    src/libsmbios_c/smbios/smbios_impl.h		\
    src/libsmbios_c/smbios/smbios_fixups.c		\
    src/libsmbios_c/smbios/smbios_obj.c			\
    src/libsmbios_c/smi/smi.c				\
    src/libsmbios_c/smi/smi_obj.c			\
    src/libsmbios_c/smi/smi_password.c			\
    src/libsmbios_c/smi/smi_impl.h			\
    src/libsmbios_c/system_info/id_byte.c		\
    src/libsmbios_c/system_info/asset_tag.c		\
    src/libsmbios_c/system_info/service_tag.c		\
    src/libsmbios_c/system_info/property_tag.c		\
    src/libsmbios_c/system_info/system_info.c		\
    src/libsmbios_c/system_info/state_byte.c		\
    src/libsmbios_c/system_info/up_flag.c		\
    src/libsmbios_c/system_info/dell_magic.h		\
    src/libsmbios_c/system_info/sysinfo_impl.h		\
    src/libsmbios_c/token/checksum.c			\
    src/libsmbios_c/token/token.c			\
    src/libsmbios_c/token/token_obj.c			\
    src/libsmbios_c/token/token_d4.c			\
    src/libsmbios_c/token/token_da.c			\
    src/libsmbios_c/token/token_impl.h

libsmbios_c_SOLARIS_SOURCES = \
    src/libsmbios_c/cmos/cmos_solaris.c			\
    src/libsmbios_c/memory/memory_solaris.c		\
    src/libsmbios_c/smbios/smbios_solaris.c		\
    src/libsmbios_c/smi/smi_solaris.c

libsmbios_c_LINUX_SOURCES = \
    src/libsmbios_c/cmos/cmos_linux.c			\
    src/libsmbios_c/memory/memory_linux.c		\
    src/libsmbios_c/smbios/smbios_linux.c		\
    src/libsmbios_c/smi/smi_linux.c

libsmbios_c_WINDOWS_SOURCES = 	\
    src/libsmbios_c/common/common_windows.c		\
    src/libsmbios_c/common/common_windows.h		\
    src/libsmbios_c/cmos/cmos_windows.c			\
    src/libsmbios_c/memory/memory_windows.c		\
    src/libsmbios_c/memory/memory_ut.c			\
    src/libsmbios_c/smbios/smbios_windows.c		\
    src/libsmbios_c/smi/smi_windows.c

if BUILD_WINDOWS
out_libsmbios_c_la_SOURCES += $(libsmbios_c_WINDOWS_SOURCES)
EXTRA_DIST += $(libsmbios_c_LINUX_SOURCES) $(libsmbios_c_SOLARIS_SOURCES)
endif

if BUILD_LINUX
out_libsmbios_c_la_SOURCES += $(libsmbios_c_LINUX_SOURCES)
EXTRA_DIST += $(libsmbios_c_WINDOWS_SOURCES) $(libsmbios_c_SOLARIS_SOURCES)
endif

if BUILD_SOLARIS
out_libsmbios_c_la_SOURCES += $(libsmbios_c_SOLARIS_SOURCES)
EXTRA_DIST += $(libsmbios_c_WINDOWS_SOURCES) $(libsmbios_c_LINUX_SOURCES)
endif

