#!/bin/sh

#set -x
set -e

DIR=$(cd $(dirname $0); pwd)
TMPDIR=$(mktemp -d  /tmp/unittest-$$-$RANDOM-XXXXXX)
trap "rm -rf $TMPDIR" EXIT QUIT HUP TERM INT

TST=src

echo -e "\n\nRunning test for RBU"
$TST/testRbu        $DIR/cppunit $TMPDIR rbu        $DIR/platform/rbu

echo -e "\n\nRunning Standalone tests."
$TST/testStandalone $DIR/cppunit $TMPDIR standalone $DIR/platform/opti

for i in $DIR/platform/opti ${UNIT_TEST_DATA_DIR}/platform/*; do
    [ -e $i/autotest_flag ] || continue
    echo -e "\n\nRunning test for $i"
    $TST/testPlatform   $DIR/cppunit $TMPDIR $(basename $i) $i
    [ $? -eq 0 ] || exit 1
done

echo -e "\n\nRunning CInterface tests."
#$TST/testCInterface $DIR/cppunit $TMPDIR standalone $DIR/platform/opti
$TST/testCInterface $DIR/cppunit $TMPDIR cinterface $DIR/platform/opti

