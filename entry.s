# 1 "entry.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "entry.S"




# 1 "include/asm.h" 1
# 6 "entry.S" 2
# 1 "include/segment.h" 1
# 7 "entry.S" 2
# 80 "entry.S"
.globl keyboard_handler; .type keyboard_handler, @function; .align 0; keyboard_handler:
hola:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 movb $0x20, %al; outb %al, $0x20;
 call keyboard_routine
 pop %ebx; pop %ecx; pop %edx; pop %esi; pop %edi; pop %ebp; pop %eax; pop %ds; pop %es; pop %fs; pop %gs
 iret

.globl clk_handler; .type clk_handler, @function; .align 0; clk_handler:
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 movb $0x20, %al; outb %al, $0x20;
 call clk_routine
 pop %ebx; pop %ecx; pop %edx; pop %esi; pop %edi; pop %ebp; pop %eax; pop %ds; pop %es; pop %fs; pop %gs
 iret


#.globl system_call_handler; .type system_call_handler, @function; .align 0; system_call_handler:
 # pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 # cmpl $0, %EAX
# jl err
# cmpl $MAX_SYSCALL, %EAX
# jg err
# call *sys_call_table(, %EAX, 0x04)
# jmp fin
#err:
# movl $-38, %EAX
#fin:
# movl %EAX, 0x18(%esp)
# pop %ebx; pop %ecx; pop %edx; pop %esi; pop %edi; pop %ebp; pop %eax; pop %ds; pop %es; pop %fs; pop %gs
 # iret


.globl system_call_handler; .type system_call_handler, @function; .align 0; system_call_handler:
 push $0x2B #stack segment
 push %EBP #stack adress
 pushfl
 push $0x23 #code segment
 push 4(%EBP) #code adress
 pushl %gs; pushl %fs; pushl %es; pushl %ds; pushl %eax; pushl %ebp; pushl %edi; pushl %esi; pushl %edx; pushl %ecx; pushl %ebx; movl $0x18, %edx; movl %edx, %ds; movl %edx, %es
 push %eax
 call enter_system
 pop %eax
 cmpl $0, %EAX
 jl sysenter_err
 cmpl $MAX_SYSCALL, %EAX
 jg sysenter_err
 call *sys_call_table(, %EAX, 0x04)
 jmp sysenter_fin
sysenter_err:
 movl $-38, %EAX
sysenter_fin:
 movl %EAX, 0x18(%esp)
 call exit_system
 pop %ebx; pop %ecx; pop %edx; pop %esi; pop %edi; pop %ebp; pop %eax; pop %ds; pop %es; pop %fs; pop %gs
 movl (%ESP), %EDX #code address (return adress)
 movl 12(%ESP), %ECX #stack address
 sti
 sysexit

.globl writeMSR; .type writeMSR, @function; .align 0; writeMSR:
 push %ebp
 mov %esp,%ebp
    movl 8(%ebp), %ecx #identificador del msr register
    movl $0,%edx #part alta del valor (no tindrem mai part alta, treballem en 32)
    movl 12(%ebp),%eax #part baixa del valor
 wrmsr
 movl %ebp,%esp
 pop %ebp
 ret
