#ifndef _UTHREAD_H_
#define _UTHREAD_H_
#include <ucontext.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include<stdlib.h>
#include<stdio.h>
#include <signal.h>
//#define uthread_abstime_nanosleep(x) _uthread_abstime_nanosleep((x), __func__)
#define BLOCKED 11	// if blocked by higher priority or resource unavailability
#define DELAYED 12
#define READY   13
#define EXITED  14
#define RUNNING 15 
/******************************************************************** 
 CSE522 Assignment 3, Arizona State University, Spring/2015
 This file shall NOT BE MODIFIED
*********************************************************************/

typedef long int uthread_t;
typedef long int uthread_mutex_t;
typedef struct tcb{
 int state;
// jmp_buf context;
 int priority;
 void *args;		// args passed to the function 
 void *stack;		// storing top pointer of stack
 void *(*func_ptr)(void *);	// for storing address of function to be called
 void *ret_val;
 //some other information continues
} tcb;
typedef struct threads{
 uthread_t tid;
 tcb th;
 struct sigaction sa;
 struct sigevent sev;
 ucontext_t *ucp;
 struct itimerspec str;
 timer_t timerid;
 struct threads *next;
 struct threads *prev;
} threads;
/*block holding thread information*/
/* priority queue array*/
threads *priority[100];
/*
 Define resource table for stack-based priority ceiling protocol
*/
typedef struct resource_entry_t
{
	uthread_mutex_t resource; //mutex is only resouce we consider
	uthread_t *thread_array; //array of threads that are going to use this resource
	int n; //number of elements in thread_array
} resource_entry_t;

/***************************************************************************
 Mutex attibutes
 When multiple threads are waiting for a mutex, 
  the highest priority thread takes the lock next.
 UTHREAD_MUTEX_ATTR_PI: 
  - Basic priority inheritance protocol is applied.
  - A thread that has inherited higher priority than its assigned priority 
	  will get its original priority back when it releases all the locks.
***************************************************************************/
#define UTHREAD_MUTEX_ATTR_NONE		0
#define UTHREAD_MUTEX_ATTR_PI			1

/*********************************************************
 All the following functions shall return zero for success
 and -1 for any failures
**********************************************************/

/*****************************************************************************************
 tid      : returned thread identification for the subsequent uses (e.g., uthread_join()) 
 start    : thread start function
 args     : argument passed to the start function
 priority : valid priority range: 0 ~ 100. 0 is the highest. 0 is reserved for main()
*****************************************************************************************/
int uthread_create(uthread_t *tid, void *(*start)(void *), void *args, int priority);

/********************************************************************************
 The calling thread blocks until the thread with tid exits (call uthread_exit())
 *value_ptr contains the passed value from uthread_exit()
*********************************************************************************/
int uthread_join(uthread_t tid, void **value_ptr);

/************************************************************************************
 This function should be implicitly called when a thread finishes its start function
 value_ptr receives the return value of the thread start function
*************************************************************************************/
void uthread_exit(void *value_ptr);

/****************************************************************************
 the behavior will be similar to calling clock_gettime(CLOCK_MONOTONIC, &tp)
*****************************************************************************/
int uthread_gettime(struct timespec *tp);

/********************************************************************************
 the behavior wil be similar to calling 
  clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &request, 0)
*********************************************************************************/
int uthread_abstime_nanosleep(const struct timespec *request);

/****************************************************************
 similar to pthread_mutex_init()
 attr is either UTHREAD_MUTEX_ATTR_NONE or UTHREAD_MUTEX_ATTR_PI
*****************************************************************/
int uthread_mutex_init(uthread_mutex_t *mutex, const int attr);

/****************************************************************
 lock/unlock "mutex"
*****************************************************************/
int uthread_mutex_lock(uthread_mutex_t *mutex);
int uthread_mutex_unlock(uthread_mutex_t *mutex);

/**********************************************************************************
 enable stack-based priority ceiling protocol
 this function should be called in main() after creating all threads and mutexes  
 mutex attributes defined previously are ignore by calling this function
	 resource_table: array of resources that are used in the program
   n: number of resources in the table
***********************************************************************************/
int uthread_srp_enable(resource_entry_t *resource_table, int n);

#endif
