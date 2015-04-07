#include "uthread.h"

//#include <ucontext.h>
/*pointer mangle function for decoding the value of a register and reassigining */

threads *threadList = NULL, *current = NULL;
static int size = 0;
static int nextalloc = 0;
ucontext_t kernel_thread;
long
pointer_mangle (void *p)
{
  long ret;
  asm volatile ("mov %1, %%rax; \n"
		"xor %%fs:0x30, %%rax;"
		"rol $0x11, %%rax;"
		"mov %%rax, %0;":"=r" (ret):"r" (p):"%rax");
  return ret;
}

int
deleteTnode (threads * tnode)
{
  threads *temp;
  if (tnode == NULL)
    {
      return -1;
    }
  temp = tnode->prev;
  temp->next = tnode->next;
  tnode->next->prev = temp;
  if (tnode->prev == NULL)
    {
      temp = tnode->next;
    }
  else if (tnode->next == NULL)
    {
      temp = tnode->prev;
    }
  free (tnode);
  tnode = temp;			//make sure to 
  return 1;
}				// logic to delete Thread node.
threads* findtcb(threads  *List, uthread_t tid){
threads *temp; //=(threads *)malloc(sizeof(threads));
temp=List;
while(temp!=NULL){
	
	if(temp->tid==tid){
		return temp;
	}
temp=temp->next;
}
return temp;
}
/*int add (threads *th, threads *head)	// adds th to the head of thread list
{
  head->next = malloc(sizeof(threads));
  head->next->prev = head;
  head=head->next;
  printf("in add with prev %ld head tid : %ld\n",th->prev->tid, threadList->tid);
  return head->prev->tid +1;
}
*/
/********************************************************************************
tid      : returned thread identification for the subsequent uses (e.g., uthread_join()) 
 start    : thread start function
 args     : argument passed to the start function
 priority : valid priority range: 0 ~ 100. 0 is the highest. 0 is reserved for main()
*****************************************************************************************/
void
kernelthread ()
{
printf("hi i am kernel thread meant for scheduling\n");
  return;
}
void *thread_wrapper(void *thread){
     ((threads *)thread)->th.ret_val=((threads *)thread)->th.func_ptr(((threads *)thread)->th.args);
}
int
uthread_create (uthread_t * tid, void *(*start) (void *), void *args,
		int priority)
{
 // void *tstart = start;
  ucontext_t *mycont;
  int temptid;
  mycont = (ucontext_t *) malloc (sizeof (ucontext_t));		//for saving uthread creat's context
  if (threadList == NULL)
    {
	printf("in firstime block --------------------------\n");
      threadList = (threads *) malloc (sizeof (threads));
      current = threadList;
      temptid = 1;
    }
  else
    {
      threads *th = (threads *) malloc (sizeof (threads));	// allocating the new node for 
      current->next=th;			//make th the new node
      th->prev=current;			// link th with the current head
	temptid=current->tid+1;
      current=current->next;				//change the head
  if (current == th)
    {
     printf("hurray !! \n");
    }

    }
  if (temptid == -1)
    {
      exit (-1);
    }
  current->ucp = (ucontext_t *) malloc (sizeof (ucontext_t));
  current->tid = temptid;
  *tid=temptid;
  current->th.args = args;		//for setting arguements
  current->th.priority = priority;	//setting priority.
  current->th.func_ptr = start;	//setting function pointer 
  if (getcontext (current->ucp) == -1)
    {
      fprintf (NULL, "error while creating thread\n", stderr);
      exit (-1);
    }				//get existing context
  current->ucp->uc_stack.ss_sp = malloc (8192);	// allocate 8K stack
  current->ucp->uc_stack.ss_size = 8192;	// save size of stack pointer
  current->ucp->uc_link = &kernel_thread;
  
  makecontext (current->ucp, (void *)thread_wrapper, 1, current);	//makes context for start function
  if (getcontext (&kernel_thread) == -1)
    {
      printf ("error while creating thread\n");
      exit (0);
    }				//get existing context

  kernel_thread.uc_stack.ss_sp = malloc (8192);	// allocate 8K stack
  kernel_thread.uc_stack.ss_size = 8192;	//sizeof(kernel_thread.uc_stack.ss_sp);             // save size of stack pointer
  kernel_thread.uc_link = mycont;
  makecontext (&kernel_thread, kernelthread, 0);	//makes context for start function
  swapcontext (mycont, current->ucp);
   free(mycont);
  return 0;
}

/********************************************************************************
 The calling thread blocks until the thread with tid exits (call uthread_exit())
 *value_ptr contains the passed value from uthread_exit()
*********************************************************************************/
int
uthread_join (uthread_t tid, void **value_ptr)
{
//wait until the thread finishes
 // printf("in join function of uthread \n");
  threads *thread;
//after it gets finished call uthread_exit();
thread=findtcb(threadList,tid);	// it will return the pointer to the found thread
	if(thread==NULL){
	printf("logic galat hai \n");
	exit(-1);
	}
//  printf(" in join %ld \n",(long)thread->th.ret_val);
  *value_ptr = (void *)thread->th.ret_val;	// will return the value from tcb block
  return 0;
}


/************************************************************************************
 This function should be implicitly called when a thread finishes its start function
 value_ptr receives the return value of the thread start function
*************************************************************************************/
void
uthread_exit (void *value_ptr)
{
}

/****************************************************************************
 the behavior will be similar to calling clock_gettime(CLOCK_MONOTONIC, &tp)
*****************************************************************************/
int
uthread_gettime (struct timespec *tp)
{
}

/********************************************************************************
 the behavior wil be similar to calling 
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &request, 0)
*********************************************************************************/
int
uthread_abstime_nanosleep (const struct timespec *request)
{
}

/****************************************************************
 similar to pthread_mutex_init()
 attr is either UTHREAD_MUTEX_ATTR_NONE or UTHREAD_MUTEX_ATTR_PI
*****************************************************************/
int
uthread_mutex_init (uthread_mutex_t * mutex, const int attr)
{
}

/****************************************************************
 lock/unlock "mutex"
*****************************************************************/
int
uthread_mutex_lock (uthread_mutex_t * mutex)
{
}

int
uthread_mutex_unlock (uthread_mutex_t * mutex)
{
}

/**********************************************************************************
 enable stack-based priority ceiling protocol
 this function should be called in main() after creating all threads and mutexes  
 mutex attributes defined previously are ignore by calling this function
	 resource_table: array of resources that are used in the program
   n: number of resources in the table
***********************************************************************************/
int
uthread_srp_enable (resource_entry_t * resource_table, int n)
{

}
