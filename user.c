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
	
	while(1);
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
	//runjp_rank(0,5);
	while(1);
	
	//char * volatile iniaddress = sbrk(1458176);
	//*(iniaddress) = '1';
 	//int * volatile finaladdress = sbrk((4096*(1024-256-8-20)));

	int * volatile maincra = sbrk(10);
	int * volatile nite4 = sbrk(-100000000);
	*(maincra)=0xdead;

	while(1);

	volatile unsigned int reservat = 0;
	int * volatile jujujyjy = sbrk(0);
	while(sbrk(4096)!=-1){
		++reservat;
	}
	int * volatile jejejaja = sbrk(-(reservat+1)*4096);
	int * volatile jijijojo = sbrk(0);
	reservat=0;
	while(sbrk(4096)!=-1){
		++reservat;
	}
	int * volatile jjjj = sbrk(0);
	//int * volatile finalfinaladdress = sbrk(-6);
	//*(iniaddress + 1024) = 1; //preguntar: hauria de poder accedir a tota la pagina reservada?Com limitar-li l'acces?

	while(1){
		write(1,"papi",5);
	}



	/*
	char buff [3];
	read(0, buff, 3);
	write(1, buff, 3);


	while(1){
		write(1,"papi",5);
	}*/
	

	return 0;

}



