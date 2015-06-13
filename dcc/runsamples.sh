#!/bin/bash

clear

echo "Running Stanford samples tests."

DCC=`pwd`/src/build/dcc
echo $DCC

TESTFILES=testdata/samples/*.dcf
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
            diff out/$dcfinput.output testdata/samples/output/$dcfinput.out > /dev/null
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

TESTFILES=testdata/samples-checkpoint/*.decaf
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
            diff out/$dcfinput.output testdata/samples-checkpoint/output/$dcfinput.out > /dev/null
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

