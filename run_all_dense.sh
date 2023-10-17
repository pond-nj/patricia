#!/bin/bash
set -e

g++-10 dense.cpp -o dense -std=c++20 -O3
g++-10 hardness.cpp -o hardness -std=c++20 -O3

data_dir="data"
for path in "$data_dir"/*
do
    filename="$(basename $path)"
    echo "start $filename"

    # ./dense $data_dir/$filename dense_keys dense_keys_int
    # ./hardness dense_keys/$filename hardness_output/dense
    # python3 cdf.py dense_keys_int/$filename cdf/dense 
    python3 plot_hardness.py hardness_output/dense/$filename hardness_graph/dense

    echo "done $filename"
done