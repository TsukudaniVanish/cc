# cc
[![C/C++ CI](https://github.com/TsukudaniVanish/cc/actions/workflows/c.yml/badge.svg)](https://github.com/TsukudaniVanish/cc/actions/workflows/c.yml)  
cc is a subset of c compiler. It will cover the c89 syntax in th future. 
This product is developed for my study of computer science. Pull requests which are related to language implementation will be declined since I want to study implementing features and do that by my self. Any issues are welcome and will help me for developing this project. Thank you for your understanding!

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


### test/

##### test.h 
A test header file for test.c
##### test.c 
A test file for application. This file is not used to build cc.

#### backend/
##### error_point.c
All error point logics are controlled here. 

##### file.c 
A file reader and an include path finder.

### cc.h
A header file which all used functions are declared in.
Also, briefs of those functions  are written here.

### Token.c  
New token functions which make  new tokens and token utilities

### Node.c
New node functions which make new nodes for abstract syntax tree

### Vector.c
This file serves vector utilities.

### Map.c
Hash map utilities

### String.c
String utilities such as strlen, strncmp, strncpy in string.h.

### unittest.c
Functions for unit test.

### tokenize.c
A tokenizer

### main.c 
A main function are written in this file.

### parse.c
Parsing functions.

### codegenerator.c
Generator functions which convert an abstract syntax tree into  assembly codes.

## todo
- [x] refactoring/ remove include standard library
- [x] allow type cast in a global declaration
- [x] add %
- [x] add unsigned long 
- [ ] add macro operator #, ##
- [ ] fix the extern static variables code generation
- [x] add va-arg function declaration 
- [x] add include file path find algorithm 
- [x] add #ifndef, #ifdef
- [x] add #elif #else
- [x] add  a character  literal ''
- [x] add type cast 
- [x] add long and long int
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
