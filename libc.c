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

int sys_getpid_wrapper();
int getpid(){
	return sys_getpid_wrapper();
}

int sys_fork_wrapper();
int fork(){
  return sys_fork_wrapper();
}

int sys_exit_wrapper();
void exit(){
  sys_exit_wrapper();
}

int sys_get_stats_wrapper(int pid, struct stats *st);
int get_stats(int pid, struct stats *st){
  return sys_get_stats_wrapper(pid,st);
}

int sys_clone_wrapper();
int clone(void (*function)(void), void *stack){
  return sys_clone_wrapper(function, stack);
}

