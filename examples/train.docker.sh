#!/bin/bash

docker run -ti --rm -v $(pwd):/app/examples -w /app/examples lstmsdparser:latest ./train.sh
