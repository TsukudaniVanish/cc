# cc
[![C/C++ CI](https://github.com/TsukudaniVanish/cc/actions/workflows/c.yml/badge.svg)](https://github.com/TsukudaniVanish/cc/actions/workflows/c.yml)  
cc is a subset of c compiler. It will cover the c89 syntax in th future. 
This product is developed for my study of computer science. Pull requests which are related to language implementation will be declined. Thank you for your understanding!

## covered language functionalities 
### type 
- void 
- char 
- int
- unsigned
- struct 
- union 
- enum 
### flow operation, statement  
- if 
- else 
- { ... }
- switch 
- while 
- for 
- continue
- break
- return 
### preprocessor 
- #define
- #if ... #endif

### other keyword  
- typedef
- sizeof 

## code/

### cc.h
header file which all used functions are declared in.
Also, briefs of those functions  are written here.

### Token.c  
new token functions which make  new tokens and token utilities

### Node.c
new node functions which make new nodes for abstract syntax tree

### Vector.c
this file serves vector utilities.

### Map.c
hash map utilities

### String.c
string utilities such as strlen, strncmp, strncpy in string.h 

### error_point.c
all error point logics are controlled here. 

### file.c 
file reader.

### unittest.c
functions for unit test.

### test.c 
test file for application. this file is not used to build cc.

### tokenize.c
a tokenizer

### main.c 
expect functions and  a main function are written in this file.

### parse.c
a parsing functions

### codegenerator.c
a generating local variable function  
a generator function which convert an abstract syntax tree into  assembly codes 

## todo
- [ ] refactoring/ remove dependency on standard library
- [ ] add long and long int
- [x] add ? : 
- [x]  add typedef 
- [x] sizeof can accept type-keyword, type alias
- [x] add do{...}while(...) 
- [x] refactoring preprocessor (especially macro_expansion)
- [x] add storage class specifiers (static, extern)
- [x] add #include to preprocessor 

## coding policy

#### memory management 
For simplicity, we will not manage memories in cc codes.
We allocate memories to pointers but will not free memories within the compiler life cycle.
Don't use "free" before changing this policy.


## Reference  
Rui Ueyama,低レイヤを知りたい人のためのCコンパイラ作成入門,2020-03-16,https://www.sigbus.info/compilerbook
 ISO/IEC 9899:2011 - Information technology - Programming languages - C  
 draft of standard above ,last edited 2012-Nov-11 20:09:34,https://port70.net/~nsz/c/c11/n1570.html
