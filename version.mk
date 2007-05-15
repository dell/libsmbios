# This file is dual bash/make variable format so I have one place to bump the version.
# Problem: need to also bump version in configure.ac. No way to automatically do this right now.

RELEASE_NAME=libsmbios
RELEASE_MAJOR=0
RELEASE_MINOR=13
RELEASE_SUBLEVEL=7
RELEASE_EXTRALEVEL=

# the variables below are now set in the configure script.
#
#LIBSMBIOS_LIBTOOL_CURRENT=$(( $RELEASE_MAJOR + 1 ))
#LIBSMBIOS_LIBTOOL_REVISION=$RELEASE_MINOR
#LIBSMBIOS_LIBTOOL_AGE=0

# Update the version information only immediately before a public release of your software. More frequent updates are unnecessary, and only guarantee that the current interface number gets larger faster.
# If the library source code has changed at all since the last update, then increment revision (c:r:a becomes c:r+1:a).
# If any interfaces have been added, removed, or changed since the last update, increment current, and set revision to 0.
# If any interfaces have been added since the last public release, then increment age.
# If any interfaces have been removed since the last public release, then set age to 0. 
