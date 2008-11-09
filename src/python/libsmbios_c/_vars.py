# the purpose of this file is so we can run from the build tree.
# for installation, this is replaced with a better version.
import os

__all__ = ["RELEASE_VERSION", "GETTEXT_PACKAGE", 
    "localedir", "pkgdatadir", "pythondir", "pkgconfdir", "libdir",
    ]

import libsmbios_c
_modpath = os.path.realpath(libsmbios_c.__path__[0])

# the following vars are all substituted on install
RELEASE_VERSION="uninstalled-version"
GETTEXT_PACKAGE="libsmbios"
libdir    =os.path.join(_modpath, "..", "..", "..", "out", ".libs")
pythondir =os.path.join(_modpath, "..")
localedir =os.path.join(_modpath, "..", "..", "..", "po")
pkgdatadir=os.path.join(_modpath, "..", "..", "..", "doc")
pkgconfdir=os.path.join(_modpath, "..", "..", "..", "etc")
# end vars

