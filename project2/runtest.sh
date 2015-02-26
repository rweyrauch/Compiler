#!/bin/bash

clear

echo "Running IR tests."


pushd testdata
for input in *.dcf
do
    ../dcc --target parse > ../out/$input.out $input
done
popd

