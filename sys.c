/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <errno.h>

#include <types.h>


#define LECTURA 0
#define ESCRIPTURA 1

extern unsigned int zeos_ticks;

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
  if(list_empty(list_first(&freequeue))) return EAGAIN;
  struct task_struct* child_task = list_head_to_task_struct(list_first(&freequeue));
  list_del(list_first(&freequeue));
  copy_data((void *)current(), (void *)child_task, KERNEL_STACK_SIZE);
  allocate_DIR(child_task);
  int pagenumber = alloc_frame();
  if (pagenumber<0) return ENOMEM;
  page_table_entry * tpentry = get_PT(child_task);  // ei cal inicialitzar tots els camps de la pagina?



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
int size_block = 4;
int sys_write(int fd, char * buffer, int size) {
	
	
	
	int error_fd = check_fd(fd, ESCRIPTURA);
	if (error_fd < 0) return -error_fd;
	if (buffer == NULL) return EFAULT;
	if (size < 0) return EINVAL;
	int i,curr_size=size_block;
	for (i=0; i<size; i+=size_block){
		char buffer_kernel[size_block];
		if(size-i<size_block) curr_size=size-i;
		int error_copy = copy_from_user(buffer+i, buffer_kernel, curr_size);
		if (error_copy < 0) return -error_copy;
		int ret_wconsole = -1;
	    if (fd == 1) {
			ret_wconsole = sys_write_console (buffer_kernel, curr_size);
			if (ret_wconsole < 0) return ret_wconsole;
		}
	}
	return 0;
}

int sys_gettime () {
	return zeos_ticks;
}
/*
extern TSS tss;
int sys_getpid(){
	return (*(tss.esp0&4096));
}*/





