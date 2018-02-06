# libsmbios
[![Build Status](https://travis-ci.org/dell/libsmbios.png)](https://travis-ci.org/dell/libsmbios)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/dell-libsmbios/badge.svg)](https://scan.coverity.com/projects/dell-libsmbios)
libsmbios provides a library to interface with the SMBIOS tables.
It also provides extensions for proprietary methods of interfacing with Dell specific
SMBIOS tables.

Dependencies
--
To build a libsmbios tarball, you will need the following dependencies, in whichever -devel package convention in use by your distribution:
1. libxml
1. autoconf
2. automake
3. gettext
4. libtool

Building
--
To build from a git checkout:
```
  $ ./autogen.sh     # autogen.sh internally runs configure automatically
  $ make
```

To Install

Standard autotools package:
```  
$ make install
```

Documentation is in doxygen format. To view the docs, run `make doxygen`, then
look in the `out/libsmbios_c/html/index.html`.  Docs are also built as part of
`make all`.

Distribution packages
--
Packages can be generated for RPM based distributions by using the helper script
```
# pkg/mk-rel-rpm.sh
```

Packages can be generated for DEB based distributions by using the helper script
```
# pkg/mk-rel-deb.sh
```

License
--
This software is dual-licensed under GPL/OSL.

See
[GPL License](https://github.com/dell/libsmbios/blob/master/COPYING-GPL)
and
[OSL License](https://github.com/dell/libsmbios/blob/master/COPYING-OSL)
for more details.
