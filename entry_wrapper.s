# 1 "entry_wrapper.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "entry_wrapper.S"
# 1 "include/asm.h" 1
# 2 "entry_wrapper.S" 2
.globl sys_write_wrapper; .type sys_write_wrapper, @function; .align 0; sys_write_wrapper:
 push %ebp
 mov %esp, %ebp
 mov 8(%ebp), %ebx #fd
 mov 12(%ebp), %ecx #buffer
 mov 16(%ebp), %edx #size
 push %ecx
 push %edx
 lea sysretaddress, %eax #perque pugui tornar el sysenter
 push %eax
 push %ebp #creem un fake dynamic link per facilitarnos la vida
 movl %esp, %ebp
 mov $4, %eax
 sysenter
sysretaddress:
    pop %ebp
 add $4, %esp #borrem el lea de sysretaddress
 pop %edx
 pop %ecx
 cmp $0, %eax
 je acaba
 lea errno, %ebx
 mov %eax, (%ebx)
 mov $-1, %eax
 jmp fi
acaba:
 mov 16(%ebp), %eax
fi:
 mov %ebp, %esp
 pop %ebp
 ret
