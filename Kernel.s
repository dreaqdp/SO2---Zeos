# 1 "Kernel.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "Kernel.S"
# 1 "include/asm.h" 1
# 2 "Kernel.S" 2
# 1 "include/segment.h" 1
# 3 "Kernel.S" 2

.globl task_switch; .type task_switch, @function; .align 0; task_switch:
 push %ebp
 mov %esp, %ebp

 push %esi
 push %edi
 push %ebx
 push 8(%ebp)
 call inner_task_switch
 pop %ebx
 pop %edi
 pop %esi

 mov %ebp, %esp
 pop %ebp
 ret

.globl inner_task_switch; .type inner_task_switch, @function; .align 0; inner_task_switch:
 push %ebp
 mov %esp, %ebp


 mov 8(%ebp),%eax #@new

 #TSS.esp0=esp
 lea tss, %edx
 mov %esp, 4(%edx)

 #MSR0x175=esp
 push %esp
 push $0x175 #cuidao si no compila
 call writeMSR
 add $8, %esp

 #setcr3
 push 4(%eax) #pointer de dir_page
 call set_cr3
 add $4, %esp

 #3
 mov %ebp, %ecx
 and $4096,%ecx
 mov %ebp, 20(%ecx) #eeeem preguntar

 #4)
 mov 20(%eax),%esp #canvi de pila kernel

 popl %ebp
 ret
