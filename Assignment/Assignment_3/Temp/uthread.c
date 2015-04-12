#include "uthread.h"
#define CLOCKID CLOCK_MONOTONIC
#define SIG SIGRTMIN
//#include <ucontext.h>
/*pointer mangle function for decoding the value of a register and reassigining */
/*write a function to rearrange priority queue*/
/*write a function to add a thread in a priority queue */
/*write a function to delete a thread in a priority queue */
threads *threadList = NULL, *current = NULL;
static int size = 0;
static int nextalloc = 0;
ucontext_t kernel_thread;	// makes context for kernel thread
static ucontext_t main_thread;	// for saving context for main thread
int main_over = 0,sleep_curr_task=0,wakeup=0;
void kernelthread(void);
threads *find_timer(timer_t timerid);
int get_running(threads **thread){
int i;
threads *temp;
for (i=0;i<100;i++){
	  if (priority[i] == NULL)
	    {
	      continue;
	    }
temp=priority[i];
	while(temp!=NULL){
		if (temp->th.state==RUNNING){
//			printf("kuch hua %d , temp id = %ld\n",i,temp->tid);
			*thread=temp;	
			return 0;		
		}
		temp=temp->next;
	}
}
return -1;
}
threads *find_timer(timer_t timerid){
int i;
//printf("balam pichkaari \n");
threads *temp;
for (i=0;i<100;i++){
	  if (priority[i] == NULL)
	    {
//	printf("get_running  main hi atak gayi %d \n",i);
	      continue;
	    }
temp=priority[i];
	while(temp!=NULL){
		if (temp->timerid==timerid){
//			printf("kuch hua %d , temp id = %ld\n",i,temp->tid);
			return temp;			
		}
		temp=temp->next;
	}
}
return NULL;
}
/*long
pointer_mangle (void *p)
{
  long ret;
  asm volatile ("mov %1, %%rax; \n"
		"xor %%fs:0x30, %%rax;"
		"rol $0x11, %%rax;"
		"mov %%rax, %0;":"=r" (ret):"r" (p):"%rax");
  return ret;
}
*/
       static void
       print_siginfo(siginfo_t *si)
       {
           timer_t *tidp;
           int or;

           tidp = si->si_value.sival_ptr;

           printf("    sival_ptr = %p; ", si->si_value.sival_ptr);
           printf("    *sival_ptr = 0x%lx\n", (long) *tidp);
	   threads *thread;//=malloc(sizeof(threads));
           thread=find_timer(*tidp);		// get the timerid and look for the thread that was waiting for this timer
           if(thread == NULL){
              printf("this is to return to kernel thread \n"); 
              return;
              }
           thread->th.state=READY;			//update this thread to ready state as it is ready to get executed
       }

       static void
       handler(int sig, siginfo_t *si, void *uc)
       {
sigset_t mask;
           /* Note: calling printf() from a signal handler is not
              strictly correct, since printf() is not async-signal-safe;
              see signal(7) */
//    printf ("main ki maa ka \n");
           sigemptyset(&mask);
          sigaddset(&mask, SIG);
           if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
               printf("sigprocmask");

  	   ucontext_t *mycont;
           mycont = (ucontext_t *) malloc (sizeof (ucontext_t));	//for saving uthread creat's context

//           printf("Caught signal %d\n", sig);
           print_siginfo(si);
	   sleep_curr_task=0;
// if (getcontext (&kernel_thread) == -1)
//    {
//      printf ("error while creating thread\n");
//      exit (0);
//    }				//get existing context
//  kernel_thread.uc_stack.ss_sp = malloc (8192);	// allocate 8K stack
//  kernel_thread.uc_stack.ss_size = 8192;	//sizeof(kernel_thread.uc_stack.ss_sp);             // save size of stack pointer
//  kernel_thread.uc_link = NULL;
  makecontext (&kernel_thread, kernelthread, 0);	//makes context for start function

//	printf("thread is set to ready state : %d \n",44);
if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1)
              printf("sigprocmask");
//    printf ("main ki maa ka \n");
  setcontext (&kernel_thread);	//for getting to kernel thread to schedule
       }

int
deleteTnode (threads * tnode, threads * pqueue)
{
  threads *temp;
  if (tnode == NULL)
    {
      return -1;
    }
  if (tnode->prev == NULL && tnode->next != NULL)
    {
      pqueue = tnode->next;
      free (tnode);
      return 1;
    }
  else if (tnode->next == NULL && tnode->prev != NULL)	//its the tail node 
    {
      free (tnode);
      return 1;
    }
  else if (tnode->prev != NULL && tnode->next != NULL)
    {
      tnode->prev->next = tnode->next;
      tnode->next->prev = tnode->prev;
    }
  free (tnode);
  return 1;
}				// logic to delete Thread node.

threads *
findtcb (uthread_t tid)
{

  threads *temp;		//=(threads *)malloc(sizeof(threads));
  temp = (threads *) tid;
  return temp;

}				/*
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
				   } */
threads *
gettail (int prio)		// adds th to the head of thread list
{
  threads *temp;
  threads *pqueue = priority[prio];	//gets the queue for prio priority
  if (pqueue == NULL)
    {
      return NULL;
    }
  else
    {
      temp = pqueue;
    }
  while (temp->next != NULL)
    {		temp=temp->next;		//till it reaches its end
    }
  return temp;			//will return the tail of this priority queue
//  printf("in add with prev %ld head tid : %ld\n",th->prev->tid, threadList->tid);
//  return head->prev->tid +1;
}

/********************************************************************************
tid      : returned thread identification for the subsequent uses (e.g., uthread_join()) 
 start    : thread start function
 args     : argument passed to the start function
 priority : valid priority range: 0 ~ 100. 0 is the highest. 0 is reserved for main()
*****************************************************************************************/
void
kernelthread (){				/* sort the priority array properly */
//printf("entered kernel thread \n");
  int i;
  struct itimerspec min;
  threads *temp;
  timer_t timerid;
  struct sigevent sev;
  struct itimerspec its;
  long long freq_nanosecs;
  sigset_t mask;
  ucontext_t *mycont;
  mycont = (ucontext_t *) malloc (sizeof (ucontext_t));	//for saving uthread creat's context

  if (main_over == 0)
    {
      printf ("main ki maa ka \n");
    }
  else
    {
min.it_value.tv_sec=0;
min.it_value.tv_nsec=0;
      for (i = 0; i < 100; i++)
	{
	  if (priority[i] == NULL)
	    {
//	printf("kernel thread priority to while main hi atak gayi %d \n",i);
	      continue;
	    }
	temp = priority[i];
	  while (temp != NULL)
	    { 
//              printf("kernel thread priority to while main hi atak gayi\n");
	      if (temp->th.state == READY){
		temp->th.state = RUNNING;
//              printf("kernel thread priority : %d \n",temp->th.priority);
		  setcontext (temp->ucp);	//switch to thread
//              printf("after returning from thread exit status is : %d",temp->th.state);
		}
	      temp = temp->next;
	    }
	}
}
	sleep(1);
//printf("exiting kernel thread \n");
  return;
}

void *
thread_wrapper (void *thread)
{				/* this function needs to be called for the thread */
ucontext_t temp;
//((threads *) thread)->th.state = RUNNING;
  ((threads *) thread)->th.ret_val =
    ((threads *) thread)->th.func_ptr (((threads *) thread)->th.args);
  ((threads *) thread)->th.state = EXITED;
//printf("after exit \n");
/*  if (getcontext (&temp) == -1)
    {
      exit (-1);
    }				//get existing context
  temp.uc_stack.ss_sp = malloc (8192);	// allocate 8K stack
  temp.uc_stack.ss_size = 8192;	// save size of stack pointer
  temp.uc_link = &kernel_thread;
  makecontext (&temp, (void *) uthread_exit, 1, );	//makes context for start function
*/}

int
uthread_create (uthread_t * tid, void *(*start) (void *), void *args,
		int prio)
{
//printf("entered create_thread \n");
  // void *tstart = start;
  ucontext_t *mycont;
  int temptid;
  mycont = (ucontext_t *) malloc (sizeof (ucontext_t));	//for saving uthread creat's context
  threadList = gettail (prio);	//will return pointer to the priority queue's tail
  if (threadList == NULL)	//that means this is the first task for this priority therefore allocate at priority[priority]
    {
      printf ("in firstime block -----------------for %d\n", prio);
      threadList = (threads *) malloc (sizeof (threads));	// allocate space for new node
      priority[prio] = threadList;	// save reference in  priority queue
      temptid = (uthread_t) threadList;	// make tid as address
      current = threadList;
//      printf("saved reference in priority thread \n");
    }
  else
    {
      threads *th = (threads *) malloc (sizeof (threads));	// allocating the new node for 
//      threadList->next = th;	//make th the new node as the tail
  //    th->prev = threadList;	// link th with the current head
      temptid = (uthread_t) th;
      current = th;		//change the head
      priority[prio] = current;
    }
  if (temptid == -1)
    {
      exit (-1);
    }
  current->ucp = (ucontext_t *) malloc (sizeof (ucontext_t));
  current->tid = temptid;
  *tid = temptid;
  current->th.args = args;	//for setting arguements
  current->th.state = READY;	//initial state should be ready
  current->th.priority = prio;	//setting priority.
  current->th.func_ptr = start;	//setting function pointer 
  if (getcontext (current->ucp) == -1)
    {
      exit (-1);
    }				//get existing context
  current->ucp->uc_stack.ss_sp = malloc (8192);	// allocate 8K stack
  current->ucp->uc_stack.ss_size = 8192;	// save size of stack pointer
  current->ucp->uc_link = &kernel_thread;
  /* for making context for the thread function current context will be for thread we will send this to a priority */
  makecontext (current->ucp, (void *) thread_wrapper, 1, current);	//makes context for start function
/* registration is done */
//           printf("Establishing handler for signal %d\n", SIG);
           current->sa.sa_flags =SA_SIGINFO;//|SA_RESTART;
           current->sa.sa_sigaction = handler;
           //sigemptyset(&sa.sa_mask);
           if (sigaction(SIG, &(current->sa), NULL) == -1)
               printf("sigaction");
//printf("get_running ke aage atak raha hai\n");
            current->sev.sigev_notify = SIGEV_SIGNAL;
           current->sev.sigev_signo = SIG;
           current->sev.sigev_value.sival_ptr = &(current->timerid);
           if (timer_create(CLOCK_MONOTONIC, &(current->sev), &(current->timerid)) == -1)
               printf("timer_create");
  main_over = 0;		// to show that we need to go back to main from kernel thread
  free (mycont);
//printf("exiting create_thread pthread[prio] %ld\n",priority[prio]->tid);
  return 0;
}

/********************************************************************************
 The calling thread blocks until the thread with tid exits (call uthread_exit())
 *value_ptr contains the passed value from uthread_exit()
*********************************************************************************/
int
uthread_join (uthread_t tid, void **value_ptr)
{
printf("entered  join_thread \n");
  threads *thread;
  ucontext_t *mycont;
  mycont = (ucontext_t *) malloc (sizeof (ucontext_t));	//for saving uthread creat's context
  main_over = 1;		// will signal kernel to indicate that main is done and 
  //wait until the thread finishes
  if (getcontext (&kernel_thread) == -1)
    {
      printf ("error while creating thread\n");
      exit (0);
    }	
thread=findtcb(tid);			//get existing context

  while (thread->th.state != EXITED)
    {
//printf("entered while: join_thread %ld,thread state : %d\n",thread->tid,thread->th.state);
      kernel_thread.uc_stack.ss_sp = malloc (8192);	// allocate 8K stack
      kernel_thread.uc_stack.ss_size = 8192;	//sizeof(kernel_thread.uc_stack.ss_sp);             // save size of stack pointer
      kernel_thread.uc_link = mycont;
      makecontext (&kernel_thread, kernelthread, 0);	//makes context for start function
      swapcontext (mycont, &kernel_thread);	//for getting to kernel thread to schedule
/*if(thread->th.state!=EXITED){
  setcontext (&kernel_thread);	//for getting to kernel thread to schedule
}*/
    }
  *value_ptr = (void *) thread->th.ret_val;	// will return the value from tcb block
//  thread->th.state=EXITED;
  uthread_exit ((void *) thread);	// for exiting and permanantly eliminating the thread from the system 
//printf("exiting  join_thread \n");
  return 0;
}


/************************************************************************************
 This function should be implicitly called when a thread finishes its start function
 value_ptr receives the return value of the thread start function
*************************************************************************************/
void
uthread_exit (void *value_ptr)
{
  long t = *((long *) value_ptr);
  printf ("got the tid as  : %lu", t);
// delete node from thread queue and from priority queue
//need to update thread queue to finally distroy the struct of the thread calling this function
//  threads *thread;

//thread=findtcb(threadList,tid);       // it will return the pointer to the found thread
//  printf(" in join %ld \n",(long)thread->th.ret_val);
//  *value_ptr = (void *)thread->th.ret_val;    // will return the value from tcb block

}

/****************************************************************************
 the behavior will be similar to calling clock_gettime(CLOCK_MONOTONIC, &tp)
*****************************************************************************/
int
uthread_gettime (struct timespec *tp)
{				/*supposed to be a simple wrapper function of clock_gettime */
return clock_gettime(CLOCK_MONOTONIC, tp);
}

/********************************************************************************
 the behavior wil be similar to calling 
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &request, 0)
*********************************************************************************/
int uthread_abstime_nanosleep(const struct timespec *request)
{
//printf("this is the thread  called uthread nano sleep\n");
/*timer_t timerid;
struct sigevent sev;*/
struct itimerspec its;
long long freq_nanosecs;
sigset_t mask;
  ucontext_t *mycont;
  mycont = (ucontext_t *) malloc (sizeof (ucontext_t));	//for saving uthread creat's context
  sleep_curr_task = 1;		// will signal kernel to indicate that main is done and 
  //wait until the thread finishes
  if (getcontext (&kernel_thread) == -1)
    {
      printf ("error while creating thread\n");
      exit (0);
    }	if (getcontext (mycont) == -1)
    {
      printf ("error while creating thread\n");
      exit (0);
    }	
threads *temp;//=malloc(sizeof(threads*));
 int res=get_running(&temp);
//printf("get_running ke aage atak raha hai\n");
if(res==0){
//printf("in res==0 \n");

           sigemptyset(&mask);
          sigaddset(&mask, SIG);
           if (sigprocmask(SIG_SETMASK, &mask, NULL) == -1)
               printf("sigprocmask");

           memcpy(&its.it_value,request,sizeof(struct timespec));//freq_nanosecs / 1000000000;
           its.it_interval.tv_sec = 0;//its.it_value.tv_sec;
           its.it_interval.tv_nsec = 0;//its.it_value.tv_nsec;
	//	printf(" %d secs  and %ld nanosecs \n",(int )its.it_value.tv_sec,its.it_value.tv_nsec);
      if (timer_settime(temp->timerid,TIMER_ABSTIME, &its, NULL) == -1)
                printf("\n timer_settime error \r ");
            if (sigprocmask(SIG_UNBLOCK, &mask, NULL) == -1)
              printf("sigprocmask");
//	    (*temp)->timerid=timerid;	//update timer id in thread
            temp->str=its;	//request the time at which it will go to sleep
            temp->th.state=DELAYED;
//      kernel_thread.uc_stack.ss_sp = malloc (8192);	// allocate 8K stack
//      kernel_thread.uc_stack.ss_size = 8192;	//sizeof(kernel_thread.uc_stack.ss_sp);             // save size of stack pointer
//      kernel_thread.uc_link = mycont;
//printf("this is the thread %ld with timerid= %ld\n",temp->tid,(long)temp->timerid);
      makecontext (&kernel_thread, kernelthread, 0);	//makes context for start function
//printf("after make context if it fails \n");
      swapcontext (temp->ucp, &kernel_thread);	//for getting to kernel thread to schedule
}else
{
printf("something that was not supposed to happen happened \n");
//      setcontext (&kernel_thread);	//for getting to kernel thread to schedule
} 
//printf("sleep is about to end\n");
//while(1);
/*supposed to be a simple wrapper function */
//clock_nanosleep(CLOCK_MONOTONIC,TIMER_ABSTIME, &request, 0)
}
//#define uthread_abstime_nanosleep(x) uthread_abstime_nanosleep_me((x),__FUNCTION__)
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

