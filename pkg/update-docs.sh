#!/bin/sh

set -e

DIR=$(cd $(dirname $0); pwd)
cd $DIR/..

[ -e Makefile ] || ./configure
make doxygen || :
rsync -e ssh -vrltH --delete doc/full/html/. /var/ftp/pub/Applications/libsmbios/main/.

