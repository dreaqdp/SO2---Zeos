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

int heysocunthread(){
	

	int juju = sem_wait(1);

	write(1,"thread ",7);

	//while(1){
		//write(1,"thread ",7);
	//}
	exit();
}


int __attribute__ ((__section__(".text.main")))
  main()
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
    
    
	runjp();
	while(1);
	

/*
	int jaja = sem_init(1,0);

	char Stack[666];
	int c = clone(&heysocunthread,&Stack[666]);

	//int jiji = sem_signal(1);

	//int jeje = sem_destroy(1);

	while(1);*/
    
/*
	int fill = fork();
	if(!fill){
		char Stack[666];
		int c = clone(&heysocunthread,&Stack[666]);
		exit();
	}
	while(1){
		write(1,"hola",5);
	}
  */
	return 0;

}



