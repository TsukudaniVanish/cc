# cc
[![C/C++ CI](https://github.com/TsukudaniVanish/cc/actions/workflows/c.yml/badge.svg)](https://github.com/TsukudaniVanish/cc/actions/workflows/c.yml)
cc is a subset of c compiler.


## Files

### cc.h
header file which all used functions are declared in.
Also, briefs of those functions  are written here.

### Token.c  
new token functions which make  new tokens

### Node.c
new node functions which make new nodes for abstract syntax tree

### Vector.c
this file serves vector utilities.

### Map.c
hash map utilities

### String.c
string utilities such as standard string library

### error_point.c
all error point logics are controlled here. 

### test... , unittest.c
functions for unit test.

### tokenize.c
a tokenizer

### main.c 
expect functions and  a main function are written in this file.

### parse.c
a parsing functions

### codegenerator.c
a generating local variable function  
a generator function which convert an abstract syntax tree into  assembly codes 



## coding policy

#### memory management 
For simplicity, we will not manage memories in cc codes.
We allocate memories to pointers but will not free memories within the compiler life cycle.
Don't use "free" before changing this policy.


## Reference  
Rui Ueyama,低レイヤを知りたい人のためのCコンパイラ作成入門,2020-03-16,https://www.sigbus.info/compilerbook
 ISO/IEC 9899:2011 - Information technology - Programming languages - C  
 draft of standard above ,last edited 2012-Nov-11 20:09:34,https://port70.net/~nsz/c/c11/n1570.html
