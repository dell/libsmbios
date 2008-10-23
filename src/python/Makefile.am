# vim:noexpandtab:autoindent:tabstop=8:shiftwidth=8:filetype=make:nocindent:tw=0:

#TESTS = src/pyunit/testAll.py
#check_SCRIPTS = src/pyunit/testAll.py

pkgpythondir=$(pythondir)/libsmbios_c
pkgpython_PYTHON = \
	src/python/libsmbios_c/__init__.py	\
	src/python/libsmbios_c/cmos.py		\
	src/python/libsmbios_c/_common.py	\
	src/python/libsmbios_c/memory.py	\
	src/python/libsmbios_c/smbios.py	\
	src/python/libsmbios_c/smi.py		\
	src/python/libsmbios_c/system_info.py

REPLACE_VARS_ON_INSTALL=$(pkgpythondir)/__init__.py
