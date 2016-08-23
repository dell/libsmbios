# vim:noexpandtab:autoindent:tabstop=8:shiftwidth=8:filetype=make:nocindent:tw=0:

pkgpythondir=$(pythondir)/libsmbios_c

if HAVE_PYTHON
pkgpython_PYTHON = \
	src/python/libsmbios_c/cmos.py		\
	src/python/libsmbios_c/token.py		\
	src/python/libsmbios_c/_common.py		\
	src/python/libsmbios_c/trace_decorator.py		\
	src/python/libsmbios_c/__init__.py		\
	src/python/libsmbios_c/smbios.py		\
	src/python/libsmbios_c/memory.py		\
	src/python/libsmbios_c/smi.py		\
	src/python/libsmbios_c/_peak_util_decorators.py		\
	src/python/libsmbios_c/system_info.py

pkgpython_PYTHON += src/python/_vars.py
endif

# src/python/libsmbios_c/_vars.py is only for in-tree use
# it is used to generate src/python/_vars.py which is installed
EXTRA_DIST += src/python/libsmbios_c/_vars.py
DISTCLEANFILES +=   src/python/_vars.py
REPLACE_VARS += LIBSMBIOS_C_SONAME

all: src/python/_vars.py
src/python/_vars.py: src/python/libsmbios_c/_vars.py  configure Makefile config.status
	mkdir -p $$(dirname $@) ||:
	cp $< $@
	for var in $(REPLACE_VARS)	;\
	do	\
		perl -p -i -e "s|^$$var\s*=.*|$$var=\"$${!var}\"|"  $@;\
	done
