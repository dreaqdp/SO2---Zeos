/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#define LECTURA 0
#define ESCRIPTURA 1

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
  int PID=-1;

  // creates the child process
  
  return PID;
}

void sys_exit()
{  
}
/* ops ens hem adonat que ja esta feta. som els putos amos
int sys_write_console (char *buffer, int size) {
	int x, y = 0;
	for (x = 0; x < size; x++) {
		printc_xy(x,y,buffer[x]);
	}
	return x;
}*/

int sys_write(int fd, char * buffer, int size) {
	
	int error_fd = check_fd(fd, ESCRIPTURA);
	if (error_fd < 0) return error_fd;
	if (buffer == NULL) return -20; //random value
	if (size < 0) return -25;  //random value
	char buffer_kernel[size];
	int error_copy = copy_from_user(buffer, buffer_kernel, size);
	if (error_copy < 0) return error_copy;
	int ret_wconsole = -1;
	if (fd == 1) {
		ret_wconsole = sys_write_console (buffer_kernel, size);
		if (ret_wconsole < 0) return ret_wconsole;
	}
	printk("arribo");
	return ret_wconsole;
}






