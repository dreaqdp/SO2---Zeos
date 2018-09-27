/*
 * libc.c 
 */

#include <libc.h>

#include <types.h>

int errno;

void itoa(int a, char *b)
{
  int i, i1;
  char c;
  
  if (a==0) { b[0]='0'; b[1]=0; return ;}
  
  i=0;
  while (a>0)
  {
    b[i]=(a%10)+'0';
    a=a/10;
    i++;
  }
  
  for (i1=0; i1<i/2; i1++)
  {
    c=b[i1];
    b[i1]=b[i-i1-1];
    b[i-i1-1]=c;
  }
  b[i]=0;
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}

void perror(void){
	char err_buffer[3]; //3 digits
	itoa(errno,err_buffer);
	write(1,err_buffer,3);
}

int sys_write_wrapper(int fd, char* buffer, int size);
int write(int fd, char* buffer, int size) {
	return sys_write_wrapper(fd, buffer, size);
}

int sys_gettime_wrapper ();
int gettime () {
	return sys_gettime_wrapper();
}

