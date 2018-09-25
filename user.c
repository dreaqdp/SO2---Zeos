#include <libc.h>
#include <io.h>

char buff[24];

int pid;

int add(int par1, int par2){
	return par1+par2;
}

long inner(long n)
{
	int i;
	long suma;
	suma = 0;
	for (i=0; i<n; i++) suma = suma + i;
	return suma;
}
long outer(long n)
{
	int i;
	long acum;
	acum = 0;
	for (i=0; i<n; i++) acum = acum + inner(i);
	return acum;
}

int sys_write_wrapper(int fd, char* buffer, int size);

int __attribute__ ((__section__(".text.main")))
  main()
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
	/*
	long count, acum;
	count = 75;
	acum = 0;
	acum = outer(count);*/
	
	char test_buffer[10] = {'h','o','l','a'};
	int err = sys_write_wrapper(1,test_buffer,4);
	
	char temp[100];
	itoa(err,temp);
	sys_write_wrapper(1,temp,100);
	
	
	while(1);
	return 0;
}



