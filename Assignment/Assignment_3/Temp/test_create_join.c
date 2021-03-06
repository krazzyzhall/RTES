#include <stdio.h>
#include <stdlib.h>
#include "uthread.h"
#include <mcheck.h>
void *thread0(void *arg)
{
	printf("thread 0 - arg = %ld\n", (long)arg);
	return (void *)0;
}

void *thread1(void *arg)
{
	printf("thread 1 - arg = %ld\n", (long)arg);
	return (void *)11;
}

void *thread2(void *arg)
{
	printf("thread 2 - arg = %ld\n", (long)arg);
	return (void *)22;
}

int main()
{
//mtrace();
	int i;
	void *uthread_ret[3];
	uthread_t tid[3];
	
//		printf("thread 1returned %ld\n",(long)uthread_ret[0]);
	uthread_create(&tid[0], thread0, (void *)0x0, 3);
//		printf("thread 1returned %ld\n",(long)uthread_ret[0]);
	uthread_create(&tid[1], thread1, (void *)0x1, 1);
//		printf("thread 2 returned %ld\n",(long)uthread_ret[1]);
	uthread_create(&tid[2], thread2, (void *)0x2, 2);
//		printf("thread 3 returned %ld\n",(long)uthread_ret[2]);
	
	for(i=0; i<3; i++)
	{
		uthread_join(tid[i], &uthread_ret[i]);
		printf("thread %ld returned %ld\n", tid[i], (long)uthread_ret[i]);
	}
//	muntrace();
	return 0;
}
