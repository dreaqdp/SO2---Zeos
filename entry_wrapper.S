#include<asm.h>
ENTRY(sys_write_wrapper)
	push %ebp
	mov %esp, %ebp
	mov 8(%ebp), %ebx
	mov 12(%ebp), %ecx
	mov 16(%ebp), %edx
	mov $4, %eax
	int $0x80
	lea errno, %ebx
	mov %eax, (%ebx)
	mov $-1, %eax
	mov %ebp, %esp
	pop %ebp
	ret
