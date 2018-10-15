/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

/**
 * Container for the Task array and 2 additional pages (the first and the last one)
 * to protect against out of bound accesses.
 */
union task_union protected_tasks[NR_TASKS+2]
  __attribute__((__section__(".data.task")));

union task_union *task = &protected_tasks[1]; /* == union task_union task[NR_TASKS] */

//#if 0
struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
}
//#endif

extern struct list_head blocked;


/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}
extern struct task_struct * task1;

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");
	
	/*char buf[2];
	//itoa(sys_getpid(),buf);
	//printk("hey");
	int i;
	for(i=0;i<100000;++i){
		printk("coses boniques hola funciono");
	}

	task_switch(task1);*/

	while(1)
	{
		printk("maincrah");
	}
}

extern struct list_head freequeue;
struct task_struct * idle_task;
struct task_struct * task1; //si no la utilitzem no fer task1 global
void init_idle (void)
{
	idle_task = list_head_to_task_struct(list_first(&freequeue));
	list_del(list_first(&freequeue));
	idle_task->PID = 0;
	allocate_DIR(idle_task);
	idle_task->kernel_esp = ((unsigned int *)idle_task)+KERNEL_STACK_SIZE-1;
	(*(idle_task->kernel_esp)) = (unsigned int)&cpu_idle;
	idle_task->kernel_esp = (idle_task->kernel_esp)-1;
	(*(idle_task->kernel_esp)) = 0;  //ebp=0 a l'stack

}
void writeMSR(int msr_id, int msr_value);
void init_task1(void)
{
	task1 = list_head_to_task_struct(list_first(&freequeue));
	list_del(list_first(&freequeue));
	task1->PID = 1;
	allocate_DIR(task1);
	set_user_pages(task1);
	tss.esp0 = (DWord)(((unsigned int *)task1)+KERNEL_STACK_SIZE);
	writeMSR(0x175,(int)tss.esp0);
	set_cr3(task1->dir_pages_baseAddr);
	
}


void init_sched(){

}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}

