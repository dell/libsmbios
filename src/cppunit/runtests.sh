#!/bin/sh

#set -x
set -e

DIR=$(cd $(dirname $0); pwd)
TMPDIR=$(mktemp -d  /tmp/unittest-$$-$RANDOM-XXXXXX)
trap "rm -rf $TMPDIR" EXIT QUIT HUP TERM INT

TST=out

echo -e "\n\nRunning CInterface tests."
$TST/testC_memory_cmos $TMPDIR 

echo -e "\n\nRunning test for RBU"
$TST/testRbu  $TMPDIR  $DIR/test_data/rbu

echo -e "\n\nRunning Standalone tests."
$TST/testStandalone $TMPDIR $DIR/test_data/opti

for i in $DIR/test_data/opti ${UNIT_TEST_DATA_DIR}/platform/*; do
    [ -e $i/autotest_flag ] || continue
    echo -e "\n\nRunning test for $i"
    $TST/testC_smbios $TMPDIR $(basename $i) $i
    [ $? -eq 0 ] || exit 1
done

for i in $DIR/test_data/opti ${UNIT_TEST_DATA_DIR}/platform/*; do
    [ -e $i/autotest_flag ] || continue
    echo -e "\n\nRunning test for $i"
    $TST/testPlatform $TMPDIR $(basename $i) $i
    [ $? -eq 0 ] || exit 1
done

