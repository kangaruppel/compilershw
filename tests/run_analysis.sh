#! /bin/bash

echo "run clang"
clang -O -emit-llvm -c $1.c -o $1.bc

echo "run pass"
opt -load ../FunctionInfo/FunctionInfo.so -function-info $1.bc -o $.out

