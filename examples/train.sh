#!/bin/bash

set -x

export PATH=$PATH:$(pwd)/../build/bin/

lstmsdparser -T data/trail.train.conll -d data/trail.dev.conll -w data/trail.emb -s list-graph --data_type trail --pretrained_dim 100 --hidden_dim 200 --bilstm_hidden_dim 100 --lstm_input_dim 200 --input_dim 100 --action_dim 50 --pos_dim 50 --rel_dim 50 --dynet_mem 2000 --max_itr 5000 -c sem16 -P -B -R -t --model_dir models/
