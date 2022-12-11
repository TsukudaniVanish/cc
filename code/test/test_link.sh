#!bin/bash

if [ ! -x ./cc ]; then 
    make 
fi 

./cc -F unittest.c > tmp_unittest.s
./cc -F main.c > tmp_main.s
./cc -F Map.c > tmp_Map.s
./cc -F parse.c > tmp_parse.s 
as -o tmp_unittest.o tmp_unittest.s
as -o tmp_main.o tmp_main.s  
as -o tmp_Map.o tmp_Map.s 
cc -no-pie -g -o tmp Vector.o tokenize.o String.o tmp_Map.s Node.o Token.o tmp_main.o tmp_parse.s tmp_unittest.s preprocess.o codegenerator.o ./backend/file.o ./backend/error_point.o

./tmp -T
if [ $? -eq 0 ]; then 
    echo "unit_test: OK!"
else 
    echo "Failed"
    $? = 1
    exit 1
fi 

./tmp -f ./test/test.c > tmp_tmp.s 
cc -g -o tmp_tmp tmp_tmp.s 
./tmp_tmp 
if [ $? -eq 0 ]; then 
    echo "OK!"
else 
    echo "Failed"
    $? = 1
    exit 1
fi 
