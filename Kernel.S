#include <asm.h>
#include <segment.h>

#.extern writeMSR

ENTRY(task_switch)
	push %ebp
	mov %esp, %ebp
	
	push %esi
	push %edi
	push %ebx
	#inner ----
	mov 8(%ebp),%ebx  #@new
	
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
	and $4096,%ecx
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
	

	
