#!/bin/bash
echo -e "\e[31mtest start!\e[m"
./cc -f test.cmini > tmp.s
cc -g -o tmp tmp.s helper_functions.o
./tmp