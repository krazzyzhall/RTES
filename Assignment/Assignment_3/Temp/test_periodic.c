#include <stdio.h>
#include <stdlib.h>
#include "uthread.h"
//#include "uthread.c"
typedef struct per_thread_t
{
	long id;
	long period;//ms
	int priority;
	struct timespec period_start;
} per_thread_t;

void *periodic_thread(void *arg)
{
	int i;
	long j;
	long s = 0;
	struct timespec next;
	long long tmp;
	per_thread_t *t_arg = (per_thread_t *)arg;
	printf("thread id: %ld, period: %ldms\n", t_arg->id, t_arg->period);
	
	next = t_arg->period_start;
//	next.tv_sec=0;
 //       next.tv_nsec=0;
	for(i=0; i<5; i++)
	{
		//task body
		for(j=0; j<10000000; j++)
		{
			s = s + j;
		}

		//get the next period start
		tmp = (next.tv_sec*1000000000 + next.tv_nsec + t_arg->period*1000000);
		next.tv_sec = tmp/1000000000;
		next.tv_nsec = tmp%1000000000;
//		printf(" this is the tmp value :%lld \n",tmp);
		uthread_abstime_nanosleep (&next);
	}
		printf(" after completing sleeping process\n");	
	return (void *)0;
}

int main()
{
	int i;
	void *uthread_ret[2];
	uthread_t tid[2];
	per_thread_t thread_arg[2];
	struct timespec period_start;
	uthread_gettime(&period_start);
//	printf("gettime value : %ld \n",(period_start.tv_sec*1000000000)+period_start.tv_nsec);
	thread_arg[0].id = 0;
	thread_arg[0].period = 5000;
	thread_arg[0].priority = 1;
	thread_arg[0].period_start = period_start;
	
	thread_arg[1].id = 1;
	thread_arg[1].period = 4000;
	thread_arg[1].priority = 2;
	thread_arg[1].period_start = period_start;

	for(i=0; i<2; i++)
		uthread_create(&tid[i], periodic_thread, (void *)&thread_arg[i], thread_arg[i].priority);
	
	for(i=0; i<2; i++)
	{
		uthread_join(tid[i], &uthread_ret[i]);
		printf("thread %d returned %ld\n", i, (long)uthread_ret[0]);
	}
	return 0;
}
