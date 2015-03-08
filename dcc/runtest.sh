#!/bin/bash

clear

echo "Running IR semantic tests."

DCC=`pwd`/src/build/dcc
echo $DCC

TESTFILES=testdata/semantic/*.dcf

for input in ${TESTFILES}
do
    dcfinput=${input##*/}
    ${DCC} -o out/$dcfinput.s $input 2> out/$dcfinput.log
done
popd

echo "Running dcc codegen tests."

TESTFILES=testdata/codegen/*.dcf
for input in ${TESTFILES}
do
    dcfinput=${input##*/}
    echo $dcfinput
    ${DCC} -o out/$dcfinput.s $input
    gcc out/$dcfinput.s -o out/$dcfinput.out 2> out/$dcfinput.log
done
