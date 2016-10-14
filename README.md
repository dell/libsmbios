# libsmbios
libsmbios provides a library to interface with the SMBIOS tables.
It also provides extensions for proprietary methods of interfacing with Dell specific
SMBIOS tables.

Dependencies
--
To build a libsmbios tarball, you will need the following dependencies, in whichever -devel package convention in use by your distribution:
1. libxml

When building from a git-checkout, you will need the following additional dependencies
1. autoconf
2. automake
3. gettext
4. libtool

Building:
--
libmsbios is a standard autotools package. Build it like any autotools package:
```
$ ./configure
  $ make
```
To build from a git checkout:
```
  $ ./autogen.sh     # autogen.sh internally runs configure automatically
  $ make
```
*RECOMMENDED* Out of tree build. To build in a separate dir, leaving a clean
source tree:
```
  $ mkdir _build
  $ ../configure
  $ make
```

To Install:

Standard autotools package:
```  
$ make install
```

Documentation is in doxygen format. To view the docs, run "make doxygen", then
look in the out/libsmbios_c/html/index.html.  Docs are also built as part of
"make all".

License
--
This software is dual-licensed under GPL/OSL.

 * Copyright (C) 2005-2016 Dell Inc.
 *  by Michael Brown <Michael_E_Brown@dell.com>
 * Licensed under the Open Software License version 2.1
 *
 * Alternatively, you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.

 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
