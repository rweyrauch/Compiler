#!/bin/bash

clear

echo "Running IR semantic tests."

DCC=`pwd`/src/build/dcc
echo $DCC

TESTFILES=testdata/semantic/*.dcf
for input in ${TESTFILES}
do
    dcfinput=${input##*/}
    echo $dcfinput
    ${DCC} -o out/$dcfinput.s $input 2> out/$dcfinput.log
done
popd
