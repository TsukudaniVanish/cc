#!bin/bash

make test 
if [ $? -eq 1 ]; then 
    exit 1
fi 

make etest 
if [ $? -eq 1 ]; then 
    exit 1
fi

make unit_test
if [ $? -eq 1 ]; then 
    exit 1
fi

sh ./test/test_link.sh 

for target in `find *.c`
do 
    echo $target
    ./cc -f $target
    if [ $? -eq 1 ]; then 
        echo "failed: $target"

        exit 1
    fi 
done