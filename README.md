# cc
cc is my c compiler making project.

## coding policy

#### memory management 
For simplicity, we will not manage memory in cc code.
we allocate memory to pointer but will not free memory　within  compiler life cycle.
don't use "free" before changeing this policy.

## Reference  
Rui Ueyama,低レイヤを知りたい人のためのCコンパイラ作成入門,2020-03-16,https://www.sigbus.info/compilerbook
