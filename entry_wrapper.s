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
 lea retwrite, %eax #perque pugui tornar el sysenter
 push %eax
 push %ebp #creem un fake dynamic link per facilitarnos la vida
 movl %esp, %ebp
 mov $4, %eax
 sysenter
retwrite:
    pop %ebp
 add $4, %esp #borrem el lea de retwrite
 pop %edx
 pop %ecx
 cmp $0, %eax
 je fi_errwrite
 lea errno, %ebx
 mov %eax, (%ebx)
 mov $-1, %eax
 jmp fi_write
fi_errwrite:
 mov 16(%ebp), %eax
fi_write:
 mov %ebp, %esp
 pop %ebp
 ret


.globl sys_gettime_wrapper; .type sys_gettime_wrapper, @function; .align 0; sys_gettime_wrapper:

 push %ebp
 mov %esp, %ebp
 push %ecx
 push %edx
 lea ret_time, %eax #perque pugui tornar el sysenter
 push %eax
 push %ebp #creem un fake dynamic link per facilitarnos la vida
 movl %esp, %ebp
 mov $10, %eax
 sysenter
ret_time:
    pop %ebp
 add $4, %esp #borrem el lea de ret_time
 pop %edx
 pop %ecx
# cmp $0, %eax
# jge acaba
# lea errno, %ebx
# mov %eax, (%ebx)
# mov $-1, %eax
#acaba:
 mov %ebp, %esp
 pop %ebp
 ret
