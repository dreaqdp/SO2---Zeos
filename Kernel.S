#include <asm.h>
#include <segment.h>

ENTRY(task_switch)
	push %ebp
	mov %esp, %ebp
	
	push %esi
	push %edi
	push %ebx
	push 8(%ebp)
	call inner_task_switch
	add $4, %esp
	pop %ebx
	pop %edi
	pop %esi
	
	mov %ebp, %esp
	pop %ebp
	ret
	
ENTRY(inner_task_switch)
	push %ebp
	mov %esp, %ebp

		
	mov 8(%ebp),%eax  #@new
	
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
	
