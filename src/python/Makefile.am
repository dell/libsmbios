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
	src/python/libsmbios_c/system_info.py	\
	src/python/libsmbios_c/token.py	\
	src/python/_vars.py

src/python/_vars.py: src/python/libsmbios_c/_vars.py  configure Makefile config.status
	mkdir -p $$(dirname $@) ||:
	cp $< $@
	for var in RELEASE_VERSION PACKAGE localedir libdir libexecdir datadir sysconfdir pythondir pkgpythondir pkgdatadir pkgconfdir pkggladedir pkglibexecdir;\
	do	\
		echo "replace $$var=\"$${!var}\""		;\
		perl -p -i -e "s|^$$var\s*=.*|$$var=\"$${!var}\"|"  $@	;\
	done

# this is only for in-tree use.
EXTRA_DIST += src/python/libsmbios_c/_vars.py
