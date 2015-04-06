#define _GNU_SOURCE 		// for CPUZERO Macro
#include <unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include<signal.h>
#include<pthread.h>
#include<semaphore.h>
#include <fcntl.h>
#include<sched.h>
#include<string.h>
#include<math.h>
#include<signal.h>
#include<ctype.h>
#include<errno.h>

sem_t rtclk;			// semaphore for right clk
sem_t ltclk;			// semaphore for left  clk
sem_t start_at_same_time;	// start all process at same time
pthread_mutex_t mutex[10];   //10 mutex locks
pthread_mutexattr_t attr;
static sigset_t   set;

void *read_task(){
	char mouse_buf[3];
	char lb,rb;
	int policy,efp;
	char *eventfilename="/dev/input/mice";
	struct sched_param param;
	efp  = open(eventfilename,O_RDONLY);		// open file to read event
	param.sched_priority=sched_get_priority_max(SCHED_FIFO);
	pthread_setschedparam(pthread_self(),SCHED_FIFO,&param);//
	pthread_getschedparam(pthread_self(),&policy,&param);//	
	while(1){	
		if(read(efp, mouse_buf, sizeof(mouse_buf))>0){
		lb=(mouse_buf[0] & 0x09);			// this is left button	
		rb=(mouse_buf[0] & 0x0a);			// this is left button	
		if((lb==0x09)){	
			sem_post(&ltclk);	//semaphore post for ltclk
			}		// aperiodic task body here
		else if(rb==0x0a){
			sem_post(&rtclk);	//semaphore post for rtclk
			}		// aperiodic task body here
		}
	}
}
void periodic_body(char *arg);
void *aperiodic_thread(void *arg){
	int event,k,policy;
	char c;
	char *temp=(char*)arg;
	struct sched_param param;
	int efp;
	char *mouse_buf[3];
	FILE *fp=tmpfile();	// to create a dummy file
	fputs(temp,fp);		// to put the string on tem file
	fseek(fp,0,SEEK_SET);
	fscanf(fp,"%d %d ",&param.sched_priority,&event);	// for reading necessary variables
	pthread_setschedparam(pthread_self(),SCHED_FIFO,&param);//	
//	param.sched_priority=0;
//	pthread_getschedparam(pthread_self(),&policy,&param);//	
	sem_wait(&start_at_same_time);		// to make sure that threads are triggered simultaneously 
	k=0;
	c=90;
	temp=(char *)calloc(100,sizeof(char));
		while(c != '\n' && !feof(fp)){
			fscanf(fp,"%c",&c);
			temp[k++]=c;
		}
		while(1){
			if(event==1){
				sem_wait(&ltclk);
			}
			else if(event == 0){
				sem_wait(&rtclk);
			}
		}
}
void *periodic_thread(void *arg){
	int period,k,policy;
	char c;
	char *temp=(char*)arg;
	struct timeval tp;
	struct sched_param param;
	FILE *fp=tmpfile();	// to create a dummy file
	fputs(temp,fp);		// to put the string on tem file
	fseek(fp,0,SEEK_SET);
	fscanf(fp,"%d %d ",&param.sched_priority,&period);	// for reading necessary variables
	pthread_setschedparam(pthread_self(),SCHED_FIFO,&param);//	
//	pthread_getschedparam(pthread_self(),&policy,&param);//	
	k=0;
	c=90;
	temp=(char *)calloc(100,sizeof(char));
		while(c != '\n' && !feof(fp)){
			fscanf(fp,"%c",&c);
			temp[k++]=c;
		}
		suseconds_t rdtsc_t1,rdtsc_t2,expected_next;
		float timeelapsed,remtime;
		gettimeofday(&tp,NULL);
		expected_next=((tp.tv_sec%100)*1000000)+tp.tv_usec+(period*1000);	//is in micro secs
	sem_wait(&start_at_same_time);		// to make sure that threads are triggered simultaneously 
		while(1){
		periodic_body(temp);
		gettimeofday(&tp,NULL);
		rdtsc_t2=((tp.tv_sec%100)*1000000)+tp.tv_usec;		// in micro secs
		if(rdtsc_t2>expected_next){
			double temp= ceil((rdtsc_t2-expected_next)/(period*1000.0));
			expected_next=expected_next + (temp)*(period*1000);
			continue;
		}
		remtime=(expected_next-rdtsc_t2);  //remtime in milisecond
		usleep((remtime));
		expected_next=expected_next+(period*1000);
		}
}
void periodic_body(char *arg){
	int init,i,j,k;
	char lcinf[4],c;
	char *temp=arg;
	FILE *fp=tmpfile();	// to create a dummy file
	fputs(temp,fp);		// to put the string on tem file
	fseek(fp,0,SEEK_SET);
	temp=(char *)calloc(100,sizeof(char));
	k=0;
	c=90;
	fscanf(fp,"%d %s ",&init,lcinf);	// for reading necessary variables
			fscanf(fp,"%c",&c);
			do{
			temp[k++]=c;
			fscanf(fp,"%c",&c);
		}while((c != '\n') && !feof(fp));
	for(i=0;i<init;i++){		//initial wait
		 j++;
	}
	int lock_no=0;
	fclose(fp);
	if(lcinf[0]=='L'){
		// apply lock
		lock_no=atoi(&lcinf[1]);
		pthread_mutex_lock(&mutex[lock_no]);		//applied lock
		periodic_body(temp);
	}
	if(lcinf[0]=='U'){
		lock_no=atoi(&lcinf[1]);
//		printf("unlocked %d\n",lock_no);
		pthread_mutex_unlock(&mutex[lock_no]);		//released lock
		periodic_body(temp);
	}
}
pthread_mutex_t start_mutex;   //10 mutex locks
int main(int argc,char **argv){
	FILE *fp;
	int count=0;
	int term,i,j,k,pri,event;
	char *buf,type,c;
	char *token;
	pthread_t click_th;
	sem_init(&start_at_same_time,0,0);
	struct sched_param param;
	cpu_set_t cpu_mask;
	CPU_ZERO(&cpu_mask);       // Initialize it all to 0, i.e. no CPUs selected.
	CPU_SET(1, &cpu_mask);  
	sched_setaffinity(0,sizeof(cpu_set_t),&cpu_mask);
	if(argc<2){
		printf("the format of this program is supposed to be: \n ./a.out <name of the specification file> <Y> priority inheritance\n");
	}
	int con=sem_init(&ltclk,0,0);		// lt clk
	if(con<0){
			printf("semaphore failed \n");
		}
	con=sem_init(&rtclk,0,0);		//rt clk
	if(con<0){
			printf("semaphore failed \n");
		}
	if(!strcmp(argv[2],"Y")){
	printf("in Y \n");
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setprotocol(&attr,PTHREAD_PRIO_INHERIT);	
}	param.sched_priority=sched_get_priority_max(SCHED_FIFO);
	pthread_setschedparam(0,SCHED_FIFO,&param);
	fp=fopen(argv[1],"r");		//open the  file for reading only
	fseek(fp,0,SEEK_SET);		// look for the begining
	fscanf(fp,"%d %d\n",&count,&term);
	pthread_t *pthrd=malloc(count*sizeof(pthread_t));
			pthread_create(&click_th,NULL,read_task,NULL);
		for(i=0;i<10;i++){	
			pthread_mutex_init(&mutex[i],&attr);
		}
	for(i=0;i<count;i++){
		buf = (char *)calloc(100,sizeof(char));//sizeof(char));
		fscanf(fp,"%c",&type);
		if(type=='P'){
			int y,loopt;
			k=0;
			c=90;
			while(c != '\n'){
				fscanf(fp,"%c",&c);
				buf[k++]=c;
			}
			fflush(NULL);
			pthread_create(&pthrd[i],NULL,periodic_thread,(void *)buf);	// for periodic task
		}
		else if(type=='A'){
			c=90;
			k=0;
			while(c != '\n'){
				fscanf(fp,"%c",&c);
				buf[k++]=c;
			}
			pthread_create(&pthrd[i],NULL,aperiodic_thread,(void *)buf);	//for aperodic task
		}
	}
	int tempp;
	pthread_setschedparam(pthread_self(),SCHED_FIFO,&param);
	param.sched_priority=0;
	pthread_getschedparam(pthread_self(),&tempp,&param);
		pthread_mutex_lock(&start_mutex);		//applied lock
		usleep(1000);
	for(i=0;i<count;i++){
	sem_post(&start_at_same_time);		// semaphore post
	}
		pthread_mutex_unlock(&start_mutex);		//applied lock
	usleep((term)*1000);	
	for(i=0;i<count;i++){
		pthread_cancel(pthrd[i]);
	}
		pthread_cancel(click_th);
	sem_destroy(&ltclk);		// destroy semaphore
	sem_destroy(&rtclk);		// destroy semaphore 
	fclose(fp);
	return 0;
}
