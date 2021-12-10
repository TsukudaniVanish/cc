#!/bin/bash
assert(){
	
    expected="$1"
    input="$2"

    ./cc "int main(){$input;}" > tmp.s
	cc -g -o tmp tmp.s
	./tmp
	actual="$?"
	if [ "$actual" == "$expected" ];then
	    echo -e ": int main(){$input;} =>$actual : \e[32mlooks ok.\e[m"
else
	    echo -e ": int main(){$input;} => $expected expected, but got \e[31m$actual\e[m"
	    exit 1
	fi
}
#error assert
assert_e(){
	
	input="$1"
	echo -n "Input : "
	echo $input
	echo $input >> test.log
	echo -e "    ^数ではありません" > test.log
	./cc "int main(){$input}" 1>tmp.s 2>error.log
	diff -q  error.log test.log	
	echo " =>"
	cat error.log
	if [ $? -eq 0 ];then
		
		echo -e "\e[32mlooks ok.\e[m"
	else
		
		echo  -e "\e[31minvaild error messages\e[m"
		exit 1
	fi
}

assert_type (){


	input="$1"
	echo -n "Input : "
	echo $input
	echo "int main(){$input}" > test.log
	echo -e "           ^型宣言がありません\n">> test.log
	./cc "int main(){$input}" 1>tmp.s 2>error.log
	diff -q error.log test.log
	result="$?"
	echo " =>"
	cat error.log
	if [ "$result" == 0 ];then
		
		echo -e "\e[32mlooks ok.\e["
	else
		echo -e "\e[31minvaild error messages\e[m"
		exit 1
	fi
}

assert_function (){

	expected="$1"
	input="$2"
	echo -n "$2 => "
	./cc "$2" > tmp.s
	cc -g -o tmp tmp.s
	./tmp
	actual=$?
	if [ "$actual" -eq "$expected" ];then

		echo -e " $actual : \e[32mlooks ok.\e[m"
	else
		echo -e " $expected ,\e[31m but got $actual\e[m"
		exit 1
	fi
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

echo -n "unit *,& test"
assert 3 "int x = 3;int *z = &x;return *z"

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

echo "local var test"
assert 14 'int a=3;int b =5*6- 8 ; a+b/2'
assert 14 'int foo =3; int baa = 5 * 6-8;foo+baa/2'

echo "return test"
assert 14 'int a = 1;return a+13'
assert 8 'return 8; return 15'

echo "if test"
assert 1 'if(1)return 1'
assert 10 'if(0)return 0; else return 10'

echo "while test"
assert 0 'int a=10;while(a) a=a-1; return a '

echo "For test"
assert 10 'for(int i = 0; i <10; i = i+1) 0; return i'

echo "block test"
assert 1 ' int a = 0; int i = 0; { i = 4 ; i = 3 ; i = 2; i=1;  } return i'
assert 1 'int a = 1; for(  int i=0 ; i < 10 ; i = i+1 ){ a = a +i ; a = a -i ; } return 1'

echo "function call test"

assert_function 16 'int gagaga(int a,int b,int c){ a = a + b*c;  return a; }int main(){return gagaga(10,2,3);}'

echo "pointer test"

assert 3 'int x;int *y;y = &x;*y = 3;return x'

echo "sizeof test"
assert 4 'int x; sizeof x'
assert 8 'int *y; sizeof(y)'

echo "array sizeof test"
assert 16 'int *a[2];sizeof a'

echo "array type chast test"

assert 2 'int a[2];*(a + 1) = 2;int *p;p = a;return *(p + 1)'

echo "array index acsess test"

assert 3 'int a[2];  a[1] = 3; return a[1]'

echo "gloval variable test "

assert_function 100 'int g = 100; int main(){return g;}'

assert_function 100 'int g; int main(){g = 100;return g;}'

assert_function 100 'int a[2]; int main(){a[1] = 100;return a[1];}'

echo "char test"

assert 0 'char a;int i = 0; return i'

assert 3 'char x[3];x[0] = -1;x[1] = 2;int y;y = 4;return x[0] + y'

assert 10 'char *b = "hi hello"; int a = 10; return a'

echo  "Error test"
assert_e '20+++3;'

assert_type 'a = 0;return a;'