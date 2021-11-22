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
	    echo -e ": $input =>$actual : \e[32mlooks ok.\e[m"
else
	    echo -e ": $input => $expected expected, but got \e[31m$actual\e[m"
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
echo -n "Disply integer another one"
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
assert 4 '(3+5)/2'
echo -n "unit '+' test"
assert 30 '+10+20'
echo -n "unit '-' test"
assert 10 '-10+20'
echo -n "equality test"
assert 0 '1==11'
assert 0 '1 != 1'
assert 1 '1==1'
assert 1 '1 !=11'
echo -n "inequality test"
assert 0 '11 < 2'
assert 0 '1 < 1'
assert 0 '1 > 2'
assert 0 '1 > 1'
assert 0 '11 <= 1'
assert 0 '1 >= 11'
assert 1 '2 < 11'
assert 1 '11 <= 11'
assert 1 '11 >= 2'

echo  "Error test"
assert_e 20+++3

