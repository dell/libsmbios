#!/bin/sh

#
# the purpose of this program is to test that each header (include/*.h) will
# properly compile stand-alone.
#
# a .cpp file is created for each header, and include for that specific header
# is added to the .cpp file. The #include line is the only line in the header.
# This is then compiled. There should be no errors or warnings.
#

outDir=cctest-$$-$RANDOM/
mkdir $outDir
RETCODE=0

for i in include/smbios/*.h ; 
do 
    echo "Test standalone compilation of $i"
    echo "#include \"smbios/$(basename $i)\"" > $outDir/$(basename $i .h).cpp
    gcc -Wall -c -Iinclude -o $outDir/$(basename $i .h).o $outDir/$(basename $i .h).cpp
    if [ $? -ne 0 ]; then RETCODE=$?; fi
done

rm -rf $outDir

exit $RETCODE
