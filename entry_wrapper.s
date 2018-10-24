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
 #feedback e1c: guardar parametres
 push %ebx
 push %ecx
 push %edx
 ##
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
 mov 16(%ebp), %eax #retornem el tercer parametre (size)
fi_write:
 #feedback e1c: popejar reg guardats
 pop %edx
 pop %ecx
 pop %ebx
 ##
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
 mov %ebp, %esp
 pop %ebp
 ret

.globl sys_getpid_wrapper; .type sys_getpid_wrapper, @function; .align 0; sys_getpid_wrapper:

 push %ebp
 mov %esp, %ebp
 push %ecx
 push %edx
 lea ret_pid, %eax #perque pugui tornar el sysenter
 push %eax
 push %ebp #creem un fake dynamic link per facilitarnos la vida
 movl %esp, %ebp
 mov $20, %eax
 sysenter
ret_pid:
    pop %ebp
 add $4, %esp #borrem el lea de ret_pid
 pop %edx
 pop %ecx
 mov %ebp, %esp
 pop %ebp
 ret

.globl sys_fork_wrapper; .type sys_fork_wrapper, @function; .align 0; sys_fork_wrapper:
 push %ebp
 mov %esp, %ebp
 push %ecx
 push %edx
 lea ret_fork, %eax #perque pugui tornar el sysenter
 push %eax
 push %ebp #creem un fake dynamic link per facilitarnos la vida
 movl %esp, %ebp
 mov $2, %eax #posar id de la sys table
 sysenter
ret_fork:
    pop %ebp
 add $4, %esp #borrem el lea de retfork
 pop %edx
 pop %ecx
 cmp $0, %eax
 jge fi_fork
 not %eax #abs(errror)
 inc %eax
 lea errno, %ebx
 mov %eax, (%ebx)
 mov $-1, %eax
fi_fork:
 mov %ebp, %esp
 pop %ebp
 ret

.globl sys_exit_wrapper; .type sys_exit_wrapper, @function; .align 0; sys_exit_wrapper:

 push %ebp
 mov %esp, %ebp
 push %ecx
 push %edx
 lea ret_exit, %eax #perque pugui tornar el sysenter
 push %eax
 push %ebp #creem un fake dynamic link per facilitarnos la vida
 movl %esp, %ebp
 mov $1, %eax
 sysenter
ret_exit:
    pop %ebp
 add $4, %esp #borrem el lea de ret_pid
 pop %edx
 pop %ecx
 mov %ebp, %esp
 pop %ebp
 ret

.globl sys_get_stats_wrapper; .type sys_get_stats_wrapper, @function; .align 0; sys_get_stats_wrapper:
 push %ebp
 mov %esp, %ebp
 #feedback e1c: guardar parametres
 push %ebx
 push %ecx
 ##
 mov 8(%ebp), %ebx #pid
 mov 12(%ebp), %ecx #stats
 push %ecx
 push %edx
 lea retgetstats, %eax #perque pugui tornar el sysenter
 push %eax
 push %ebp #creem un fake dynamic link per facilitarnos la vida
 movl %esp, %ebp
 mov $35, %eax
 sysenter
retgetstats:
    pop %ebp
 add $4, %esp #borrem el lea de retwrite
 pop %edx
 pop %ecx
 cmp $0, %eax
 je fi_getstats
 lea errno, %ebx
 mov %eax, (%ebx)
 mov $-1, %eax
fi_getstats:
 #feedback e1c: popejar regs guardats
 pop %ecx
 pop %ebx
 ##
 mov %ebp, %esp
 pop %ebp
 ret
