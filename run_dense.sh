#!/bin/bash 

if g++-10 dense.cpp -o dense -std=c++20 -O3; then
    # ./dense data/emails-validated-random-only-30-characters_clean.txt dense_keys dense_keys_int
    ./dense data/git_repo_name_clean.txt dense_keys dense_keys_int
    # ./dense data/url_data_set_clean.txt dense_keys dense_keys_int
fi