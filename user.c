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
/*
int heysocunthread(){
	

	int juju = sem_wait(1);

	write(1,"thread ",7);

	//while(1){
		//write(1,"thread ",7);
	//}
	exit();
}*/


int __attribute__ ((__section__(".text.main")))
  main()
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
    
    
	runjp_rank(0,10);
	while(1);
	
	int * volatile iniaddress = sbrk(0);
	int * volatile finaladdress = sbrk(3);
	//int * volatile finalfinaladdress = sbrk(-6);
	*(iniaddress + 1024) = 1; //preguntar: hauria de poder accedir a tota la pagina reservada?Com limitar-li l'acces?

	while(1);



	/*
	char buff [3];
	read(0, buff, 3);
	write(1, buff, 3);


	while(1){
		write(1,"papi",5);
	}*/
	

	return 0;

}



