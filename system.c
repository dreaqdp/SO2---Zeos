/*
 * system.c - 
 */

#include <segment.h>
#include <types.h>
#include <interrupt.h>
#include <hardware.h>
#include <system.h>
#include <sched.h>
#include <mm.h>
#include <io.h>
#include <utils.h>
#include <zeos_mm.h> /* TO BE DELETED WHEN ADDED THE PROCESS MANAGEMENT CODE TO BECOME MULTIPROCESS */
#include <circbuff.h>


int (*usr_main)(void) = (void *) PH_USER_START;
unsigned int *p_sys_size = (unsigned int *) KERNEL_START;
unsigned int *p_usr_size = (unsigned int *) KERNEL_START+1;
unsigned int *p_rdtr = (unsigned int *) KERNEL_START+2;

unsigned int zeos_ticks;
extern int global_PID;
/************************/
/** Auxiliar functions **/
/************************/
/**************************
 ** setSegmentRegisters ***
 **************************
 * Set properly all the registers, used
 * at initialization code.
 *   DS, ES, FS, GS <- DS
 *   SS:ESP <- DS:DATA_SEGMENT_SIZE
 *         (the stacks grows towards 0)
 *
 * cld -> gcc2 wants DF (Direction Flag (eFlags.df))
 *        always clear.
 */

/*
 * This function MUST be 'inline' because it modifies the %esp 
 */
inline void set_seg_regs(Word data_sel, Word stack_sel, DWord esp)
{
      esp = esp - 5*sizeof(DWord); /* To avoid overwriting task 1 */
	  __asm__ __volatile__(
		"cld\n\t"
		"mov %0,%%ds\n\t"
		"mov %0,%%es\n\t"
		"mov %0,%%fs\n\t"
		"mov %0,%%gs\n\t"
		"mov %1,%%ss\n\t"
		"mov %2,%%esp"
		: /* no output */
		: "r" (data_sel), "r" (stack_sel), "g" (esp) );

}
//nostre
void writeMSR(int msr_id, int msr_value); 
void system_call_handler();
void setMSR(){
	writeMSR(0x174,__KERNEL_CS);
	writeMSR(0x175,INITIAL_ESP);
	writeMSR(0x176,(int)system_call_handler);
} //end nostre

/*
 *   Main entry point to ZEOS Operating System
 */

void init_circbuff(){
  circbuffptr_last = &keybuffer; 
  circbuffptr_first = &keybuffer;
  circbuffcounter = 0;
}


extern union task_union *task; //pointer to task[1] (0 is protected)
 
struct list_head freequeue; 
 
void init_freequeue(){
   
   INIT_LIST_HEAD(&freequeue);
   int i;
   for(i=0;i<NR_TASKS;++i){
    task[i].task.PID=-1;
    list_add_tail(&(task[i].task.list), &freequeue);
   }
/*
   struct list_head * e;
   list_for_each( e, &freequeue ) {
      printk("\n");
      printnum((int)e->prev);
      printk(" ");
      printnum((int)e->next);
  }*/
}

extern struct list_head keyboardqueue;
struct list_head readyqueue; 

void init_queue(struct list_head *queue){
	INIT_LIST_HEAD(queue);
}


//extern struct semaphore;


struct semaphore{
  int counter;
  struct list_head blocked;
  int owner_pid;
};
struct semaphore sem_array[20];

void init_semaphores(){
  for(int i=0; i<20; ++i){
    sem_array[i].counter = 0;
    sem_array[i].owner_pid = -1;
    INIT_LIST_HEAD(&sem_array[i].blocked);
  }
}



int __attribute__((__section__(".text.main")))
  main(void)
{

  set_eflags();

  /* Define the kernel segment registers  and a stack to execute the 'main' code */
  // It is necessary to use a global static array for the stack, because the
  // compiler will know its final memory location. Otherwise it will try to use the
  // 'ds' register to access the address... but we are not ready for that yet
  // (we are still in real mode).
  set_seg_regs(__KERNEL_DS, __KERNEL_DS, (DWord) &protected_tasks[5]);

  /*** DO *NOT* ADD ANY CODE IN THIS ROUTINE BEFORE THIS POINT ***/
  printk("Kernel Loaded!    ");


  /* Initialize hardware data */
  setGdt(); /* Definicio de la taula de segments de memoria */
  setIdt(); /* Definicio del vector de interrupcions */
  setTSS(); /* Definicio de la TSS */
  setMSR();
  /* Initialize Memory */
  init_mm();
  

/* Initialize an address space to be used for the monoprocess version of ZeOS */

  //monoprocess_init_addr_space(); /* TO BE DELETED WHEN ADDED THE PROCESS MANAGEMENT CODE TO BECOME MULTIPROCESS */

  /* process queues */
  init_freequeue();
  init_queue(&readyqueue);
  init_queue(&keyboardqueue);

  /* init circular buffer */
  init_circbuff();
  /* init semaphores */

  init_semaphores();

  // initialize global PID
  global_PID = 2;
  /* Initialize idle task  data */
  init_idle();
  /* Initialize task 1 data */
  init_task1();
  /* Initialize Scheduling */
  init_sched(); 
  /* Move user code/data now (after the page table initialization) */
  copy_data((void *) KERNEL_START + *p_sys_size, usr_main, *p_usr_size);

  zeos_ticks = 0;
  printk("Entering user mode...");

  zeos_init_auxjp();
  enable_int();
  /*
   * We return from a 'theorical' call to a 'call gate' to reduce our privileges
   * and going to execute 'magically' at 'usr_main'...
   */
   
  return_gate(__USER_DS, __USER_DS, USER_ESP, __USER_CS, L_USER_START);

  /* The execution never arrives to this point */
  return 0;
}


void printnum(int n){
  char buff[10] = "         ";
  itoa(n,buff);
  printk(buff);
}
/*
int size(void* p){
  return sizeof(*p)/sizeof((p[0]));
}*/


