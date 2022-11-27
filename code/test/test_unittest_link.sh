#!bin/bash

if [ ! -x ./cc ]; then 
    make 
fi 

./cc -F unittest.c > tmp_ut.s
./cc -F main.c > tmp_main.s
as -o tmp_ut.o tmp_ut.s
as -o tmp_main.o tmp_main.s  
cc -no-pie -g -o tmp Vector.o tokenize.o String.o Map.o Node.o Token.o tmp_main.o parse.o tmp_ut.o preprocess.o codegenerator.o ./backend/file.o ./backend/error_point.o

./tmp -T

if [ $? -eq 0 ]; then 
    echo "OK!"
else 
    echo "Failed"
    $? = 1
fi 