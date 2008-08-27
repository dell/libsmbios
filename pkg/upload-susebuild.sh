#!/bin/sh

set -e
set -x

PROJECT=$1
PACKAGE=$2
if [ -z "$PROJECT" -o -z "$PACKAGE" ]; then
    echo "Must specify project and package"
    exit 1
fi

osc co $PROJECT $PACKAGE
rm -f $PROJECT/$PACKAGE/*.tar.gz
rm -f $PROJECT/$PACKAGE/*.spec
cp ${PACKAGE}*.tar.gz $PROJECT/$PACKAGE
cp */${PACKAGE}.spec $PROJECT/$PACKAGE
pushd $PROJECT/$PACKAGE
osc addremove
yes | osc updatepacmetafromspec
osc ci -m "scripted source update"
popd
