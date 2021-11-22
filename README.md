# cc
cc is my c compiler making project.


## file


### cc.h
header file

### main.c 

tokenizer and main function are written in this file.

### parse.c
parser 

### codegenerator.c
generator function which convert abstract syntax tree into assembly code 



## coding policy

#### memory management 
For simplicity, we will not manage memory in cc code.
We allocate memory to pointer but will not free memory　within  compiler life cycle.
Don't use "free" before changing this policy.


## Reference  
Rui Ueyama,低レイヤを知りたい人のためのCコンパイラ作成入門,2020-03-16,https://www.sigbus.info/compilerbook
