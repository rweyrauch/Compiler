#!/bin/bash

clear

echo "Running IR semantic tests."

pushd testdata/semantic
for input in *.dcf
do
    ../../dcc --target parse > ../../out/$input.out $input
done
popd

