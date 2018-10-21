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


extern struct task_struct * task1;
extern struct list_head readyqueue;
extern union task_union *task;
extern struct list_head freequeue; 
 
extern int global_PID;
unsigned int get_esp();
unsigned int get_ebp();



int ret_from_fork(){
  return 0;
}
extern void printnum(int n);
struct task_struct* child_task;

int contador=0;

int sys_fork(){
  int PID=-1;
  if (list_empty(&freequeue)) return -EAGAIN;
  //struct task_struct* child_task = list_head_to_task_struct(list_first(&freequeue));
  struct list_head * h = list_first(&freequeue);
  list_del(h);
  child_task = list_head_to_task_struct(h);
  //1->copiem l'union de la tasca del pare al fill
  copy_data((void *)current(), (void *)child_task, (int)sizeof(union task_union));
  //2->aloquem un directori (i tp) pel fill (es posa a la seva PCB també)
  allocate_DIR(child_task);
  //3->agafem les taules de pagines de pare i fill
  page_table_entry * father_tp = get_PT(current());
  page_table_entry * child_tp = get_PT(child_task);
  //4->inicialitzo les pagines del fill corresponents al codi, mapejantles igual que les del pare
  int i;
  for (i=0;i<NUM_PAG_CODE;++i){
    set_ss_pag(child_tp,PAG_LOG_INIT_CODE + i, get_frame(father_tp, PAG_LOG_INIT_CODE+i));
  } 
  //5-> inicialitzo les pagines del fill corresponents al kernel, mapejantles igual que les del pare
  for (i=0;i<NUM_PAG_KERNEL;++i){
    set_ss_pag(child_tp,i, get_frame(father_tp, i));
  }
  //6->junt : alloc fisiques dades fill, ini entrades TP de dades fill, temp i copia de les del pare.
  int allocatedpages[NUM_PAG_DATA]; //vector per poder tirar enrere en cas de ENOMEM
  for (i=0;i<NUM_PAG_DATA;++i){
    //alloquem una frame fisica i la guardem al vector de pagines alocades
    int pagenumber = alloc_frame();
    allocatedpages[i]=pagenumber;
    if (pagenumber<0) { //en cas d'error, fem la marcha atras.
      int j;
      for(j=0;j<i;++j) free_frame(allocatedpages[j]);
      return -ENOMEM;
    }
    //inicialitzem una entrada de dades del child mapejada a la nova frame
    set_ss_pag(child_tp,PAG_LOG_INIT_DATA+i,pagenumber); 
    //inicialitzem una entrada de dades del pare mapejada a la nova frame per poder escriure coses a la física   
    set_ss_pag(father_tp,PAG_LOG_INIT_DATA+NUM_PAG_DATA+i,pagenumber);
    //el copy_data funciona amb @ lògiques (comencen al "0" per cada proces). el <<12 es pq nomes guardem un "tag" de la pag.física
    copy_data((void*)((PAG_LOG_INIT_DATA + i)<<12), (void*)((PAG_LOG_INIT_DATA+NUM_PAG_DATA+i)<<12), PAGE_SIZE); 
    //borrem les pàgines temporals que hem creat en la TP del pare
    del_ss_pag(father_tp,PAG_LOG_INIT_DATA+NUM_PAG_DATA+i);
  }
  //fem flush del TLB del pare
  set_cr3(get_DIR(current()));
  //Actualitzem el PID del fill
  PID = global_PID++;
  child_task->PID = PID;
  //Actualizem el kernel_esp del fill, basantme en la transparencia nº52 del tema 4
  //crec que lo de -18 i -19 era si no miraves que a les transpes posa que utilitzis el ebp i no el esp
  unsigned int ebp_father = get_ebp();
  unsigned int offset = ebp_father - (unsigned int)current();
  unsigned int * stackpointer = (unsigned int *) ( (unsigned int)child_task + offset);
  (*stackpointer) = (unsigned int)&ret_from_fork;
  stackpointer = stackpointer - 1;
  (*stackpointer) = 0;
  child_task->kernel_esp = stackpointer;
  //encuem el fill a la readyqueue
  list_add_tail(&(child_task->list), &readyqueue);
  child_task->state = ST_READY;
  //retornem el PID del fill
  return PID;
}



void sys_exit()
{  
   //per l'andrea: afeir a freeqeueu i no cal el bucle, ja el fa free user pages
   /*page_table_entry * curr_tp = get_PT(current());
   int i;
   for(i=0;i<NUM_PAG_DATA;++i){
     free_frame(get_frame(curr_tp,PAG_LOG_INIT_DATA+i));
   }*/
   free_user_pages(current());
   list_add_tail(&(current()->list), &freequeue);
   sched_next_rr();

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
//per l'andrea: he tret les variables locals current_time..
void enter_system(){
//  unsigned long current_time = get_ticks();
//a)
  current()->p_stats.user_ticks += get_ticks(); - current()->p_stats.elapsed_total_ticks;
  current()->p_stats.elapsed_total_ticks = get_ticks();
}
void exit_system(){
//b)
 // unsigned long current_time = get_ticks();
  current()->p_stats.system_ticks += get_ticks(); - current()->p_stats.elapsed_total_ticks;
  current()->p_stats.elapsed_total_ticks = get_ticks();
}
//per l'andrea : afegit el acces_ok..no se si esta be
int sys_get_stats(int pid, struct stats *st){
   int i;
   char b = 0;
   if (st==NULL || !access_ok(VERIFY_READ,st,sizeof(struct stats))) return EFAULT;
   if (sizeof(*(st))!= sizeof(struct stats)) return EINVAL;
   if(pid<0) return EINVAL;
   for(i=0;i<NR_TASKS && b==0;++i){
      if (pid == task[i].task.PID) {
        *(st) = current()->p_stats;
        b=1;
        return 0;
      }
   }
   return ESRCH;

}




