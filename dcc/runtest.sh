#!/bin/bash

clear

echo "Running IR semantic tests."

DCC=`pwd`/src/build/dcc
echo $DCC

TESTFILES=testdata/semantic/*.dcf
for input in ${TESTFILES}
do
    dcfinput=${input##*/}
    dcfinput=${dcfinput%%.*}
    
    rm -f out/$dcfinput
    rm -f out/$dcfinput.*
    
    echo "---------------------------"
    echo "Test: ${dcfinput}"
    
    ${DCC} -o out/$dcfinput.s $input 2> out/$dcfinput.output
    diff out/$dcfinput.output testdata/semantic/output/$dcfinput.out > /dev/null
    if [ $? -eq "0" ]
    then
        echo "PASS: ${input}."
    else
        echo "FAIL: ${input} did not produce the expected output."                
    fi
done

