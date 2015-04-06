#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
struct task_seq{   //every node of link list will have this
	char lock_num[2];
	int busy_cycle;
	struct task_seq *next;
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
char *buf,type,c;
char *token;
if(argc<2){
	printf("the format of this programm is supposed to be: \n ./a.out <name of the specification file>\n");
}
k=0;
//printf("%s",argv[0]);
fp=fopen(argv[1],"r");		//open the  file for reading only
fseek(fp,0,SEEK_SET);		// look for the begining
while( !feof(fp)){		
k=0;
c=c+20;
buf=(char *)calloc(100,sizeof(char));
	while(c != '\n' && !feof(fp)){
//			if()
			fscanf(fp,"%c",&c);
			buf[k++]=c;
		}
//fseek(fp,10,SEEK_CUR);		// look for the begining
k=0;
//while(buf[k++]!='\n')
printf("%s",buf);
free(buf);
//printf("\n");

	}
//puts(buf);
close(fp);
return 0;
}
