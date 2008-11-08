
# purpose of the following is so we can easily run from built source tree if
# topdir == builddir. (ie. doesnt support out of tree builds, set
# LD_LIBRARY_PATH manually for that)
import os
from _vars import *
":".join(os.environ.get("LD_LIBRARY_PATH", "").split(":") + [libdir,])

import gettext
t = gettext.translation(GETTEXT_PACKAGE, localedir, fallback=True)
_ = t.ugettext

# import the .so one time rather than in each submodule
import ctypes
libsmbios_c_DLL = ctypes.cdll.LoadLibrary("libsmbios_c.so.2")

# check ctypes bool support. If it doesnt exist, fake it.
if not getattr(ctypes, "c_bool", None):
    ctypes.c_bool = ctypes.c_uint8

import _common

# import all our packages.
import cmos
import memory
import smbios
import smi
import system_info
import token

# push gettext vars into namespaces
for i in (cmos, memory, smbios, smi, system_info, token, _common):
    setattr(i, "_", _)

__VERSION__ = RELEASE_VERSION

_all_ = [
    "cmos", "memory", "smbios", "smi", "system_info", "token",
    "GETTEXT_PACKAGE",
    "localedir", "pkgdatadir", "pythondir", "pkgconfdir"
    ]
