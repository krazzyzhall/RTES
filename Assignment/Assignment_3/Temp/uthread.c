#include "uthread.h"

//#include <ucontext.h>
/*pointer mangle function for decoding the value of a register and reassigining */
int deleteTnode(threads *tnode){
	threads *temp;
	if(tnode==NULL){
	return -1;
	}
	temp=tnode->prev;
	temp->next=tnode->next;
	tnode->next->prev=temp;
	if(tnode->prev==NULL){
	temp=tnode->next;
	}
	else if(tnode->next==NULL){
	temp=tnode->prev;
	}
	free(tnode);
	tnode=temp;		//make sure to 
	return 1;
}	// logic to delete Thread node.
int add(threads *th, threads *head){
    head->next=th;
    th->prev=head;
    th->next=NULL;
    return th->prev->tid + 1;
}
long pointer_mangle(void* p)
{
 long ret;
 asm volatile(
 "mov %1, %%rax; \n"
 "xor %%fs:0x30, %%rax;"
 "rol $0x11, %%rax;"
 "mov %%rax, %0;"
 : "=r" (ret)
 : "r" (p)
 : "%rax"
 );
 return ret;
}
threads *threadList=NULL,*current=NULL;
static int size=0;
static int nextalloc=0;
ucontext_t kernel_thread;
/********************************************************************************
tid      : returned thread identification for the subsequent uses (e.g., uthread_join()) 
 start    : thread start function
 args     : argument passed to the start function
 priority : valid priority range: 0 ~ 100. 0 is the highest. 0 is reserved for main()
*****************************************************************************************/
void kernelthread(){
//printf("hi i am kernel thread\n");
return;
}
int uthread_create(uthread_t *tid, void *(*start)(void *), void *args, int priority){
ucontext_t mycont;
void *tstart=start;
//add new Thread node
int temptid;
threads *th=(threads *)malloc( sizeof(threads) );	// allocating the new node for 
if(threadList==NULL){
threadList=th;
current=th;
temptid=1;
}else{
temptid=add(th,current);				// add this new node to the thread list 
}
if(temptid==-1){
	exit(-1);
	}//*tid=temptid;
th->tid=temptid;
th->th.args=args;	//for setting arguements
th->th.priority=priority;	//setting priority.
th->th.func_ptr=tstart;	//setting function pointer 
th->ucp=malloc(sizeof(ucontext_t));
if(getcontext(th->ucp)==-1){
//  printf("error while creating thread\n");
  exit(-1);
}			//get existing context
/*if(getcontext(&kernel_thread)==-1){
//  printf("error while creating thread\n");
  exit(-1);
}*/			//get existing context
th->ucp->uc_stack.ss_sp=malloc(8192);		// allocate 8K stack
th->ucp->uc_stack.ss_size=sizeof(th->ucp->uc_stack.ss_sp);		// save size of stack pointer
//th->ucp->uc_link = &kernel_thread;
th->ucp->uc_link = &mycont;
makecontext(th->ucp,tstart,1,args);	//makes context for start function
//printf("context made now swaping context\n");
/*if(getcontext(&kernel_thread)==-1){
  printf("error while creating thread\n");
  exit(0);
}			//get existing context
//kernel_thread.uc_stack.ss_sp=malloc(8192);		// allocate 8K stack
//kernel_thread.uc_stack.ss_size=sizeof(th->ucp->uc_stack.ss_sp);		// save size of stack pointer
//kernel_thread.uc_link = &mycont;
//makecontext(&kernel_thread,kernelthread,0);	//makes context for start function
*/swapcontext(&mycont,th->ucp);
printf("creating the thread was complete\n");
return 0;
}

/********************************************************************************
 The calling thread blocks until the thread with tid exits (call uthread_exit())
 *value_ptr contains the passed value from uthread_exit()
*********************************************************************************/
int uthread_join(uthread_t tid, void **value_ptr){
	*value_ptr=(void *)10;
	return 0;
}


/************************************************************************************
 This function should be implicitly called when a thread finishes its start function
 value_ptr receives the return value of the thread start function
*************************************************************************************/
void uthread_exit(void *value_ptr){
}

/****************************************************************************
 the behavior will be similar to calling clock_gettime(CLOCK_MONOTONIC, &tp)
*****************************************************************************/
int uthread_gettime(struct timespec *tp){
}

/********************************************************************************
 the behavior wil be similar to calling 
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &request, 0)
*********************************************************************************/
int uthread_abstime_nanosleep(const struct timespec *request){
}

/****************************************************************
 similar to pthread_mutex_init()
 attr is either UTHREAD_MUTEX_ATTR_NONE or UTHREAD_MUTEX_ATTR_PI
*****************************************************************/
int uthread_mutex_init(uthread_mutex_t *mutex, const int attr){
}

/****************************************************************
 lock/unlock "mutex"
*****************************************************************/
int uthread_mutex_lock(uthread_mutex_t *mutex){
}
int uthread_mutex_unlock(uthread_mutex_t *mutex){
}

/**********************************************************************************
 enable stack-based priority ceiling protocol
 this function should be called in main() after creating all threads and mutexes  
 mutex attributes defined previously are ignore by calling this function
	 resource_table: array of resources that are used in the program
   n: number of resources in the table
***********************************************************************************/
int uthread_srp_enable(resource_entry_t *resource_table, int n){

}
