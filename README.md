# cc
cc is a my subset of c compiler making project.


## Files

### cc.h
header file which all used functions are declared in.
Also, briefs of those functions  are written here.

### Token.c  
new token functions which make  new tokens.

### Node.c
new node functions which make new nodes for abstract syntax tree.

### Vector.c
this file serves vector utilities.

### tokenize.c
a tokenizer function

### main.c 
expect functions and  a main function are written in this file.

### parse.c
a parser function

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
 draft of stamderd above ,last edited 2012-Nov-11 20:09:34,https://port70.net/~nsz/c/c11/n1570.html
