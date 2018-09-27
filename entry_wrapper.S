#include<asm.h>
ENTRY(sys_write_wrapper)
	push %ebp
	mov %esp, %ebp
	mov 8(%ebp), %ebx    #fd
	mov 12(%ebp), %ecx   #buffer
	mov 16(%ebp), %edx   #size
	push %ecx
	push %edx
	lea sysretaddress, %eax #perque pugui tornar el sysenter
	push %eax
	push %ebp #creem un fake dynamic link per facilitarnos la vida
	movl %esp, %ebp
	mov $4, %eax
	sysenter
sysretaddress:
    pop %ebp
	add $4, %esp	#borrem el lea de sysretaddress
	pop %edx
	pop %ecx
	cmp $0, %eax
	je acaba
	lea errno, %ebx
	mov %eax, (%ebx)
	mov $-1, %eax 
	jmp fi
acaba:
	mov 16(%ebp), %eax
fi:
	mov %ebp, %esp
	pop %ebp
	ret

	
	
	


