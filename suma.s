# 1 "suma.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "suma.S"
ENTRY(add2)
push %ebp
mov %esp, %ebp
mov -12(%ebp), %eax
add -8(%ebp), %eax
mov %ebp, %esp
pop %ebp
ret
