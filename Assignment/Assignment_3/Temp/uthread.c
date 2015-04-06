#include "uthread.h"

//#include <ucontext.h>
/*pointer mangle function for decoding the value of a register and reassigining */
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
threads *th;
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
printf("hi i am kernel thread\n");
return;
}
int uthread_create(uthread_t *tid, void *(*start)(void *), void *args, int priority){
ucontext_t mycont;
void *tstart=start;
int temptid;
if(nextalloc==size){
temptid=size++;	nextalloc++; //if all the spawned threads are still active
}
else {
temptid=size++;	nextalloc++; //if all the spawned threads are still active
}
printf("before relloc %lu \n",size*sizeof(threads));
threads *tth=malloc((size)*sizeof(threads));	// allocate the array or threads for tracking
if(size!=0){
memcpy(tth,th,(size-1)*sizeof(threads));//threads *tth=realloc(th,(size)*sizeof(threads));	// allocate the array or threads for tracking
}else{
th=tth;}printf("ee kala kawa kaat khayega sach bol\n");
if(tth==NULL){
printf("ee kala kawa kaat khayega sach bol\n");
exit(-1);
}else{
th=tth;
}
*tid=temptid;
th[nextalloc-1].tid=temptid;
th[nextalloc-1].th.args=args;	//for setting arguements
th[nextalloc-1].th.priority=priority;	//setting priority.
th[nextalloc-1].th.func_ptr=tstart;	//setting function pointer 
th[nextalloc-1].ucp=malloc(sizeof(ucontext_t));
if(getcontext(th[nextalloc-1].ucp)==-1){
  printf("error while creating thread\n");
  exit(0);
}			//get existing context
if(getcontext(&kernel_thread)==-1){
  printf("error while creating thread\n");
  exit(0);
}			//get existing context
th[nextalloc-1].ucp->uc_stack.ss_sp=malloc(8192);		// allocate 8K stack
th[nextalloc-1].ucp->uc_stack.ss_size=sizeof(th[nextalloc-1].ucp->uc_stack.ss_sp);		// save size of stack pointer
th[nextalloc-1].ucp->uc_link = &kernel_thread;
th[nextalloc-1].ucp->uc_link = &mycont;
makecontext(th[nextalloc-1].ucp,tstart,1,args);	//makes context for start function
printf("context made now swaping context\n");
if(getcontext(&kernel_thread)==-1){
  printf("error while creating thread\n");
  exit(0);
}			//get existing context
kernel_thread.uc_stack.ss_sp=malloc(8192);		// allocate 8K stack
kernel_thread.uc_stack.ss_size=sizeof(th[nextalloc-1].ucp->uc_stack.ss_sp);		// save size of stack pointer
kernel_thread.uc_link = &mycont;
makecontext(&kernel_thread,kernelthread,0);	//makes context for start function
swapcontext(&mycont,th[nextalloc-1].ucp);
}

/********************************************************************************
 The calling thread blocks until the thread with tid exits (call uthread_exit())
 *value_ptr contains the passed value from uthread_exit()
*********************************************************************************/
int uthread_join(uthread_t tid, void **value_ptr){
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
