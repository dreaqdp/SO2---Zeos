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
extern struct list_head readyqueue; 
extern int global_PID;
unsigned int get_esp();
unsigned int get_ebp();


struct task_struct* child_task;


int ret_from_fork(){
  printk("xd");
  return 0;
}


int sys_fork()
{
  int PID=-1;
  if (list_empty(list_first(&freequeue))) return -EAGAIN;
  child_task = list_head_to_task_struct(list_first(&freequeue));
  list_del(list_first(&freequeue));
  copy_data((void *)current(), (void *)child_task, (int)sizeof(union task_union));
  allocate_DIR(child_task);
  
  // e i)
  page_table_entry * father_tp = get_PT(current());
  page_table_entry * child_tp = get_PT(child_task);  // ei cal inicialitzar tots els camps de la pagina?
  int i;
  // user code pages child <= user code pages father
  int size_pte = (int) sizeof(page_table_entry);
  for (i=0;i<NUM_PAG_CODE;++i){
  	copy_data(father_tp + PAG_LOG_INIT_CODE + i,child_tp + PAG_LOG_INIT_CODE + i, size_pte);
  } 
  // kernel code
  for (i=0;i<NUM_PAG_KERNEL;++i){
  	copy_data(father_tp + i,child_tp + i,size_pte);
  }
  // user data frames initialization

  for (i=0;i<NUM_PAG_DATA;++i){
	int pagenumber = alloc_frame();
	if (pagenumber<0) return -ENOMEM; //pregunta pablo : si hem d'alocar X pagines i nomes hi ha espai per X-1 no hauriem de desalocar les alocades? :D
	set_ss_pag(child_tp,PAG_LOG_INIT_DATA+i,pagenumber);
	// creem temp pel pare		//si peta ens mirem aixo pq hem confiat massa :D
	set_ss_pag(father_tp,PAG_LOG_INIT_DATA+NUM_PAG_DATA+i,pagenumber);
	copy_data(father_tp + PAG_LOG_INIT_DATA + i, father_tp+PAG_LOG_INIT_DATA+NUM_PAG_DATA+i, size_pte);
  }
  
  for(i=0;i<NUM_PAG_DATA;++i){
					  //si peta ens mirem aixo pq hem confiat massa :D
	del_ss_pag(father_tp,PAG_LOG_INIT_DATA+NUM_PAG_DATA+i);
 }
  set_cr3(get_DIR(current()));
  list_add(&(child_task->list), &readyqueue);
  //PID = (((unsigned int)child_task - (unsigned int)task)>>12) + 1313; //vacio legal no es la seva posicio
  PID = global_PID++;
  child_task->PID = PID;

  //cuidao
 
  unsigned int esp_father = get_esp();
  unsigned int offset = get_esp() - (unsigned int)current();
  unsigned int * stackpointer = (unsigned int *)((unsigned int )(child_task)+offset);
  child_task->kernel_esp = (unsigned int *)stackpointer - 19;
  /*unsigned int ebp_father = get_ebp();
  unsigned int offset = ebp_father - (unsigned int)current();
  unsigned int * stackpointer = (unsigned int *)((unsigned int )(child_task)+offset);

  child_task->kernel_esp = ebp_father + 1;
  unsigned int ebp_temp = *((unsigned int *)ebp_father);
  child_task->kernel_esp = child_task->kernel_esp - 1;
  (*(child_task->kernel_esp)) = (unsigned int)&ret_from_fork;
  child_task->kernel_esp = child_task->kernel_esp - 1;
  (*(child_task->kernel_esp)) = ebp_temp;*/



  (*(stackpointer - 18)) = (unsigned int)&ret_from_fork;
  (*(stackpointer - 19)) = 0; //valor random
/*
  (*(stackpointer3)) = 0; //eax de la kernel stack del fill = 0;
  (*(stackpointer+1)) = &ret_from_fork; //*((unsigned int *)get_ebp()+1);*/
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






