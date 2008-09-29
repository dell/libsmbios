#!/bin/sh

#set -x
set -e

DIR=$(cd $(dirname $0); pwd)
TMPDIR=$(mktemp -d  /tmp/unittest-$$-$RANDOM-XXXXXX)
trap "rm -rf $TMPDIR" EXIT QUIT HUP TERM INT

TST=out

echo -e "\n\nRunning CInterface tests."
#$TST/testC_memory_cmos $DIR/cppunit $TMPDIR standalone $DIR/platform/opti
$TST/testC_memory_cmos $TMPDIR 

echo -e "\n\nRunning test for RBU"
$TST/testRbu  $TMPDIR  $DIR/platform/rbu

echo -e "\n\nRunning Standalone tests."
$TST/testStandalone $TMPDIR $DIR/platform/opti

echo -e "\n\nRunning Standalone tests."
$TST/testC_smbios $TMPDIR $DIR/platform/opti

for i in $DIR/platform/opti ${UNIT_TEST_DATA_DIR}/platform/*; do
    [ -e $i/autotest_flag ] || continue
    echo -e "\n\nRunning test for $i"
    $TST/testPlatform $TMPDIR $(basename $i) $i
    [ $? -eq 0 ] || exit 1
done

