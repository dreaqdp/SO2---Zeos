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
	mov %ebp, %esp
	pop %ebp
	ret
	
ENTRY(sys_getpid_wrapper)

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
	add $4, %esp	#borrem el lea de ret_pid
	pop %edx
	pop %ecx
	mov %ebp, %esp
	pop %ebp
	ret	
	
ENTRY(sys_fork_wrapper)
	push %ebp
	mov %esp, %ebp
	push %ecx
	push %edx
	lea retfork, %eax #perque pugui tornar el sysenter
	push %eax
	push %ebp #creem un fake dynamic link per facilitarnos la vida
	movl %esp, %ebp
	mov $2, %eax #posar id de la sys table
	sysenter
retfork:
    pop %ebp
	add $4, %esp	#borrem el lea de retwrite
	pop %edx
	pop %ecx
	cmp $0, %eax
	je fi_errfork
	lea errno, %ebx
	mov %eax, (%ebx)
	mov $-1, %eax 
	jmp fi_fork
fi_errfork:
	mov 16(%ebp), %eax
fi_fork:
	mov %ebp, %esp
	pop %ebp
	ret

