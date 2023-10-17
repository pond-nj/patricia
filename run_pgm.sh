#!/bin/bash

if g++-10 hardness.cpp -o hardness -std=c++20; then
    ./hardness ./dense_keys/git_repo_name_clean.txt hardness_output/dense
fi