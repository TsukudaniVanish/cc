#!/bin/bash
assert(){
	
    expected="$1"
    input="$2"

    ./cc "$input" > tmp.s
	gcc -o tmp tmp.s
	./tmp
	actual="$?"

	if [ "$actual" == "$expected" ]
then
	    echo ": $input =>$actual : looks ok. "
else
	    echo ": $input => $expected expected, but got $actual"
	    exit 1
	fi
}
#error assert
assert_e(){
	
	input="$1"
	echo -n "Input : "
	echo $input
	./cc "$input" 1>tmp.s 2>error.log
	cat error.log

}


echo -n "Disply integer"
assert 0 0
echo -n "Displya integer another one"
assert 42 42
echo -n "sum and sub test"
assert 21 "5+20-4"
echo -n "tokenize test"
assert 21 "5 + 20 -4"
echo -n "mul test"
assert 47 '5+6*7'
echo -n "div test"
assert 4 '20/5'
echo -n "braket test"
assert 15 '5*(9-6)'
ssert 4 '(3+5)/2'

echo  "Error test"
assert_e 20++3

