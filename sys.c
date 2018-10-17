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
    set_ss_pag(child_tp,PAG_LOG_INIT_CODE + i, get_frame(father_tp, PAG_LOG_INIT_CODE+i));
   // copy_data(father_tp + PAG_LOG_INIT_CODE + i,child_tp + PAG_LOG_INIT_CODE + i, size_pte);
  } 
  // kernel code
  for (i=0;i<NUM_PAG_KERNEL;++i){
    set_ss_pag(child_tp,i, get_frame(father_tp, i));
    //copy_data(father_tp + i,child_tp + i,size_pte);
  }
  // user data frames initialization

  for (i=0;i<NUM_PAG_DATA;++i){
  	int pagenumber = alloc_frame();
  	if (pagenumber<0) return -ENOMEM; //pregunta pablo : si hem d'alocar X pagines i nomes hi ha espai per X-1 no hauriem de desalocar les alocades? :D
  	set_ss_pag(child_tp,PAG_LOG_INIT_DATA+i,pagenumber);
  	// creem temp pel pare		//si peta ens mirem aixo pq hem confiat massa :D
  	set_ss_pag(father_tp,PAG_LOG_INIT_DATA+NUM_PAG_DATA+i,pagenumber);
  	copy_data((void*)((PAG_LOG_INIT_DATA + i)<<12), (void*)((PAG_LOG_INIT_DATA+NUM_PAG_DATA+i)<<12), PAGE_SIZE);
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

  //(*(stackpointer+3)) = 0; //eax de la kernel stack del fill = 0;
  //(*(stackpointer+1)) = &ret_from_fork; //*((unsigned int *)get_ebp()+1);*/

  return PID;
}

/*
int sys_fork() {

  int PID=5;
  
  // a
  if (list_empty(&freequeue)) return -1; // NO FREE PROCESS
  struct list_head *e = list_first(&freequeue);
  list_del(e);

  struct task_struct *new_t = list_entry(e, struct task_struct, anchor);
  union task_union *new_u = (union task_union*) new_t;

  char buf[30]; itoa(new_t->PID, &buf); printk("PID: "); printk(buf); printk("\n");
  
  struct task_struct *curr_t = current();
  union task_union *curr_u = (union task_union*) curr_t;

  // b
  copy_data(curr_u, new_u, KERNEL_STACK_SIZE * sizeof(long));

  // c
  allocate_DIR(new_t);
  
  // d
  int dataFrames[NUM_PAG_DATA];
  for (int i = 0; i < NUM_PAG_DATA; i++) {
    dataFrames[i] = alloc_frame();
    if (dataFrames[i] == -1) {
      // free all allocated pages until now, we won't use them
      for (int j = 0; j <= i; j++) free_frame(dataFrames[j]);
      return -1; // Not enough physical pages left for new process
    }
  }
  

  // e.i
  page_table_entry *new_PT =  get_PT(new_t);
  page_table_entry *curr_PT =  get_PT(curr_t);  
  // e.i.A  
  copy_data(curr_PT, new_PT, NUM_PAG_CODE * sizeof(page_table_entry));
  // e.i.B
  for (int i = 0; i < NUM_PAG_DATA; i++) {
    set_ss_pag(new_PT, PAG_LOG_INIT_DATA + i, dataFrames[i]);
  }

  // e.ii
  int firstFree = NUM_PAG_KERNEL + NUM_PAG_CODE + NUM_PAG_DATA;
  int p = firstFree;
  for (int i = 0; i < NUM_PAG_CODE; i++) {
    // p = first free page (entry != 0)
    while (curr_PT[p].entry != 0 && p < TOTAL_PAGES) p++;
    // we didn't find a page
    if (p == TOTAL_PAGES) {
      if (p == 0) return -1; // no free pages in parent process
      
      // we found a page at some point, but now we're full, flush TLB and start over
      set_cr3(curr_t->dir_pages_baseAddr);
      p = firstFree;
      i--;
      continue;
    }
    // there is a free page
    // e.ii.A
    set_ss_pag(curr_PT, p, curr_PT[PAG_LOG_INIT_DATA+i].bits.pbase_addr);
    // e.ii.B

    copy_data(((PAG_LOG_INIT_DATA+i)*PAGE_SIZE), p*PAGE_SIZE, PAGE_SIZE);
    // e.ii.C
    del_ss_pag(curr_PT, p);

    
    // flush tlb
    set_cr3(curr_t->dir_pages_baseAddr);
  }

  // f
  new_t->PID++; // TODO find a better pid lol

  // g
  // eax = 0 in the child
  new_u->stack[KERNEL_STACK_SIZE-5] = 0; // return 0 to child


  
  

  int ebp_offset = get_ebp() & 0xfff;
  int new_ebp = ebp_offset + (int)new_t;



  new_u->stack[ebp_offset/sizeof(long)] = 0;
  new_u->task.kernel_esp = new_ebp;
  new_u->stack[(ebp_offset+4)/sizeof(long)] = &ret_from_fork;

  // h
  
  // i
  list_add_tail(&new_t->list, &readyqueue);

  // j
  
  return PID;
}
*/


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






