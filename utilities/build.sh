#!/bin/bash

export DYNET_BLAS_BACKEND=${1:-eigen}

mkdir -p build

cd build
cmake -DDYNET_BLAS_BACKEND="${DYNET_BLAS_BACKEND}" ..

cmake --build . -j$(nproc)