#!/bin/bash

clear

echo "Running scanner tests."

pushd testdata/scanner
for input in *
do
	cat $input | ../../src/parser -s > ../../out/$input.out
done
popd

echo "Running parser tests."

pushd testdata/parser
for input in *
do
	cat $input | ../../src/parser > ../../out/$input.out
done
popd 
