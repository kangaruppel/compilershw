#! /bin/bash

echo "run clang"
clang -Xclang -disable-O0-optnone -O0 -emit-llvm -c $1.c -o $1.bc

echo "run mem2reg"
opt -mem2reg $1.bc -o $1-m2r.bc

echo "run pass"
opt -load ./../LocalOpts/LocalOpts.so -local-opts $1-m2r.bc -o $1.out

