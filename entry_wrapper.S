#include<asm.h>
ENTRY(sys_write_wrapper)
	push %ebp
	mov %esp, %ebp
	mov 8(%ebp), %ebx    #fd
	mov 12(%ebp), %ecx   #buffer
	mov 16(%ebp), %edx   #size
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
	add $4, %esp	#borrem el lea de retwrite
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


ENTRY(sys_gettime_wrapper)

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
	add $4, %esp	#borrem el lea de ret_time
	pop %edx
	pop %ecx
#	cmp $0, %eax
#	jge acaba
#	lea errno, %ebx
#	mov %eax, (%ebx)
#	mov $-1, %eax 
#acaba:
	mov %ebp, %esp
	pop %ebp
	ret
	
	
	


