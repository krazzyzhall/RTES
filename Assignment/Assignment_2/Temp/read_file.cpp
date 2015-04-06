#include<fstream>	// for io ops of file
#include<cstring>	// for memcpy
#include<cstdlib>
#include<math.h>
#include<iostream>
using namespace std;
struct task_attr{
	float wcet;
	float deadline;
	float period;
	float resp_time;	// worst case response time at critical instant.
	float slack_time;	// saves value of worst case execution including wcet of higher processes.
	bool is_sched;	// after response time analusis
};

class rm{
private:
	struct task_attr *tasks;
	int numberoftasks;
	float util;
public:
	int get_no_tasks(){			// return number of tasks 
		return numberoftasks;
	}
	void get_tasks(struct task_attr &);	//for getting the list of all the tasks
	rm(int no_task){
		tasks=(struct task_attr*)malloc(no_task*sizeof(struct task_attr));
		numberoftasks=no_task;
	}
	void set_tasks(struct task_attr task,int task_num){
		memcpy(&tasks[task_num],&task,sizeof(struct task_attr));	//for copying to task info in native structure
//		cout<<"rm : "<< tasks[task_num].wcet <<" "<< tasks[task_num].deadline <<" " << tasks[task_num].period <<endl;

	}
	float calc_util_bound(int no_tasks){
		util=no_tasks*(pow(2,1.0/no_tasks)-1);
//		cout<< numberoftasks<<" util :"<<util <<endl;
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
//			cout<<an;
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
//				cout<<" task number : "<< task_no << " is not schedulable" <<endl;
				return 0;
			}
			else if(util_by_tasks<=tutil){
				tasks[task_no].is_sched=true;
//				cout<<" task number : "<< task_no << " is schedulable" <<endl;
				return 1;
			}
			else{
				cout<<"rm -- utilization based analysis inconclusive, hence response based analysis :"<<endl;
//				cout<<" dont know if task number : "<< task_no << " is schedulable" <<endl;
				int resp_tim=resp_tim_for_task(task_no);		// get resp of task_no in presence of higher priority tasks
				return tasks[task_no].is_sched=resp_tim<tasks[task_no].deadline;		// if response time of task is less than deadline then schedulable
			}
	}
};
class dm{
private:
	struct task_attr *tasks;
	int numberoftasks;
public:	
	int get_no_tasks(){			// return number of tasks 
		return numberoftasks;
	}
	void get_tasks(struct task_attr &);	//for getting the list of all the tasks
	dm(int no_task){
		tasks=(struct task_attr*)malloc(no_task*sizeof(struct task_attr));
		numberoftasks=no_task;
	}
	void set_tasks(struct task_attr task,int task_num){
		memcpy(&tasks[task_num],&task,sizeof(struct task_attr));	//for copying to task info in native structure
//		cout<<"dm : "<< tasks[task_num].wcet <<" "<< tasks[task_num].deadline <<" " << tasks[task_num].period <<endl;
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
			
				util_by_tasks+=(tasks[i].wcet/din);
			}
		}
			if(util_by_tasks>=1){		// if density is more than one it is supposed to do time response analysis
				cout<<"dm -- utilization based analysis inconclusive, hence response based analysis :"<<endl;
				int resp_tim=resp_tim_for_task(task_no);		// get resp of task_no in presence of higher priority tasks
				return tasks[task_no].is_sched=resp_tim<tasks[task_no].deadline;		// if response time of task is less than deadline then schedulable
//				cout<<" task number : "<< task_no << " is not schedulable" <<endl;
			}
			else if(util_by_tasks<1){
				tasks[task_no].is_sched=true;
//				cout<<" task number : "<< task_no << " is schedulable" <<endl;
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
//			cout<<an;
		}while(1);
		tasks[task_no].resp_time=an;	//sets resp time of tha task in this task set
	return an;
	}
	bool resp_analysis(){
		for(int i=0;i<numberoftasks;i++){
			float resp_tim=resp_tim_for_task(i);	//get response time for the task.
			if(tasks[i].resp_time>tasks[i].deadline){
			//	cout<<resp_tim<<" "<<i<< endl;
				return false;
			}
//				cout<<resp_tim<<" "<<i<< endl;
		}
		return true;
	}
//function named analyze for anylizing utilization  
};
class LST{
private:
	struct task_attr *tasks;
	int numberoftasks;
public:
	int get_no_tasks(){			// return number of tasks 
		return numberoftasks;
	}
	void get_tasks(struct task_attr &);	//for getting the list of all the tasks
	LST(int no_task){
		tasks=(struct task_attr*)malloc(no_task*sizeof(struct task_attr));
		numberoftasks=no_task;
	}
	void set_tasks(struct task_attr task,int task_num){
		memcpy(&tasks[task_num],&task,sizeof(struct task_attr));	//for copying to task info in native structure
		tasks[task_num].slack_time=tasks[task_num].deadline - tasks[task_num].wcet;	// get slack
//		cout<<"dm : "<< tasks[task_num].wcet <<" "<< tasks[task_num].deadline <<" " << tasks[task_num].period <<endl;
	}
/*	void slack_tim_for_task(int task_no){
		for(int i=0;i<numberoftasks;i++){
			tasks[i].slack_time=tasks[i].deadline - tasks[i].wcet; //calculate least slack time
		}
	}*/
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
	//		slack_tim_of_task(task_no);	//to get slack time of task "task_no"
		for(int i=0;i<numberoftasks;i++){
	//		slack_tim_of_task(i);		// to get slack time of every other task
			if(tasks[task_no].slack_time>=tasks[i].slack_time){	// check all the higher priority tasks.
				count++;
				din=tasks[i].period;
				if(tasks[i].deadline < tasks[i].period){
					din=tasks[i].deadline;
				}
			
				util_by_tasks+=(tasks[i].wcet/din);
			}
		}
			if(util_by_tasks>=1){		// if density is more than one it is supposed to do time response analysis
				cout<<"LST -- utilization based analysis inconclusive, hence response based analysis :"<<endl;
				int resp_tim=resp_tim_for_task(task_no);		// get resp of task_no in presence of higher priority tasks
				return tasks[task_no].is_sched=resp_tim<tasks[task_no].deadline;		// if response time of task is less than deadline then schedulable
//				cout<<" task number : "<< task_no << " is not schedulable" <<endl;
			}
			else if(util_by_tasks<1){
				tasks[task_no].is_sched=true;
//				cout<<" task number : "<< task_no << " is schedulable" <<endl;
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
//			cout<<an;
		}while(1);
		tasks[task_no].resp_time=an;	//sets resp time of tha task in this task set
	return an;
	}
	bool resp_analysis(){
		for(int i=0;i<numberoftasks;i++){
			float resp_tim=resp_tim_for_task(i);	//get response time for the task.
			if(tasks[i].resp_time>tasks[i].deadline){
			//	cout<<resp_tim<<" "<<i<< endl;
				return false;
			}
				cout<<resp_tim<<" "<<i<< endl;
		}
		return true;
	}
};

//function named analyze for anylizing utilization  

int main(int argc,char **argv){
ifstream fp;
char tline[100];
if (argc!=2){
cout << "format : a.out <name of parameter file>" << endl; //if improper number of parameters
	return -1;
}
try{	//if the file is not present
	int nop;	//variable to get number of processes.
	char junk[100];
	fp.open(argv[1],ifstream::in);
		fp>>nop;
		fp.ignore(256,'\n');	// moves file pointer to end of line
	for(int i=0;(i<nop);i++){
		int jobs;
		fp>>jobs;	//reads number of jobs for tha process.
		fp.ignore(256,'\n');
		rm rm_analysis=rm(jobs);	
		dm dm_analysis=dm(jobs);	
		LST lst_analysis=LST(jobs);	
		for(int j=0;(j<jobs);j++){
			int wcet,deadline,period;	//variable to get job attributes
			struct task_attr task;
			fp>>wcet>>deadline>>period;		
			fp.ignore(256,'\n');
			task.wcet=wcet;
			task.deadline=deadline;
			task.period=period;
			rm_analysis.set_tasks(task,j);		// sets task to rm object
			dm_analysis.set_tasks(task,j);		// sets task to dm object
			lst_analysis.set_tasks(task,j);		// sets task to dm object
//		cout<< task.wcet <<" "<< task.deadline <<" " << task.period <<endl;
		}
	for(int t=0;(t<rm_analysis.get_no_tasks());t++){
//		cout<< rm_analysis.get_no_tasks() <<" "<< "haha "<<" " << "haha " <<endl;
			cout<<"rm -- taskset : "<<i<<" task number : "<<t+1<<" schedulability res : " <<rm_analysis.is_bounded(t)<<endl;
			cout<<"dm -- taskset : "<<i<<" task number : "<<t+1<<" schedulability res : " <<dm_analysis.is_bounded(t)<<endl;
			cout<<"LST-- taskset : "<<i<<" task number : "<<t+1<<" schedulability res : "<<lst_analysis.is_bounded(t)<<endl;
	}
			cout<<"rm -- schedulability res for taskset-"<<i<<" :  " <<rm_analysis.taskset_schedulable()<<endl;
			cout<<"dm -- schedulability res for taskset-"<<i<<" :  " <<dm_analysis.taskset_schedulable()<<endl;
			cout<<"LST-- schedulability res for taskset-"<<i<<" :  "<<lst_analysis.taskset_schedulable()<<endl;
//			cout<<rm_analysis.is_bounded();
	}
}
catch(...){
cout<<"exception caught"<<endl;
}
fp.close();
return 0;
}
