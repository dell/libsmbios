
# purpose of the following is so we can easily run from built source tree if
# topdir == builddir. (ie. doesnt support out of tree builds, set
# LD_LIBRARY_PATH manually for that)
import os
from ._vars import *

# import the .so one time rather than in each submodule
import ctypes
try:
    # try explicit path first
    # this lets us run cleanly from build tree
    # or if user uses wierd --prefix for configure
    # doesnt work from build tree if top_builddir != top_srcdir
    libsmbios_c_DLL = ctypes.cdll.LoadLibrary(os.path.join(libdir, LIBSMBIOS_C_SONAME))
except OSError as e:
    # then let it search
    libsmbios_c_DLL = ctypes.cdll.LoadLibrary(LIBSMBIOS_C_SONAME)

# import all our packages.
from . import cmos
from . import memory
from . import smbios
from . import smi
from . import system_info
from . import smbios_token
from . import _common

import gettext
gettext.install(GETTEXT_PACKAGE, localedir)

__VERSION__ = "uninstalled-version"

_all_ = [
    "cmos", "memory", "smbios", "smi", "system_info", "smbios_token",
    "GETTEXT_PACKAGE",
    "localedir", "pkgdatadir", "pythondir", "pkgconfdir"
    ]
