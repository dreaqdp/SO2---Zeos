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
	addl $4, %esp

	pop %ebx
	pop %edi
	pop %esi
	mov %ebp, %esp
	pop %ebp
	ret
	
inner_task_switch:
    push %ebp
    mov %esp, %ebp
	mov 8(%ebp),%ebx  #@new
	#mov 16(%ebx),%esi #kernel esp del pcb del new
	mov %ebx, %esi
	add $4096,%esi
	#TSS.esp0=esp(abaix)
	lea tss, %edx
	mov %esi, 4(%edx)

	#MSR0x175=esp(abaix)
	push %esi;
	push $0x175 #cuidao si no compila
	call writeMSR
	add $8, %esp
	
	#guardar ebp (ho hem mogut adalt del cr3 per el cas dels threads)
	mov %ebp, %ecx
	and $0xFFF000,%ecx
	mov %ebp, 16(%ecx)

	#setcr3
	mov 4(%ecx),%edi #els threads no han de fer un canvi de directori si abans estava el mateix dir
		#dir new =? dir old
	cmpl 4(%ebx),%edi
	je etiqueta
	push 4(%ebx) #pointer de dir_page
	call set_cr3
	add $4, %esp
	

	#mov %esp, 16(%ecx)
	#4)
etiqueta:
	#mov %esi,%esp #canvi de pila kernel
	mov 16(%ebx),%esp
	pop %ebp
	ret

ENTRY(get_esp)
	mov %esp,%eax
    ret
ENTRY(get_ebp)
	mov %ebp,%eax
    ret
