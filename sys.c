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
  return 0;
}

int sys_fork(){
  int PID=-1;
  if (list_empty(list_first(&freequeue))) return -EAGAIN;
  //struct task_struct* child_task = list_head_to_task_struct(list_first(&freequeue));
  child_task = list_head_to_task_struct(list_first(&freequeue));
  list_del(list_first(&freequeue));  
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
  //encuem el fill a la readyqueue
  list_add(&(child_task->list), &readyqueue);
  //Actualitzem el PID del fill
  PID = global_PID++;
  child_task->PID = PID;
  //Actualizem el kernel_esp del fill, basantme en la transparencia nº52 del tema 4
  //crec que lo de -18 i -19 era si no miraves que a les transpes posa que utilitzis el ebp i no el esp
  int test=0;
  if(!test){ //ENS VEIEM FORÇATS A PASSARLO PER AQUI..SI HO FEM POSANTO A EAX LA FUNCIO DE ENTRY.S  SYSTEM_CALL_HANDLER ENS SOBREESCRIU EL EAX DE LA PILA...
              //POTSER PODRIEM APROFITAR UN ALTRE DELS DEL SAVE_ALL...HAURIA DE CANVIARS EL DEL +8
    unsigned int ebp_father = get_ebp();
    unsigned int offset = ebp_father - (unsigned int)current();
    unsigned int * stackpointer = (unsigned int *) ( (unsigned int)child_task + offset);
    (*stackpointer) = (unsigned int)&ret_from_fork;
    stackpointer = stackpointer - 1;
    (*stackpointer) = 0;
    child_task->kernel_esp = stackpointer;
  }else{ //per l'Andrea -> es el meu intent de evitar el ret from fork..al comentari superior diu pq no funciona
    unsigned int ebp_father = get_ebp();
    unsigned int offset = ebp_father - (unsigned int)current();
    unsigned int * stackpointer = (unsigned int *) ( (unsigned int)child_task + offset);
    (*(stackpointer+8)) = 0;
    child_task->kernel_esp = stackpointer;
  }
  //encuem el fill a la readyqueue
  list_add_tail(&(child_task->list), &readyqueue);
  //retornem el PID del fill
  return PID;
}


int sys_fork_old()
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
    // creem temp pel pare    //si peta ens mirem aixo pq hem confiat massa :D
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






