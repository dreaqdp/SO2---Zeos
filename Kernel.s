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

#.extern writeMSR

.globl task_switch; .type task_switch, @function; .align 0; task_switch:
 push %ebp
 mov %esp, %ebp

 push %esi
 push %edi
 push %ebx
 #inner ----
 mov 8(%ebp),%ebx #@new

 #TSS.esp0=esp
 lea tss, %edx
 mov %esp, 4(%edx)

 #-- llegim el msr
 #mov $0x175, %ecx
 #rdmsr
 #-- \llegim msr



 #MSR0x175=esp
 push %esp
 push $0x175 #cuidao si no compila
 call writeMSR
 add $8, %esp

 #-- llegim el msr
 #mov $0x175, %ecx
 #rdmsr
 #-- \llegim msr

 #setcr3
 push 4(%ebx) #pointer de dir_page
 call set_cr3
 add $4, %esp

 #3)

 mov %ebp, %ecx
 and $0xfffff000,%ecx
 mov %ebp, 20(%eax)

 #4)
 mov 16(%ebx),%esp #canvi de pila kernel
 pop %ebp
 ret
 #inneer----

 pop %ebx
 pop %edi
 pop %esi

 pop %ebp
 ret
