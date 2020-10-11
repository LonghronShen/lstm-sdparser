#!/bin/bash

set -x

export USE_GPU=${1-0}

if [ "$USE_GPU" -ne "0" ]
then
    docker run -ti --rm --gpus all -v $(pwd):/app/examples -w /app/examples lstmsdparser:latest ./train.sh
else
    docker run -ti --rm -v $(pwd):/app/examples -w /app/examples lstmsdparser:latest ./train.sh
fi
