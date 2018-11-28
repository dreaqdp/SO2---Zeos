#include<asm.h>
ENTRY(sys_write_wrapper)
	push %ebp
	mov %esp, %ebp
	#feedback e1c: guardar reg-param
	push %ebx
	push %ecx
	push %edx
	##
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
	mov 16(%ebp), %eax #retornem el tercer parametre (size)
fi_write:
	#feedback e1c: popejar reg guardats
	pop %edx
	pop %ecx
	pop %ebx
	##
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
	lea ret_fork, %eax #perque pugui tornar el sysenter
	push %eax
	push %ebp #creem un fake dynamic link per facilitarnos la vida
	movl %esp, %ebp
	mov $2, %eax #posar id de la sys table
	sysenter
ret_fork:
    pop %ebp
	add $4, %esp	#borrem el lea de retfork
	pop %edx
	pop %ecx
	cmp $0, %eax
	jge fi_fork
	not %eax #abs(errror) 
	inc %eax
	lea errno, %ebx
	mov %eax, (%ebx)
	mov $-1, %eax 
fi_fork:
	mov %ebp, %esp
	pop %ebp
	ret

ENTRY(sys_exit_wrapper)

	push %ebp
	mov %esp, %ebp
	push %ecx
	push %edx
	lea ret_exit, %eax #perque pugui tornar el sysenter
	push %eax
	push %ebp #creem un fake dynamic link per facilitarnos la vida
	movl %esp, %ebp
	mov $1, %eax
	sysenter
ret_exit:
    pop %ebp
	add $4, %esp	#borrem el lea de ret_pid
	pop %edx
	pop %ecx
	mov %ebp, %esp
	pop %ebp
	ret	

ENTRY(sys_get_stats_wrapper)
	push %ebp
	mov %esp, %ebp
	#feedback e1c: guardar parametres
	push %ebx
	push %ecx
	##
	mov 8(%ebp), %ebx    #pid
	mov 12(%ebp), %ecx   #stats
	push %ecx
	push %edx
	lea retgetstats, %eax #perque pugui tornar el sysenter
	push %eax
	push %ebp #creem un fake dynamic link per facilitarnos la vida
	movl %esp, %ebp
	mov $35, %eax
	sysenter
retgetstats:
    pop %ebp
	add $4, %esp	#borrem el lea de retwrite
	pop %edx
	pop %ecx
	cmp $0, %eax
	je fi_getstats
	lea errno, %ebx
	mov %eax, (%ebx)
	mov $-1, %eax 
fi_getstats:
	#feedback e1c: popejar regs guardats
	pop %ecx
	pop %ebx
	##
	mov %ebp, %esp
	pop %ebp
	ret


ENTRY(sys_clone_wrapper)
	push %ebp
	mov %esp, %ebp
	push %ecx
	push %edx
	##params
	push %ebx
	push %ecx
	mov 8(%ebp), %ebx    #function
	mov 12(%ebp), %ecx   #stack
	##
	lea ret_clone, %eax #perque pugui tornar el sysenter
	push %eax
	push %ebp #creem un fake dynamic link per facilitarnos la vida
	movl %esp, %ebp #guardarse el esp per sysenter
	mov $19, %eax #posar id de la sys table
	sysenter
ret_clone:
    pop %ebp
	add $4, %esp	#borrem el lea de retfork
	pop %edx
	pop %ecx
	cmp $0, %eax
	jge fi_clone
	not %eax #abs(errror) 
	inc %eax
	lea errno, %ebx
	mov %eax, (%ebx)
	mov $-1, %eax 
fi_clone:
	##params
	pop %ecx
	pop %ebx
	#
	mov %ebp, %esp
	pop %ebp
	ret


ENTRY(sys_sem_init_wrapper)
	push %ebp
	mov %esp, %ebp
	#feedback e1c: guardar parametres
	push %ebx
	push %ecx
	##
	mov 8(%ebp), %ebx    #n_sem
	mov 12(%ebp), %ecx   #value
	push %ecx
	push %edx
	lea retseminit, %eax #perque pugui tornar el sysenter
	push %eax
	push %ebp #creem un fake dynamic link per facilitarnos la vida
	movl %esp, %ebp
	mov $21, %eax
	sysenter
retseminit:
    pop %ebp
	add $4, %esp	#borrem el lea de retwrite
	pop %edx
	pop %ecx
	cmp $0, %eax
	je fi_seminit
	not %eax #abs(errror) 
	inc %eax
	lea errno, %ebx
	mov %eax, (%ebx)
	mov $-1, %eax 
fi_seminit:
	#feedback e1c: popejar regs guardats
	pop %ecx
	pop %ebx
	##
	mov %ebp, %esp
	pop %ebp
	ret

ENTRY(sys_sem_wait_wrapper)
	push %ebp
	mov %esp, %ebp
	#feedback e1c: guardar parametres
	push %ebx
	##
	mov 8(%ebp), %ebx    #n_sem
	push %ecx
	push %edx
	lea retsemwait, %eax #perque pugui tornar el sysenter
	push %eax
	push %ebp #creem un fake dynamic link per facilitarnos la vida
	movl %esp, %ebp
	mov $22, %eax
	sysenter
retsemwait:
    pop %ebp
	add $4, %esp	#borrem el lea de retwrite
	pop %edx
	pop %ecx
	cmp $0, %eax
	je fi_semwait
	not %eax #abs(errror) 
	inc %eax
	lea errno, %ebx
	mov %eax, (%ebx)
	mov $-1, %eax 
fi_semwait:
	#feedback e1c: popejar regs guardats
	pop %ebx
	##
	mov %ebp, %esp
	pop %ebp
	ret


ENTRY(sys_sem_signal_wrapper)
	push %ebp
	mov %esp, %ebp
	#feedback e1c: guardar parametres
	push %ebx
	##
	mov 8(%ebp), %ebx    #n_sem
	push %ecx
	push %edx
	lea retsemsignal, %eax #perque pugui tornar el sysenter
	push %eax
	push %ebp #creem un fake dynamic link per facilitarnos la vida
	movl %esp, %ebp
	mov $23, %eax
	sysenter
retsemsignal:
    pop %ebp
	add $4, %esp	#borrem el lea de retwrite
	pop %edx
	pop %ecx
	cmp $0, %eax
	je fi_semsignal
	not %eax #abs(errror) 
	inc %eax
	lea errno, %ebx
	mov %eax, (%ebx)
	mov $-1, %eax 
fi_semsignal:
	#feedback e1c: popejar regs guardats
	pop %ebx
	##
	mov %ebp, %esp
	pop %ebp
	ret


ENTRY(sys_sem_destroy_wrapper)
	push %ebp
	mov %esp, %ebp
	#feedback e1c: guardar parametres
	push %ebx
	##
	mov 8(%ebp), %ebx    #n_sem
	push %ecx
	push %edx
	lea retsemdestroy, %eax #perque pugui tornar el sysenter
	push %eax
	push %ebp #creem un fake dynamic link per facilitarnos la vida
	movl %esp, %ebp
	mov $24, %eax
	sysenter
retsemdestroy:
    pop %ebp
	add $4, %esp	#borrem el lea de retwrite
	pop %edx
	pop %ecx
	cmp $0, %eax
	je fi_semdestroy
	not %eax #abs(errror) 
	inc %eax	
	lea errno, %ebx
	mov %eax, (%ebx)
	mov $-1, %eax 
fi_semdestroy:
	#feedback e1c: popejar regs guardats
	pop %ebx
	#
	mov %ebp, %esp
	pop %ebp
	ret


ENTRY(sys_read_wrapper)
	push %ebp
	mov %esp, %ebp
	#feedback e1c: guardar reg-param
	push %ebx
	push %ecx
	push %edx
	##
	mov 8(%ebp), %ebx    #fd
	mov 12(%ebp), %ecx   #buffer
	mov 16(%ebp), %edx   #size
	push %ecx
	push %edx
	lea retread, %eax #perque pugui tornar el sysenter
	push %eax
	push %ebp #creem un fake dynamic link per facilitarnos la vida
	movl %esp, %ebp
	mov $3, %eax
	sysenter
retread:
    pop %ebp
	add $4, %esp	#borrem el lea de retwrite
	pop %edx
	pop %ecx
	cmp $0, %eax
	je fi_errread
	lea errno, %ebx
	mov %eax, (%ebx)
	mov $-1, %eax 
	jmp fi_read
fi_errread:
	mov 16(%ebp), %eax #retornem el tercer parametre (size)
fi_read:
	#feedback e1c: popejar reg guardats
	pop %edx
	pop %ecx
	pop %ebx
	##
	mov %ebp, %esp
	pop %ebp
	ret

ENTRY(sys_sbrk_wrapper)
	push %ebp
	mov %esp, %ebp
	push %ecx
	push %edx
	##params
	push %ebx
	mov 8(%ebp), %ebx    #increment
	##
	lea ret_sbrk, %eax #perque pugui tornar el sysenter
	push %eax
	push %ebp #creem un fake dynamic link per facilitarnos la vida
	movl %esp, %ebp #guardarse el esp per sysenter
	mov $13, %eax #posar id de la sys table
	sysenter
ret_sbrk:
    pop %ebp
	add $4, %esp	#borrem el lea de retfork
	pop %edx
	pop %ecx
	cmp $0, %eax
	jge fi_sbrk
	not %eax #abs(errror) 
	inc %eax
	lea errno, %ebx
	mov %eax, (%ebx)
	mov $-1, %eax 
fi_sbrk:
	##params
	pop %ecx
	#
	mov %ebp, %esp
	pop %ebp
	ret