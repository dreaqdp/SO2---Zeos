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

    
	runjp_rank(1,32);
	while(1){
		;
	}

	return 0;
	
	/*
	struct stats p_stats;
	char buff[20];
	//int pid = fork();
	while(1){
		//if(pid){
			int error = get_stats(1,&p_stats);
	        if (error) perror();
	        itoa(p_stats.total_trans,buff);
	        write(1,buff,20);write(1,"\n",2);
	    //}
	}
	return 0;*/
	
/*
	int pid=0;
    pid = fork();

    int i;
    for(i=0;pid==0&&i<20;++i){
    	pid=fork();
    	if(pid && i<9) exit();
    }
    char buff[10];
    itoa(pid,buff);
    while(1){
    	write(1,buff,10);
    	write(1,"\n",2);
    }
	return 0;*/
}



