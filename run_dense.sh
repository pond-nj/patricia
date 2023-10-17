#!/bin/bash 

if g++-10 dense.cpp -o dense -std=c++20; then
    ./dense data/git_repo_name_clean.txt dense_keys dense_keys_int
fi