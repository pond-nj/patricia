#!/bin/bash
set -e

g++-10 sparse.cpp -o sparse -std=c++20
g++-10 hardness.cpp -o hardness -std=c++20

data_dir="data"
for path in "$data_dir"/*
do
    filename="$(basename $path)"
    echo "start $filename"

    # ./sparse $data_dir/$filename sparse_keys sparse_keys_int
    # ./hardness sparse_keys/$filename hardness_output/sparse
    python3 cdf.py sparse_keys_int/$filename cdf/sparse 
    python3 plot_hardness.py hardness_output/sparse/$filename hardness_graph/sparse

    echo "done $filename"
done