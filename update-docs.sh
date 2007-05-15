#!/bin/sh

set -e

[ -e Makefile ] || ./configure
make doxygen || :
rsync -e ssh -vrltH --delete doc/full/html/. hb:/var/ftp/pub/Applications/libsmbios/main/.

