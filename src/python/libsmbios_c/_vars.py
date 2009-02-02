# the purpose of this file is so we can run from the build tree.
# for installation, this is replaced with a better version.
import os

__all__ = ["__VERSION__", "GETTEXT_PACKAGE",
    "localedir", "pkgdatadir", "pythondir", "pkgconfdir", "libdir",
    "LIBSMBIOS_C_SONAME"
    ]

LIBSMBIOS_C_SONAME =   "libsmbios_c.so.2"   # replaced at install time

import libsmbios_c
_modpath = os.path.realpath(libsmbios_c.__path__[0])

# the following vars are all substituted on install
__VERSION__="uninstalled-version"
GETTEXT_PACKAGE="libsmbios"
libdir    =os.path.join(_modpath, "..", "..", "..", "out", ".libs")
pythondir =os.path.join(_modpath, "..")
localedir =os.path.join(_modpath, "..", "..", "..", "po")
pkgdatadir=os.path.join(_modpath, "..", "..", "..", "doc")
pkgconfdir=os.path.join(_modpath, "..", "..", "..", "etc")

# end vars

