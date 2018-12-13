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

//#include<circbuff.h>

#define LECTURA 0
#define ESCRIPTURA 1

extern unsigned int zeos_ticks;

int check_fd(int fd, int permissions)
{
  if ((fd!=1 && permissions == ESCRIPTURA) || (fd != 0 && permissions == LECTURA)) return -9; /*EBADF*/
  if ((fd == 1 && permissions!=ESCRIPTURA) || (fd == 0 && permissions != LECTURA)) return -13; /*EACCES*/
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

extern int dir_counter[NR_TASKS];

int sys_fork(){
  int PID=-1;
  if (list_empty(&freequeue)) return -EAGAIN;
  struct list_head * h = list_first(&freequeue);
  list_del(h);
  struct task_struct* child_task = list_head_to_task_struct(h);
  //1->copiem l'union de la tasca del pare al fill
  copy_data((void *)current(), (void *)child_task, (int)sizeof(union task_union));
  //2->aloquem un directori (i tp) pel fill (es posa a la seva PCB també)
  allocate_DIR(child_task);
  //3->agafem les taules de pagines de pare i fill
  page_table_entry * father_tp = get_PT(current());
  page_table_entry * child_tp = get_PT(child_task);
  //4->inicialitzar les pagines del fill corresponents al codi, mapejantles igual que les del pare
  int i;
  for (i=0;i<NUM_PAG_CODE;++i){
    set_ss_pag(child_tp,PAG_LOG_INIT_CODE + i, get_frame(father_tp, PAG_LOG_INIT_CODE+i));
  } 
  //5-> inicialitzar les pagines del fill corresponents al kernel, mapejantles igual que les del pare
  for (i=0;i<NUM_PAG_KERNEL;++i){
    set_ss_pag(child_tp,i, get_frame(father_tp, i));
  }

  // heap: inizialitzar les pag del fill corresponents al heap, mapejar-les igual que les del pare
  volatile unsigned int programbreak_page = (unsigned int)(current()->programbreak-1) >> 12;
  for (i = PAG_LOG_INIT_HEAP; i <= programbreak_page; i++) {
    set_ss_pag(child_tp, i, get_frame(father_tp, i));
  }


  //6->junt : alloc fisiques dades fill, ini entrades TP de dades fill, temp i copia de les del pare.
  // heap: alloc tantes pag de heap com tingui el pare
  int pag_heap = (int)programbreak_page - PAG_LOG_INIT_HEAP + 1;
  int allocatedpages[NUM_PAG_DATA + pag_heap]; //vector per poder tirar enrere en cas de ENOMEM
  for (i=0;i<NUM_PAG_DATA + pag_heap;++i){
    //alloquem una frame fisica i la guardem al vector de pagines alocades
    int pagenumber = alloc_frame();
    allocatedpages[i]=pagenumber;
    if (pagenumber<0) { //en cas d'error, fem la marcha atras.
      int j;
      for(j=0;j<i;++j) free_frame(allocatedpages[j]);
      return -ENOMEM;
    }
  }
  for (i=0;i<NUM_PAG_DATA + pag_heap;++i){
    int pagenumber = allocatedpages[i];
    //inicialitzem una entrada de dades del child mapejada a la nova frame
    // heap: en teoria tambe serveix aixi pel heap ja que sinicia just a continuació de les de dades
    set_ss_pag(child_tp,PAG_LOG_INIT_DATA+i,pagenumber); 
    //inicialitzem una entrada de dades del pare mapejada a la nova frame per poder escriure coses a la física   
    // heap: ha de ser a partir de les pag de heap, sino se les carrega
    set_ss_pag(father_tp,PAG_LOG_INIT_DATA+NUM_PAG_DATA + pag_heap +i,pagenumber);
    //el copy_data funciona amb @ lògiques (comencen al "0" per cada proces). el <<12 es pq nomes guardem un "tag" de la pag.física
    copy_data((void*)((PAG_LOG_INIT_DATA + i)<<12), (void*)((PAG_LOG_INIT_DATA+NUM_PAG_DATA + pag_heap +i)<<12), PAGE_SIZE); 
    //borrem les pàgines temporals que hem creat en la TP del pare
    del_ss_pag(father_tp,PAG_LOG_INIT_DATA+NUM_PAG_DATA + pag_heap +i);
  }

  //fem flush del TLB del pare
  set_cr3(get_DIR(current()));

  //Actualitzem el PID del fill
  PID = global_PID++;
  child_task->PID = PID;
  //Actualizem el kernel_esp del fill, basantnos en la transparencia nº52 del tema 4
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
  struct stats newstats = {0, 0, 0, 0, 0, 0, 0};
  child_task->p_stats = newstats; 
  //retornem el PID del fill
  return PID;
}
// TODO: sha de modificar pel sbrk
int sys_clone(void (*function)(void), void *stack){
  
  int PID=-1;
  if (list_empty(&freequeue)) return -EAGAIN;
  if ((int)function<L_USER_START || (int)function>= (PAG_LOG_INIT_DATA+NUM_PAG_DATA)*PAGE_SIZE) return -EINVAL;
  if ((int)stack < PAG_LOG_INIT_DATA*PAGE_SIZE || (int)stack >= (PAG_LOG_INIT_DATA+NUM_PAG_DATA)*PAGE_SIZE) return -EINVAL;

  struct list_head * h = list_first(&freequeue);
  list_del(h);
  struct task_struct* thread_task = list_head_to_task_struct(h);
  copy_data((void *)current(), (void *)thread_task, (int)sizeof(union task_union));

  dir_counter[current()->pos_in_dir_counter]++;

  PID = global_PID++;
  thread_task->PID = PID;
  unsigned int ebp_father = get_ebp();
  unsigned int offset = ebp_father - (unsigned int)current();
  unsigned int * stackpointer = (unsigned int *) ( (unsigned int)thread_task + offset);

  (*stackpointer) = (unsigned int)&ret_from_fork;
  stackpointer = stackpointer - 1;
  (*stackpointer) = 0;
  thread_task->kernel_esp = stackpointer;

  list_add_tail(&(thread_task->list), &readyqueue);
  thread_task->state = ST_READY;
  struct stats newstats = {0, 0, 0, 0, 0, 0, 0};
  thread_task->p_stats = newstats;

  *(((unsigned int *)thread_task)+KERNEL_STACK_SIZE-5) = function;
  *(((unsigned int *)thread_task)+KERNEL_STACK_SIZE-2) = stack;


  return PID;
}


struct semaphore{
  int counter;
  struct list_head blocked;
  int owner_pid;
};

extern struct semaphore sem_array[20];


int sys_sem_init (int n_sem, unsigned int value) {
  if (n_sem<0 || n_sem>=20) return -EINVAL;
  if (sem_array[n_sem].owner_pid!=-1) return -EBUSY;
  sem_array[n_sem].counter = value;
  INIT_LIST_HEAD(&sem_array[n_sem].blocked);
  sem_array[n_sem].owner_pid = current()->PID;
  return 0;
}

int sys_sem_wait (int n_sem) {
  if (n_sem<0 || n_sem>=20) return -EINVAL;
  if (sem_array[n_sem].owner_pid==-1) return -EINVAL;
  if (sem_array[n_sem].counter <= 0){
    current()->state = ST_BLOCKED;
    list_add_tail(&(current()->list), &(sem_array[n_sem].blocked));
    sched_next_rr();
  }else --sem_array[n_sem].counter;

  if (current()->semdestroyed){
    current()->semdestroyed=0;
    return -EINVAL;
  }
  return 0;
}

int sys_sem_signal (int n_sem) {
  if (n_sem<0 || n_sem>=20) return -EINVAL;
  if (sem_array[n_sem].owner_pid==-1) return -EINVAL;
  if (list_empty(&sem_array[n_sem].blocked)){
    ++sem_array[n_sem].counter;
  }else{
    struct list_head * h = list_first(&sem_array[n_sem].blocked);
    list_del(h);
    struct task_struct* thread_task = list_head_to_task_struct(h);
    thread_task->state = ST_READY;
    list_add_tail(&(thread_task->list), &readyqueue);
  }
  return 0;
}

int sys_sem_destroy (int n_sem) {
  if (n_sem<0 || n_sem>=20 || sem_array[n_sem].owner_pid==-1) return -EINVAL;
  if (sem_array[n_sem].owner_pid != current()->PID) return -EPERM;

  while(!list_empty(&sem_array[n_sem].blocked)){
    struct list_head * h = list_first(&sem_array[n_sem].blocked);
    list_del(h);
    struct task_struct* thread_task = list_head_to_task_struct(h);
    thread_task->semdestroyed = 1;
    thread_task->state = ST_READY;
    list_add_tail(&(thread_task->list), &readyqueue);
  }
  sem_array[n_sem].owner_pid=-1;
  return 0;


}

// TODO: sha de modificar pel sbrk
void sys_exit() {  
  
  for(int i=0; i<20; ++i){
    if(sem_array[i].owner_pid == current()->PID) sys_sem_destroy(i);
  }

  int pos = ((int)current()->dir_pages_baseAddr - (int)&dir_pages)>>12;
  --dir_counter[pos];
  if (!dir_counter[pos]){
    free_user_pages(current());
  }
  current()->PID=-1;
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

void enter_system(){
//a)
  current()->p_stats.user_ticks += get_ticks() - current()->p_stats.elapsed_total_ticks;
  current()->p_stats.elapsed_total_ticks = get_ticks();

}
void exit_system(){
//b)
 // unsigned long current_time = get_ticks();
  current()->p_stats.system_ticks += get_ticks() - current()->p_stats.elapsed_total_ticks;
  current()->p_stats.elapsed_total_ticks = get_ticks();
}

extern unsigned int left_ticks;
int sys_get_stats(int pid, struct stats *st){
   int i;
   char b = 0;
   if (st==NULL || !access_ok(VERIFY_READ,st,sizeof(struct stats))) return EFAULT;
   if (sizeof(*(st))!= sizeof(struct stats)) return EINVAL;
   if(pid<0) return EINVAL;
   for(i=0;i<NR_TASKS && b==0;++i){
      if (pid == task[i].task.PID) {
        if (current() == &task[i].task) task[i].task.p_stats.remaining_ticks = left_ticks;
        else task[i].task.p_stats.remaining_ticks = 0;
        *(st) = task[i].task.p_stats;
        b=1;
        return 0;
      }
   }
   return ESRCH;

}

int sys_read(int fd, char *buffer, int count){
  int error_fd = check_fd(fd, LECTURA);
  if (error_fd < 0) return -error_fd;
  if (buffer == NULL) return EFAULT;
  if (count < 0) return EINVAL;
 
  if ((int)buffer < PAG_LOG_INIT_DATA*PAGE_SIZE || (int)buffer >= (PAG_LOG_INIT_DATA+NUM_PAG_DATA)*PAGE_SIZE) return EFAULT;

  sys_read_keyboard(buffer,count);

  return 0;
}

extern struct list_head keyboardqueue;
extern int keyboard_readers_count[NR_TASKS];

void sys_read_keyboard(char * buffer, int count){
  keyboard_readers_count[((int)current()-(int)task)/sizeof(union task_union)] = count;
  if(!list_empty(&keyboardqueue)){
    update_process_state_rr (current(), &keyboardqueue);
    sched_next_rr();
    return;
  }

  int i;
  while (i < count) {
    if (!circbuff_empty()) {
      buffer[i] = circbuff_front();
      circbuff_pop();
      i++;
    }
    else {
      current()->state = ST_BLOCKED;
      list_add(&(current()->list), &keyboardqueue);
      sched_next_rr();
    }
  }
}


void *sys_sbrk(int increment){
    

    volatile int x = free_pages();


    unsigned char *endaddress = current()->programbreak + increment;
    if (increment>=0&&(((int)endaddress >> 12) < PAG_LOG_INIT_HEAP || ((int)endaddress >> 12) > TOTAL_PAGES)) return -ENOMEM;

    page_table_entry * current_tp = get_PT(current());
    unsigned int ini_programbreak_page = (unsigned int)current()->programbreak >> 12; //agafem la pagina d'inici
    unsigned int new_programbreak_page = (unsigned int)endaddress >> 12; //agafem pagina final
    char belowheap = 0;
    if(increment>0){
      volatile unsigned int npages = new_programbreak_page - ini_programbreak_page; 
      int tpentry = ini_programbreak_page + 1; //per saber quina entrada de la TP actualitzar. En principi la pàgina
                                              //on es troba l'inici del sbrk ja esta reservada, per aixo el +1.
      if(!((int)current()->programbreak & 0x0FFF)) { //si l'inici del sbrk és un inici de pàgina
        ++npages; //haurem de reservar una pagina mes
        tpentry--;// i per tant, desfem la suposicio de que ja estava reservada (el +1 anterior)
      }
      if(!((int)endaddress & 0x0FFF)) --npages; //en cas de que el final conicideixi amb una pagina nova, no cal reservarla
      int allocatedpages[npages];
      for (int i=0; i<npages; ++i){
        int pagenumber = alloc_frame();
        allocatedpages[i]=pagenumber;
        if (pagenumber<0) {
          int j;
          for(j=0;j<i;++j) free_frame(allocatedpages[j]);
          return -ENOMEM;
        }
        
      }
      for (int i=0; i<npages; ++i){
        set_ss_pag(current_tp,tpentry + i,allocatedpages[i]); 
      }

    }else if(increment<0){
      if (new_programbreak_page<PAG_LOG_INIT_HEAP){
        new_programbreak_page=PAG_LOG_INIT_HEAP;
        belowheap=1;
      } 
      volatile unsigned int npages = ini_programbreak_page - new_programbreak_page;
      int i;
      for (i = 0; i < npages; i++) {
        free_frame(get_frame(current_tp,ini_programbreak_page - i));
        del_ss_pag(current_tp,ini_programbreak_page - i);
      }

      if (!((int)endaddress & 0x0FFF)) {
        free_frame(get_frame(current_tp,new_programbreak_page));
        del_ss_pag(current_tp,new_programbreak_page);
      }
      set_cr3(get_DIR(current()));

    }
    //hem entes que cal retornar el valor de  programbreak anterior a l'execució de sbrk.
    char * last_programbreak = current()->programbreak;
    if(belowheap) current()->programbreak = PAG_LOG_INIT_HEAP<<12;
    else current()->programbreak = endaddress;
    return (void *)last_programbreak;

}




