# the purpose of this file is so we can run from the build tree.
# for installation, this is replaced with a better version.

import os
import sys
import gettext

__all__ = ["RELEASE_VERSION", "localedir", "GETTEXT_PACKAGE", "pkgdatadir", "pythondir", "_"]

# the following vars are all substituted on install
RELEASE_VERSION="uninstalled-version"
GETTEXT_PACKAGE="libsmbios"
pkgdatadir=os.path.join(os.path.dirname(os.path.realpath(sys.argv[0])), "..", "..", "doc")
pythondir=os.path.join(os.path.dirname(os.path.realpath(sys.argv[0])), "..", "python")
localedir=os.path.join(os.path.dirname(os.path.realpath(sys.argv[0])), "..", "..", "po")
# end vars

t = gettext.translation(GETTEXT_PACKAGE, localedir, fallback=True)
_ = t.ugettext

