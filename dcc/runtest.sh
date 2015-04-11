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

echo "Running dcc dataflow tests."

TESTFILES=testdata/dataflow/*.dcf
for input in ${TESTFILES}
do
    dcfinput=${input##*/}
    echo $dcfinput
    ${DCC} -o out/$dcfinput.s $input
    gcc out/$dcfinput.s -o out/$dcfinput.out 2> out/$dcfinput.log
done

echo "Running dcc optimizations tests."

TESTFILES=testdata/optimizer/correctness/*.dcf
for input in ${TESTFILES}
do
    dcfinput=${input##*/}
    echo $dcfinput
    ${DCC} -o out/$dcfinput.s $input
    gcc out/$dcfinput.s -o out/$dcfinput.out 2> out/$dcfinput.log
done

echo "Running dcc optimization performance tests."

TESTFILES=testdata/optimizer/tests/*.dcf
gcc -c testdata/optimizer/tests/lib/6035.c -o out/6035.o
for input in ${TESTFILES}
do
    dcfinput=${input##*/}
    echo $dcfinput
    ${DCC} -o out/$dcfinput.s $input
    gcc out/$dcfinput.s out/6035.o -o out/$dcfinput.out 2> out/$dcfinput.log
done
cp testdata/optimizer/tests/saman.pgm out
cp testdata/optimizer/tests/saman_noise.pgm out
cp testdata/optimizer/tests/segovia.pgm out

