# cc
cc is a my c compiler making project.

## Files

### cc.h
header file

### tokenize.c
a tokenizer function and token making functions 

### main.c 

expect functions and  a main function are written in this file.

### parse.c
a parser function and node making functions 

### codegenerator.c
a generate local variable function
a generator function which convert an abstract syntax tree into  assembly codes 

## coding policy

#### memory management 
For simplicity, we will not manage memories in cc codes.
We allocate memories to pointers but will not free memories within the compiler life cycle.
Don't use "free" before changing this policy.






## Reference  
Rui Ueyama,低レイヤを知りたい人のためのCコンパイラ作成入門,2020-03-16,https://www.sigbus.info/compilerbook
