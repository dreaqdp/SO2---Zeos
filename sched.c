/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>


int global_PID;
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



extern int dir_counter[NR_TASKS];
int allocate_DIR(struct task_struct *t) 
{
	for(int i=0; i<NR_TASKS; ++i){
		if(!dir_counter[i]){
			dir_counter[i]=1;
			t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[i]; 
			t->pos_in_dir_counter=i;
			return 1;
		}
	}

/*
	int pos;
	pos = ((int)t-(int)task)/sizeof(union task_union);
	dir_counter[pos]=1;
	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 
*/
	return 1;
}
extern struct task_struct * task1;

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1)
	{
		//printk("soc el idle");
	}
}

extern struct list_head freequeue;
struct task_struct * idle_task;
struct task_struct * task1; //si no la utilitzem no fer task1 global
void init_idle (void)
{
	struct list_head *h = list_first(&freequeue);
	list_del(h);
	idle_task = list_head_to_task_struct(h);
	idle_task->PID = 0;
	idle_task->semdestroyed = 0;
	set_quantum(idle_task,50);
	idle_task->p_stats.remaining_ticks = 50;
	allocate_DIR(idle_task);
	idle_task->kernel_esp = ((unsigned int *)idle_task)+KERNEL_STACK_SIZE-1;
	(*(idle_task->kernel_esp)) = (unsigned int)&cpu_idle;
	idle_task->kernel_esp = (idle_task->kernel_esp)-1;
	(*(idle_task->kernel_esp)) = 0;  //ebp=0 a l'stack

}
void writeMSR(int msr_id, int msr_value);
void init_task1(void)
{
	struct list_head *h = list_first(&freequeue);
	list_del(h);
	task1 = list_head_to_task_struct(h);
	task1->programbreak = (unsigned char *)(PAG_LOG_INIT_HEAP << 12);
	task1->PID = 1;
	set_quantum(task1,50);
	task1->p_stats.remaining_ticks = 50;
	task1->state = ST_RUN;
	task1->semdestroyed = 0;
	allocate_DIR(task1);
	set_user_pages(task1);
	tss.esp0 = (DWord)(((unsigned int *)task1)+KERNEL_STACK_SIZE);
	writeMSR(0x175,(int)tss.esp0);
	set_cr3(task1->dir_pages_baseAddr);
}

extern unsigned int get_ticks();
unsigned int left_ticks;
void init_sched(){
	left_ticks = get_quantum(task1);
	//ticks_process = task1->quantum;
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


int get_quantum (struct task_struct *t){
	return t->quantum;
}
void set_quantum(struct task_struct *t, int new_quantum){
	t->quantum = new_quantum;
}

void update_sched_data_rr (void){
	//--current()->p_stats.remaining_ticks;
	--left_ticks;
}
//returns 1 if it's necessary to change the current process
int needs_sched_rr (void){
	if (left_ticks == 0){
		current()->p_stats.total_trans = current()->p_stats.total_trans + 1;
		return 1;
	}
	return 0;
}

extern struct list_head readyqueue;
void update_process_state_rr (struct task_struct * t, struct list_head * dst_queue){
    if (t->state!=ST_RUN) list_del(&(t->list));
    if (dst_queue!=NULL) list_add_tail(&(t->list), dst_queue);

	if (dst_queue==&readyqueue){
		t->state=ST_READY;
	}else if (dst_queue==NULL){
		t->state=ST_RUN;
	}else{
		t->state=ST_BLOCKED; 
	}
	

}
void task_switch(union task_union * t);
extern void printnum(int n);

void sched_next_rr (void){
	struct task_struct * next;
	if (!list_empty(&readyqueue)) {
		struct list_head * h = list_first(&readyqueue); 
		next = list_head_to_task_struct(h); //agafem el primer de la ready
	}
	else {
		next = idle_task;
		task_switch((union task_union *)next);
		return;
	}
	update_process_state_rr(next, NULL); //treiem el nou de sa cua(segurament ready) i li canviem l'state

	// stats

	current()->p_stats.system_ticks += get_ticks()- current()->p_stats.elapsed_total_ticks;
	current()->p_stats.elapsed_total_ticks = get_ticks();

	//next->p_stats.remaining_ticks = get_quantum(next);
	left_ticks = get_quantum(next);
	task_switch((union task_union *)next);
	current()->p_stats.total_trans = current()->p_stats.total_trans + 1;
	//current()->p_stats.remaining_ticks = get_quantum(current()); //provar treurel

	current()->p_stats.ready_ticks += get_ticks() - current()->p_stats.elapsed_total_ticks;
	current()->p_stats.elapsed_total_ticks = get_ticks();
}

void schedule(){
	update_sched_data_rr();
	if(needs_sched_rr()){ //quantum over
		if (current() != idle_task) update_process_state_rr(current(),&readyqueue); //poses el current a la ready
		sched_next_rr(); //canviar a la nova
	}
}