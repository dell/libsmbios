
# purpose of the following is so we can easily run from built source tree if
# topdir == builddir. (ie. doesnt support out of tree builds, set
# LD_LIBRARY_PATH manually for that)
import os
from _vars import *

# import the .so one time rather than in each submodule
import ctypes
try:
    # try explicit path first
    # this lets us run cleanly from build tree
    # or if user uses wierd --prefix for configure
    # doesnt work from build tree if top_builddir != top_srcdir
    libsmbios_c_DLL = ctypes.cdll.LoadLibrary(os.path.join(libdir, "libsmbios_c.so.2"))
except OSError, e:
    # then let it search
    libsmbios_c_DLL = ctypes.cdll.LoadLibrary("libsmbios_c.so.2")

# check ctypes bool support. If it doesnt exist, fake it.
if not getattr(ctypes, "c_bool", None):
    ctypes.c_bool = ctypes.c_uint8

# import all our packages.
import cmos
import memory
import smbios
import smi
import system_info
import token

# push gettext vars into namespaces
import gettext
t = gettext.translation(GETTEXT_PACKAGE, localedir, fallback=True)
for i in (cmos, memory, smbios, smi, system_info, token, _common):
    setattr(i, "_", t.ugettext)

__VERSION__ = RELEASE_VERSION

_all_ = [
    "cmos", "memory", "smbios", "smi", "system_info", "token",
    "GETTEXT_PACKAGE",
    "localedir", "pkgdatadir", "pythondir", "pkgconfdir"
    ]
