#!/bin/bash

clear

DCC=`pwd`/src/build/dcc
echo $DCC

echo "Running dcc optimizations tests."

TESTFILES=testdata/optimizer/correctness/*.dcf

for input in ${TESTFILES}
do
    dcfinput=${input##*/}
    dcfinput=${dcfinput%%.*}
    
    rm -f out/$dcfinput
    rm -f out/$dcfinput.*
    
    echo "---------------------------"
    echo "Test: ${dcfinput}"
    
    ${DCC} -o out/$dcfinput.s $input
    if [ -e out/$dcfinput.s ]
    then
        gcc out/$dcfinput.s -o out/$dcfinput 2> out/$dcfinput.log
        if [ -e out/$dcfinput ]
        then
            out/$dcfinput > out/$dcfinput.output
            diff out/$dcfinput.output testdata/optimizer/correctness/output/$dcfinput.out > /dev/null
            if [ $? -eq "0" ]
            then
                echo "PASS: ${input}."
            else
                echo "FAIL: ${input} did not produce the expected output."                
            fi
        else
            echo "FAIL: Failed to link ${input}."
        fi
    else
        echo "FAIL: Failed to compile ${input}."
    fi
done


echo "Running dcc optimization performance tests."

cp testdata/optimizer/tests/saman.pgm out
cp testdata/optimizer/tests/saman_noise.pgm out
cp testdata/optimizer/tests/segovia.pgm out

TESTFILES=testdata/optimizer/tests/*.dcf
gcc -c testdata/optimizer/tests/lib/6035.c -o out/6035.o
for input in ${TESTFILES}
do
    dcfinput=${input##*/}
    dcfinput=${dcfinput%%.*}
    
    rm -f out/$dcfinput
    rm -f out/$dcfinput.*
    
    echo "---------------------------"
    echo "Test: ${dcfinput}"
    
    ${DCC} -o out/$dcfinput.s $input
    if [ -e out/$dcfinput.s ]
    then    
        gcc out/$dcfinput.s out/6035.o -o out/$dcfinput 2> out/$dcfinput.log
        if [ -e out/$dcfinput ]
        then
            cd out
            ./$dcfinput > $dcfinput.output
            cd ..
        else
            echo "FAIL: Failed to link ${input}."
        fi        
    else
        echo "FAIL: Failed to compile ${input}."
    fi        
done

