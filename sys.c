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

extern union task_union *task;
extern struct list_head freequeue; 

int sys_fork()
{
  
  printk("heeeey");
  int PID=-1;
  if (list_empty(list_first(&freequeue))) return EAGAIN;
  struct task_struct* child_task = list_head_to_task_struct(list_first(&freequeue));
  list_del(list_first(&freequeue));
  copy_data((void *)current(), (void *)child_task, KERNEL_STACK_SIZE);
  allocate_DIR(child_task);
  
  // e i)
  page_table_entry * father_tp = get_PT(current());
  page_table_entry * child_tp = get_PT(child_task);  // ei cal inicialitzar tots els camps de la pagina?
  int i;
  // user code pages child <= user code pages father
  for (i=0;i<NUM_PAG_CODE;++i){
  	copy_data(father_tp + PAG_LOG_INIT_CODE + i,child_tp + PAG_LOG_INIT_CODE + i,PAGE_SIZE);
  }
  // kernel code
  for (i=0;i<NUM_PAG_KERNEL;++i){
  	copy_data(father_tp + i,child_tp + i,PAGE_SIZE);
  }
  // user data frames initialization

  for (i=0;i<NUM_PAG_DATA;++i){
	int pagenumber = alloc_frame();
	if (pagenumber<0) return ENOMEM; //pregunta pablo : si hem d'alocar X pagines i nomes hi ha espai per X-1 no hauriem de desalocar les alocades? :D
	set_ss_pag(child_tp,PAG_LOG_INIT_DATA+i,pagenumber);
	// creem temp pel pare		//si peta ens mirem aixo pq hem confiat massa :D
	set_ss_pag(father_tp,PAG_LOG_INIT_DATA+NUM_PAG_DATA+i,pagenumber);
	copy_data(father_tp + PAG_LOG_INIT_DATA + i, father_tp+PAG_LOG_INIT_DATA+NUM_PAG_DATA+i, PAGE_SIZE );
  }
  //copy_data(father_tp + PAG_LOG_INIT_DATA , father_tp+PAG_LOG_INIT_DATA+NUM_PAG_DATA , PAGE_SIZE);

  for(i=0;i<NUM_PAG_DATA;++i){
					  //si peta ens mirem aixo pq hem confiat massa :D
	del_ss_pag(father_tp,PAG_LOG_INIT_DATA+NUM_PAG_DATA+i);
 }
  set_cr3(get_DIR(current()));
  
  PID = (((unsigned int)child_task - (unsigned int)task)>>12) + 1313; //vacio legal no es la seva posicio
  child_task->PID = PID;

  unsigned int offset = (unsigned int *)current()-(current()->kernel_esp);
  child_task->kernel_esp = (unsigned int)child_task+offset;
  unsigned int * stackpointer = child_task->kernel_esp;
  (*(stackpointer+3)) = 0; //eax de la kernel stack del fill = 0;
  //no cal kernel_ebp crec peeeeeero
  
  printk("byeeee");
  return PID;
}

void sys_exit()
{  
}

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






