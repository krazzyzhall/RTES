#include<fstream>	// for io ops of file
#include<cstring>	// for memcpy
#include<cstdlib>
#include<math.h>
#include<iostream>
#include<time.h>
using namespace std;
struct task_attr{
	float wcet;
	float deadline;
	float period;
	float resp_time;	// worst case response time at critical instant.
	float slack_time;	// saves value of worst case execution including wcet of higher processes.
	bool is_sched;	// after response time analusis
};
class synthetic_taskset{
	private:
		struct task_attr *taskset;	//data structure used to store synthetic task set.
		int numberoftasks;		// how many number of tasks are there.
	public:
	void get_taskset(struct task_attr *set){
		memcpy(set,taskset,numberoftasks*sizeof(struct task_attr));	//for copying to task info in native structure
		}
		synthetic_taskset(int no_tasks){	// generates no_tasks number of synthetic task set.
			taskset=(struct task_attr*)malloc(no_tasks*sizeof(struct task_attr));	//allocat memory
			numberoftasks=no_tasks;		// set numer in the object
		}
		float* Uscaling(int n,float U){
				int random[n];
				float sum=0;
				float *ret_array=(float *)malloc(n*sizeof(float));
			for(int i=0;i<n;i++){
				random[i]=rand()%n + 1;
				sum+=random[i];
			}
			for(int i=0;i<n;i++){
				ret_array[i]=(random[i]*U + 1)/sum;		//calculates utilization
			}
			for(int i=0;i<n;i++){
				taskset[i].period=(rand()%numberoftasks) + 1;	//set random period
				taskset[i].wcet=ret_array[i]*taskset[i].period; 		// set wcet with respect to its utilization and random period. 
				taskset[i].deadline=(rand()%numberoftasks)+taskset[i].wcet;	// set random deadline
			}
			
		}
		float* Ufitting(int n,float U){
				int random[n];
				float sum=0;
				float upperLimit=U;
				float *ret_array=(float *)malloc(n*sizeof(float));
			for(int i=0;i<n-1;i++){
				random[i]=(((rand()%n) + 1.0))*upperLimit;
				upperLimit=upperLimit-((float)random[i]/n);
			}
			random[n-1]=upperLimit;
			for(int i=0;i<n;i++){
				taskset[i].period=(rand()%numberoftasks) + 1;	//set random period
				taskset[i].wcet=((float)random[i]/n)*taskset[i].period; 		// set wcet with respect to its utilization and random period. 
				taskset[i].deadline=(rand()%numberoftasks)+taskset[i].wcet;	// set random deadline
			}
			
		}
		float* Uunifast(int n,float U,int M,int type){
				int random[n];
				float sumU=U;
				float nextsumu;
				float *ret_array=(float *)malloc(n*sizeof(float));
			for(int i=0;i<n-1;i++){
				nextsumu=sumU*pow((float)(rand()%n + 1)/n,1.0/(n-i-1));
				ret_array[i]=sumU-nextsumu;
				sumU=nextsumu;
			}
			ret_array[n-1]=sumU;
			for(int i=0;i<n;i++){
				int temp=rand()%M;
				int temp2,temp3;
				if(temp==0){	//period should be between 1000 to 10000
					temp2=1000;
					temp3=100;
				}else if(temp==1){
					temp2=10000;
					temp3=1000;
				}
				else{
					temp2=100000;
					temp3=10000;
				}
				taskset[i].period=(rand()%temp2) + temp3;	//set random period
				taskset[i].wcet=(ret_array[i])*taskset[i].period; 		// set wcet with respect to its utilization and random period. 
				if(type==0){
					taskset[i].deadline=(int)((rand()%(int)taskset[i].period)-taskset[i].wcet)%((int)taskset[i].period) + taskset[i].wcet;	// set random deadline
				}else{
					taskset[i].deadline=(int)((rand()%(int)taskset[i].period)-((int)taskset[i].wcet)%((int)taskset[i].period)) + (taskset[i].wcet + ((taskset[i].period-taskset[i].wcet)/2));	// set random deadline
				}
			}
			
		}
};
class rm{
private:
	struct task_attr *tasks;
	int numberoftasks;
	int no_task_schedulable;
	int no_task_unschedulable;
	float util;
public:
	int get_sched_tasks(){
		return no_task_schedulable;
	}
	void set_taskset(struct task_attr *set){
		memcpy(tasks,set,numberoftasks*sizeof(struct task_attr));	//for copying to task info in native structure
		}
	int get_no_tasks(){			// return number of tasks 
		return numberoftasks;
	}
	void get_tasks(struct task_attr &);	//for getting the list of all the tasks
	rm(int no_task){
		tasks=(struct task_attr*)malloc(no_task*sizeof(struct task_attr));
		numberoftasks=no_task;
	no_task_schedulable=0;
	no_task_unschedulable=0;
	}
	void set_tasks(struct task_attr task,int task_num){
		memcpy(&tasks[task_num],&task,sizeof(struct task_attr));	//for copying to task info in native structure

	}
	float calc_util_bound(int no_tasks){
		util=no_tasks*(pow(2,1.0/no_tasks)-1);
		return util;
	}
	float get_azero(int task_no){			//function to calculate A0 for response time analysis
		float azero=tasks[task_no].wcet;
		for(int i=0;i<numberoftasks;i++){
			if(tasks[i].period<tasks[task_no].period)
				azero=azero + tasks[i].wcet;	//summation of wcet
		}
		return azero;
	}
	float resp_tim_for_task(int task_no){		// function to calculate response time of the task 
		float anplusone;
		float an;
		float azero=get_azero(task_no);
		an=azero;
		do{
			float temp=0;
			for(int j=0;j<numberoftasks;j++){
				if(tasks[j].period<=tasks[task_no].period && (j!=task_no)){	//to include only high priority tasks
					float temp2=an/tasks[j].period;
					temp=temp+ceil(temp2)*tasks[j].wcet;
				}
			}
			anplusone= tasks[task_no].wcet + temp;
			if(anplusone==an)	// termination condition
				break;
			an=anplusone;
		}while(1);
		tasks[task_no].resp_time=an;	//sets resp time of tha task in this task set
	return an;
	}
	bool taskset_schedulable(){			//check if the taskset is schedulable or not
		bool is_sched=true;
		for(int i=0;i<numberoftasks;i++){
			is_sched=is_sched && tasks[i].is_sched;	// check for all tasks if set is schedulable	
		}
		return is_sched;
	}
	bool is_bounded(int task_no){			// see of the task is schedulable or not
		float util_by_tasks=0;
		float din,tutil;
		int count=0;
		for(int i=0;i<numberoftasks;i++){
			if(tasks[task_no].period>=tasks[i].period){	// check all the higher priority tasks.
				count++;
				din=tasks[i].period;
				if(tasks[i].deadline < tasks[i].period){
					din=tasks[i].deadline;
				}
			
				util_by_tasks+=(tasks[i].wcet/din);
			}
		}
			tutil=calc_util_bound(count);	//util bound calculation
			if(util_by_tasks>1){
				tasks[task_no].is_sched=false;
				no_task_unschedulable++;
				return 0;
			}
			else if(util_by_tasks<=tutil){
				tasks[task_no].is_sched=true;
				no_task_schedulable++;
				return 1;
			}
			else{
				int resp_tim=resp_tim_for_task(task_no);		// get resp of task_no in presence of higher priority tasks
				if(tasks[task_no].is_sched=resp_tim<=tasks[task_no].deadline){	
					no_task_schedulable++;
				}else{		// if response time of task is less than deadline then schedulable
					no_task_unschedulable++;
			}	return tasks[task_no].is_sched=resp_tim<=tasks[task_no].deadline;		// if response time of task is less than deadline then schedulable
			}
	}
};
class dm{
private:
	struct task_attr *tasks;
	int numberoftasks;
	int no_task_schedulable;
	int no_task_unschedulable;
	int util;
public:	
	int get_sched_tasks(){
		return no_task_schedulable;
	}
	void set_taskset(struct task_attr *set){
		memcpy(tasks,set,numberoftasks*sizeof(struct task_attr));	//for copying to task info in native structure
		}
	int get_no_tasks(){			// return number of tasks 
		return numberoftasks;
	}
	void get_tasks(struct task_attr &);	//for getting the list of all the tasks
	dm(int no_task){
		tasks=(struct task_attr*)malloc(no_task*sizeof(struct task_attr));
		numberoftasks=no_task;
		no_task_schedulable=0;
		no_task_unschedulable=0;
	}
	void set_tasks(struct task_attr task,int task_num){
		memcpy(&tasks[task_num],&task,sizeof(struct task_attr));	//for copying to task info in native structure
	}
	float calc_util_bound(int no_tasks){
		util=no_tasks*(pow(2,1.0/no_tasks)-1);
		return util;
	}
	float get_azero(int task_no){
		float azero=tasks[task_no].wcet;
		for(int i=0;i<numberoftasks;i++){
			if(tasks[i].deadline<tasks[task_no].deadline)
				azero=azero + tasks[i].wcet;	//summation of wcet
		}
		return azero;
	}
	bool taskset_schedulable(){			//check if the taskset is schedulable or not
		bool is_sched=true;
		for(int i=0;i<numberoftasks;i++){
			is_sched=is_sched && tasks[i].is_sched;	// check for all tasks if set is schedulable	
		}
		return is_sched;
	}
	bool is_bounded(int task_no){			// see of the task is schedulable or not
		float util_by_tasks=0;
		float din,tutil;
		int count=0;
		for(int i=0;i<numberoftasks;i++){
			if(tasks[task_no].deadline>=tasks[i].deadline){	// check all the higher priority tasks.
				count++;
				din=tasks[i].period;
				if(tasks[i].deadline < tasks[i].period){
					din=tasks[i].deadline;
				}			
				if(tasks[i].period<din){
					din=tasks[i].period;		//higher priority task with smaller period
				}
				else if(tasks[i].period>din){
					din=tasks[task_no].period;
				if(tasks[task_no].deadline < tasks[task_no].period){
					din=tasks[task_no].deadline;
				}			
				}
				/*din=tasks[i].period;
				if(tasks[i].deadline < tasks[i].period){
					din=tasks[i].deadline;
				}*/
			
				util_by_tasks+=(tasks[i].wcet/din);
			}
		}
			if(util_by_tasks>=1){		// if density is more than one it is supposed to do time response analysis
				int resp_tim=resp_tim_for_task(task_no);		// get resp of task_no in presence of higher priority tasks
				if(tasks[task_no].is_sched=(resp_tim<=tasks[task_no].deadline)){
					no_task_schedulable++;
				}else{
					no_task_unschedulable++;
				}
				return tasks[task_no].is_sched;		// if response time of task is less than deadline then schedulable
			}
			else if(util_by_tasks<1){
				no_task_schedulable++;
				tasks[task_no].is_sched=true;
				return 1;
			}
	}
	float resp_tim_for_task(int task_no){
		float anplusone;
		float an;
		float azero=get_azero(task_no);
		an=azero;
		do{
			float temp=0;
			for(int j=0;j<numberoftasks;j++){
				if(tasks[j].deadline<=tasks[task_no].deadline &&(j!=task_no)){	//to include only high priority tasks
					float temp2=an/tasks[j].period;
					temp=temp+ceil(temp2)*tasks[j].wcet;
				}
			}
			anplusone= tasks[task_no].wcet + temp;
			if(anplusone==an)	// termination condition
				break;
			an=anplusone;
		}while(1);
		tasks[task_no].resp_time=an;	//sets resp time of tha task in this task set
	return an;
	}
	bool resp_analysis(){
		for(int i=0;i<numberoftasks;i++){
			float resp_tim=resp_tim_for_task(i);	//get response time for the task.
			if(tasks[i].resp_time>tasks[i].deadline){
				return false;
			}
		}
		return true;
	}
};
class LST{
private:
	struct task_attr *tasks;
	int numberoftasks;
	int no_task_schedulable;
	int no_task_unschedulable;
	int util;
public:
	int get_sched_tasks(){
		return no_task_schedulable;
	}
	void set_taskset(struct task_attr *set){
		memcpy(tasks,set,numberoftasks*sizeof(struct task_attr));	//for copying to task info in native structure
		for(int i=0;i<numberoftasks;i++){
			tasks[i].slack_time=tasks[i].deadline - tasks[i].wcet;	// get slack
		}
		}
	int get_no_tasks(){			// return number of tasks 
		return numberoftasks;
	}
	float calc_util_bound(int no_tasks){
		util=no_tasks*(pow(2,1.0/no_tasks)-1);
		return util;
	}
	void get_tasks(struct task_attr &);	//for getting the list of all the tasks
	LST(int no_task){
		tasks=(struct task_attr*)malloc(no_task*sizeof(struct task_attr));
		numberoftasks=no_task;
		no_task_schedulable=0;
		no_task_unschedulable=0;
	}
	void set_tasks(struct task_attr task,int task_num){
		memcpy(&tasks[task_num],&task,sizeof(struct task_attr));	//for copying to task info in native structure
		tasks[task_num].slack_time=tasks[task_num].deadline - tasks[task_num].wcet;	// get slack
	}
	float get_azero(int task_no){			//function to calculate A0 for response time analysis
		float azero=tasks[task_no].wcet;
		for(int i=0;i<numberoftasks;i++){
			if(tasks[i].slack_time<tasks[task_no].slack_time)
				azero=azero + tasks[i].wcet;	//summation of wcet
		}
		return azero;
	}
	bool taskset_schedulable(){			//check if the taskset is schedulable or not
		bool is_sched=true;
		for(int i=0;i<numberoftasks;i++){
			is_sched=is_sched && tasks[i].is_sched;	// check for all tasks if set is schedulable	
		}
		return is_sched;
	}
	bool is_bounded(int task_no){			// see of the task is schedulable or not
		float util_by_tasks=0;
		float din,tutil;
		int count=0;
		for(int i=0;i<numberoftasks;i++){
			if(tasks[task_no].slack_time>=tasks[i].slack_time){	// check all the higher priority tasks including itself.
				count++;
				din=tasks[i].period;
				if(tasks[i].deadline < tasks[i].period){
					din=tasks[i].deadline;
				}			
				if(tasks[i].period<din){
					din=tasks[i].period;		//higher priority task with smaller period
				}
				else if(tasks[i].period>din){
					din=tasks[task_no].period;
				if(tasks[task_no].deadline < tasks[task_no].period){
					din=tasks[task_no].deadline;
				}			
				}
				util_by_tasks+=(tasks[i].wcet/din);
			}
		}
			tutil=calc_util_bound(count);	//util bound calculation
			if(util_by_tasks>tutil){		// if density is more than one it is supposed to do time response analysis
				int resp_tim=resp_tim_for_task(task_no);		// get resp of task_no in presence of higher priority tasks
				if(tasks[task_no].is_sched=resp_tim<=tasks[task_no].deadline){	
				no_task_schedulable++;
				}else {		
					no_task_unschedulable++;
				}return tasks[task_no].is_sched;		// if response time of task is less than deadline then schedulable
				
			}
			else if(util_by_tasks<1){
				tasks[task_no].is_sched=true;
				no_task_schedulable++;
				return 1;
			}
	}
	float resp_tim_for_task(int task_no){
		float anplusone;
		float an;
		float azero=get_azero(task_no);
		an=azero;
		do{
			float temp=0;
			for(int j=0;j<numberoftasks;j++){
				if(tasks[j].slack_time<=tasks[task_no].slack_time &&(j!=task_no)){	//to include only high priority tasks
					float temp2=an/tasks[j].period;
					temp=temp+ceil(temp2)*tasks[j].wcet;
				}
			}
			anplusone= tasks[task_no].wcet + temp;
			if(anplusone==an)	// termination condition
				break;
			an=anplusone;
		}while(1);
		tasks[task_no].resp_time=an;	//sets resp time of tha task in this task set
	return an;
	}
	bool resp_analysis(){
		for(int i=0;i<numberoftasks;i++){
			float resp_tim=resp_tim_for_task(i);	//get response time for the task.
			if(tasks[i].resp_time>tasks[i].deadline){
				return false;
			}
		}
		return true;
	}
};


int main(int argc,char **argv){
ifstream fp;
ofstream lstfile[6],rmfile[6],dmfile[6];
char tline[100];
time_t t;
srand((unsigned) time(&t));
/*if (argc!=3){
cout << "format : a.out <name of parameter file>" << endl; //if improper number of parameters
	return -1;
}*/
rmfile[0].open("rm_plot1.txt",ofstream::out);
rmfile[1].open("rm_plot2.txt",ofstream::out);
rmfile[2].open("rm_plot3.txt",ofstream::out);
rmfile[3].open("rm_plot4.txt",ofstream::out);
rmfile[4].open("rm_plot5.txt",ofstream::out);
rmfile[5].open("rm_plot6.txt",ofstream::out);
dmfile[0].open("dm_plot1.txt",ofstream::out);
dmfile[1].open("dm_plot2.txt",ofstream::out);
dmfile[2].open("dm_plot3.txt",ofstream::out);
dmfile[3].open("dm_plot4.txt",ofstream::out);
dmfile[4].open("dm_plot5.txt",ofstream::out);
dmfile[5].open("dm_plot6.txt",ofstream::out);
lstfile[0].open("lst_plot1.txt",ofstream::out);
lstfile[1].open("lst_plot2.txt",ofstream::out);
lstfile[2].open("lst_plot3.txt",ofstream::out);
lstfile[3].open("lst_plot4.txt",ofstream::out);
lstfile[4].open("lst_plot5.txt",ofstream::out);
lstfile[5].open("lst_plot6.txt",ofstream::out);
try{	//if the file is not present
	int nop;	//variable to get number of processes.
	char junk[100];
	fp.open(argv[1],ifstream::in);
		fp>>nop;
		fp.ignore(256,'\n');	// moves file pointer to end of line
		int file_no=0;
	for(int type=0;type<2;type++){
		for(int i=1;(i<=4);i*=2){
			cout<<file_no << " is the file number "<<endl;
			int jobs=i*12;
			synthetic_taskset set=synthetic_taskset(jobs);	// generates jobs number of synthetic task.
			rm rm_analysis=rm(jobs);	
			dm dm_analysis=dm(jobs);	
			LST lst_analysis=LST(jobs);	
			for(int j=0;j<10;j++){		//for different set of 
				float util=j*0.1 + 0.05;
				cout<<util<<" "<<i<<endl;
				int rmcount=0;
				int dmcount=0;
				int lstcount=0;
				for(int z=0;z<10000;z++){	//for generating 10000 times
					set.Uunifast(jobs,util,3,type);		// for 12 tasks and total utilization 1
					rm_analysis=rm(jobs);
					dm_analysis=dm(jobs);
					lst_analysis=LST(jobs);
					struct task_attr *taskset=(struct task_attr*)malloc(jobs*sizeof(struct task_attr));
					set.get_taskset(taskset);
					rm_analysis.set_taskset(taskset);
					dm_analysis.set_taskset(taskset);
					lst_analysis.set_taskset(taskset);
					for(int t=0;(t<rm_analysis.get_no_tasks());t++){
						/*cout<<"rm -- taskset : "<<i<<" task number : "<<t+1<<" schedulability res : " <<*/rm_analysis.is_bounded(t)/*<<endl*/;
						/*cout<<"dm -- taskset : "<<i<<" task number : "<<t+1<<" schedulability res : " <<*/dm_analysis.is_bounded(t)/*<<endl*/;
						/*cout<<"LST-- taskset : "<<i<<" task number : "<<t+1<<" schedulability res : "<<*/lst_analysis.is_bounded(t)/*<<endl*/;
					}
					if(rm_analysis.taskset_schedulable()){
						rmcount++;
					}
					if(dm_analysis.taskset_schedulable()){
						dmcount++;
					}
					if(lst_analysis.taskset_schedulable()){
						lstcount++;
					}
				}
				
					rmfile[file_no]<<util<<" : "<<rmcount<<endl;
					dmfile[file_no]<<util<<" : "<<dmcount<<endl;
					lstfile[file_no]<<util<<" : "<<lstcount<<endl;
			}
			file_no++;
		}
	}
}
catch(...){
cout<<"exception caught"<<endl;
}
fp.close();
return 0;
}
