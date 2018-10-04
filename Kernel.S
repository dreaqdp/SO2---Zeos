#include <asm.h>
#include <segment.h>

ENTRY(task_switch)
	push %ebp
	mov $esp, %ebp
	push %esi
	push %edi
	push %ebx
	call inner_task_switch
	pop %ebx
	pop %edi
	pop %esi
	mov %ebp, %esp
	pop &ebp
	ret
	
ENTRY(inner_task_switch)
	
