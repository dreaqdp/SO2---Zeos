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
 mov 8(%ebp), %ebx
 mov 12(%ebp), %ecx
 mov 16(%ebp), %edx
 mov $4, %eax
 int $0x80
 lea errno, %ebx
 mov %eax, (%ebx)
 mov $-1, %eax
 mov %ebp, %esp
 pop %ebp
 ret
