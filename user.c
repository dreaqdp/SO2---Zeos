#include <libc.h>
#include <io.h>

char buff[24];

int pid;
/*
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
*/

extern int errno;


extern int jeje;
int __attribute__ ((__section__(".text.main")))
  main()
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

	//runjp();
	int i;
	char buff[10];
	int pid = fork();
	if(pid!=0) {
		pid = fork();
	}
	if(pid!=0){
		int jaja = getpid();
	} 
	itoa(getpid(),buff);
	//write(1,buff,10);
	while(1){
	write(1,buff,10);
	}
	return 0;
}



