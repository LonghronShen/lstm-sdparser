#!/bin/bash

set -x

unameOut="$(uname -s)"
arch="x64"
case "${unameOut}" in
    Linux*)
        machine=linux
        apt update && apt install -y git build-essential libopenblas-dev python3-pip python3-all-dev libboost-all-dev libicu-dev
        hash cmake 2>/dev/null || { pip3 install -i https://mirrors.aliyun.com/pypi/simple cmake; }
        ;;
    Darwin*)
        machine=osx
        hash sudo 2>/dev/null || { echo >&2 "Plesse ensure you have executed 'xcode-select --install' to have build tools ready."; exit; }
        brew install boost@1.67 protobuf@3.6.0 icu4c
        ;;
    CYGWIN*)        machine=win;;
    MINGW32_NT)
        machine=win
        arch=x86
        ;;
    MINGW64_NT)
        machine=win
        ;;
    *)
        machine="UNKNOWN:${unameOut}"
        echo "Not supported platform: ${machine}"
        exit -1
esac

git submodule update --init --recursive