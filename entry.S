/*
 * entry.S - Entry point to system mode from user mode
 */

#include <asm.h>
#include <segment.h>


/**************************************************/
/**** Save & Restore ******************************/
/**                                              **/
/** When we change to privilege level 0 (kernel) **/
/** (through an interrupt, a system call, an     **/
/** exception ...) we must save the state of the **/
/** currently running task (save).               **/
/**                                              **/
/** Stack layout in 'systemCall':                **/
/**                                              **/
/**   0(%esp) - %ebx    \                        **/
/**   4(%esp) - %ecx     |                       **/
/**   8(%esp) - %edx     |                       **/
/**   C(%esp) - %esi     | Register saved        **/
/**  10(%esp) - %edi     |  by 'save'            **/
/**  14(%esp) - %ebp     |                       **/
/**  18(%esp) - %eax     |                       **/
/**  1C(%esp) - %ds      |                       **/
/**  20(%esp) - %es      |                       **/
/**  24(%esp) - %fs      |                       **/
/**  28(%esp) - %gs     /                        **/
/**  2C(%esp) - %eip    \                        **/
/**  30(%esp) - %cs      |                       **/
/**  34(%esp) - %eflags  |  Return context saved **/
/**  38(%esp) - %oldesp  |   by the processor.   **/
/**  3C(%esp) - %oldss  /                        **/
/**                                              **/
/**************************************************/

#define SAVE_ALL \
      pushl %gs; \
      pushl %fs; \
      pushl %es; \
      pushl %ds; \
      pushl %eax; \
      pushl %ebp; \
      pushl %edi; \
      pushl %esi; \
      pushl %edx; \
      pushl %ecx; \
      pushl %ebx; \
      movl $__KERNEL_DS, %edx;    \
      movl %edx, %ds;           \
      movl %edx, %es




#define RESTORE_ALL \
      pop %ebx; \
      pop %ecx; \
      pop %edx; \
      pop %esi; \
      pop %edi; \
      pop %ebp; \
      pop %eax; \
      pop %ds; \
      pop %es; \
      pop %fs; \
      pop %gs
      
	  /* feedback e1c: treure aquestes tres lineas de restore_all
	  #movl $__USER_DS, %edx; \
	  #movl %edx, %es; \
	  #movl %edx, %ds; \ 
	  */

#define EOI      \
      movb $0x20, %al; \
      outb %al, $0x20;

ENTRY(keyboard_handler)	
hola:
	SAVE_ALL 
	EOI
	call keyboard_routine
	RESTORE_ALL
	iret
	
ENTRY(clk_handler)	
	SAVE_ALL 
	EOI
	call clk_routine
	RESTORE_ALL
	iret


#ENTRY(system_call_handler)
#	SAVE_ALL
#	cmpl $0, %EAX
#	jl err
#	cmpl $MAX_SYSCALL, %EAX
#	jg err
#	call *sys_call_table(, %EAX, 0x04)
#	jmp fin
#err:
#	movl $-38, %EAX
#fin:
#	movl %EAX, 0x18(%esp)
#	RESTORE_ALL
#	iret


ENTRY(system_call_handler)
	push $__USER_DS  #stack segment
	push %EBP #stack adress
	pushfl
	push $__USER_CS  #code segment
	push 4(%EBP)   #code adress
	SAVE_ALL
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
	RESTORE_ALL
	movl (%ESP), %EDX #code address (return adress)
	movl 12(%ESP), %ECX #stack address
	sti
	sysexit

ENTRY(writeMSR)
	push %ebp
	mov %esp,%ebp
    movl 8(%ebp), %ecx #identificador del msr register
    movl $0,%edx #part alta del valor (no tindrem mai part alta, treballem en 32)
    movl 12(%ebp),%eax #part baixa del valor
	wrmsr
	movl %ebp,%esp
	pop %ebp
	ret
