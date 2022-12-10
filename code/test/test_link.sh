#!bin/bash

if [ ! -x ./cc ]; then 
    make 
fi 

./cc -F unittest.c > tmp_ut.s
./cc -F main.c > tmp_main.s
./cc -F Map.c > tmp_Map.s
as -o tmp_ut.o tmp_ut.s
as -o tmp_main.o tmp_main.s  
as -o tmp_Map.o tmp_Map.s 
cc -no-pie -g -o tmp Vector.o tokenize.o String.o tmp_Map.s Node.o Token.o tmp_main.s parse.o tmp_ut.s preprocess.o codegenerator.o ./backend/file.o ./backend/error_point.o

./tmp -T
if [ $? -eq 0 ]; then 
    echo "unit_test: OK!"
else 
    echo "Failed"
    $? = 1
    exit 1
fi 

# ./tmp -f ./test/test.c > tmp_tmp.s 
# cc -g -o tmp_tmp tmp_tmp.s 
# ./tmp_tmp 
# if [ $? -eq 0 ]; then 
#     echo "OK!"
# else 
#     echo "Failed"
#     $? = 1
#     exit 1
# fi 
