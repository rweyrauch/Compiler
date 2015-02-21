#!/bin/bash

clear

echo "Running IR tests."

pushd testdata
for input in *.dcf
do
    ../src/parser --target parse > ../out/$input.out $input
done
popd 
