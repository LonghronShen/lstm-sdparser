#!/bin/bash

export WITH_CUDA=${1:-off}

mkdir -p build

cd build
cmake -DWITH_CUDA="${WITH_CUDA}" ..

cmake --build . -j$(nproc)