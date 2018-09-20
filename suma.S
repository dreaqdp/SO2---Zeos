#include<asm.h>
ENTRY(add2)
push %ebp
mov %esp, %ebp
mov -12(%ebp), %eax
add -8(%ebp), %eax
mov %ebp, %esp
pop %ebp
ret
