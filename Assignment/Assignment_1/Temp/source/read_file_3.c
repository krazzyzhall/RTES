#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
struct task_seq{   //every node of link list will have this
	char *pending_seq;
//	int busy_cycle;
//	struct task_seq *next;
};
typedef struct task_seq task_seq;
typedef struct {
	int pri;
	int period;
	char type;
	task_seq seq;
	}task_p;
int main(int argc,char **argv){
FILE *fp;
int count=0;
int term,i,j,k,pri,event;
char buf[100],type,c;
char *token;
if(argc<2){
	printf("the format of this programm is supposed to be: \n ./a.out <name of the specification file>\n");
}
//printf("%s",argv[0]);
fp=fopen(argv[1],"r");		//open the  file for reading only
fseek(fp,0,SEEK_SET);		// look for the begining
fscanf(fp,"%d %d\n",&count,&term);
task_p *task=(task_p *)malloc(count*sizeof(task_p*));	//make an array of tasks
printf("total number of threads : %d will terminate after %d mil sec \n",count,term);
//task_seq *curr=&task[];
for(i=0;i<count;i++){
	task_seq *temp;//=(task_seq *)malloc(sizeof(task_seq *));	//make the node
	//curr->next=temp;
	//task[i].seq=temp;
			if(feof(fp))
				break;
	fscanf(fp,"%c",&type);
	task[i].type=type;
	if(type=='P'){
		int y,loopt;
		k=0;
		fscanf(fp,"%d %d ",&pri,&loopt);
		task[i].pri=pri;	//set priority of task
		task[i].period=loopt;	//period of the thread;
		printf("its perodic with loopt : %d and priority : %d\n",loopt,pri);
		while(c != '\n'){
			fscanf(fp,"%c",&c);
			buf[k++]=c;
		}
		token=strtok(buf," ");
		temp=(task_seq *)malloc(sizeof(task_seq *));	//make the node
		if(token[0]=='L' || token[0]=='U'){
			//to set lock
			task[i].seq.lock_num[0]=token[0];
			task[i].seq.lock_num[1]=token[1];
		}
		if(isdigit(token[0])){
			task[i].seq.busy_cycle=atoi(token);
			printf("%d ",task[i].seq.busy_cycle);
			//to set unlock sequence
		}
		temp=&(task[i].seq);	//get address of first seq element 
			token=strtok(NULL," ");
		while(token!=NULL){
		temp->next=(task_seq *)malloc(sizeof(task_seq *));	//make the node
		temp=temp->next;
		if(token[0]=='L' || token[0]=='U'){
			temp->lock_num[0]=token[0];
			temp->lock_num[1]=token[1];
			temp->busy_cycle=-1;
		}
		if(isdigit(token[0])){
			temp->lock_num[0]='N';
			temp->lock_num[1]='N';
			temp->busy_cycle=atoi(token);
			printf("%d ",temp->busy_cycle);
		}
		//temp->next=(task_seq *)malloc(sizeof(task_seq *));	//make the node
		//temp=temp->next;
			token=strtok(NULL," ");
		}
		printf("\n");
	}
	else if(type=='A'){
		printf("its aperodic \n");
	}
//type=fgetc(fp);
//printf("%c %s\n",type,buf);
//fscanf(fp,"\n");
}
close(fp);
return 0;
}
