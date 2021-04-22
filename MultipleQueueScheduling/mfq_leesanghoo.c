#include <stdio.h>
#include <stdlib.h>

//  Process �� ����ü�� ����  
typedef struct Process {          
    int PID;                      // identification number of the process
    int queue;                    // initial queue
    int at;                    // arrival time
    int n_cycle;				// �Է¹��� ����Ŭ  
    int burst_arr_index;			// burst arr �� ���ʷ� �����ϱ� ���� index  
    int sum_burst_time;			// burst time �� ����  
    int * burst_arr;               // int array for storing burst times
} Process;

// Ready Queue �� ����  
// linked list �� Ȱ���� priority queue  
typedef struct RQ {
    struct RQ* next;   
    Process* data;      
} RQ;



Process    **  job_queue;           // processes before arriving ready queue
RQ       *   ready_queue0;      	// Q0, RR(time quantum = 2)
RQ       *   ready_queue1;      	// Q1, RR(time quantum = 4)
RQ    *  ready_queue2;        		// Q2, RR(time quantum = 8)
RQ       *   ready_queue3;       	// Q3, FCFS
Process    **  sleep_queue;         // processes requesting I/O system call
int        **  process_table;       // burst time ����� ���̺� 
int            process_num;			// ���μ��� ����  

int global_time = 0; //while ���� ����ʿ� ���� 1�� ����  
int time_quantum; // RR �����ٸ� ����� ���ԵǴ� TIME QUANTUM  



// func
// �ʱ� �Է°��� ready queue �� �־��ִ� �Լ�  
int push_queue(Process* process);

// �߰����� process �� �ִ���, job queue �� �̿��� üũ  
int arrival_check(void);

// burst time �� ���Ҵ��� Ȯ���ϴ� �Լ� 
int burst_check(Process* process, int queue, int PID);

// fcfs �����ٸ�  
Process *fcfs(int type);

// burst time �� ��� ������ ���μ��� ����  
void delete_process(Process* process, int global_time);

// �Է¹��� ������ queue �� �־��ִ� �Լ�  
Process * init_process(int PID, int queue , int at, int n_cycle); 

// CHART ���  
void print_table(void); 


int main(int argc, char *argv[]) {
	Process* new_process;
	int i, j, k;
    int pid, init_q, at, cycle, size_arr, burst_value;
	
	
	
    Process *current_process;	// ����  �������� ���μ���  
    int current_process_id; 	
    int current_queue;
    int prev_process_id;
    int remain_process;
    int line;
    int arrival_result;
    int break_check;
    int burst_time;
	
	FILE *file;	
	
	file = fopen("input.txt", "r");
	if(file == NULL){
		printf("error 0 : check your file path. \n");
		return 0;
	}
	
	fscanf(file, "%d", &process_num);
//	printf("process _ num  = %d \n", process_num);
	if(process_num < 0){
		printf("error 1 : process number should be positive. \n");
		return 0;
	}
	
	
	// ����� ť�� �����Ҵ� �Ѵ�.  
	job_queue = (Process**)malloc(sizeof(Process) * process_num);
	
	ready_queue0 = (RQ*)malloc(sizeof(RQ));
	ready_queue0->data = NULL;
    ready_queue0->next = NULL;
	
	
	ready_queue1 = (RQ*)malloc(sizeof(RQ));
	ready_queue1->data = NULL;
    ready_queue1->next = NULL;
	
	
	ready_queue2 = (RQ*)malloc(sizeof(RQ));
	ready_queue2->data = NULL;
    ready_queue2->next = NULL;
    
    ready_queue3 = (RQ*)malloc(sizeof(RQ));
    ready_queue3->data = NULL;
    ready_queue3->next = NULL;
    
    sleep_queue = (Process**)malloc(sizeof(Process) * process_num);
    for (i = 0; i < process_num; i++) {
        sleep_queue[i] = NULL;
    }    
    
    process_table = (int**)malloc(sizeof(int*) * process_num);
    for (i = 0; i < process_num; i++) 
	process_table[i] = (int*)malloc(sizeof(int) * 2);
    
    
    
    // �о�� ������ �����Ҵ��� ť�鿡 ����ֱ�
    for (i = 0; i < process_num; i++) {
        int total = 0;
        burst_value = 0;
        
		fscanf(file, "%d %d %d %d", &pid, &at, &init_q, &cycle);
        
        size_arr = (cycle * 2) - 1;        
		
		// �Է¹��� ������ ���� process �ʱ�ȭ �Լ�  
        new_process = init_process(pid, init_q, at, cycle);
        
        // burst arr ����Ŭ�� ���� �迭  ������ ��ŭ �Է� �ޱ�  
        for (j = 0; j < size_arr; j++) {
            if(fscanf(file, "%d", &burst_value) == -1){
            	
            	printf("\nerror 3 : burst arr must follow cycle number constraints. \n");
				return 0 ;
			}
            
            total += burst_value;
            new_process->burst_arr[j] = burst_value;
        	
        }    
        new_process->sum_burst_time = total;
        job_queue[i] = new_process;
    }
	
	
	// �ʱ�ȭ  
    current_process = NULL;
    current_process_id = 0;
    current_queue = -1;
    prev_process_id = -1;
    remain_process = 1;
    line = 0;

    printf("  Process ID\n");
    
    
    // while ���ѷ����� ���� �Ҵ�Ǵ� ���μ����� �����Ѵ�.  
    while (1) {
    	
    	// 1. global time �� �� ���μ����� at �� arrival_check �Լ��� ���Ͽ� ���Ͽ�
		// �߰������� �Ҵ��ؾ��ϴ� ���μ����� ������ Ȯ���Ѵ�.  
    	if (remain_process == 1) {
            arrival_result = arrival_check();
            if (arrival_result == 0) {
                remain_process = 0;
            }
        }
        
        
        // 2. I/O �� ��û�� ���μ����� ������� Ȯ���ϰ�
        // ready queue �� �־��ش�.  
		for (i = 0; i < process_num; i++) {
        	if (sleep_queue[i] != NULL) {
        	
        		// sleep queue [i] �� ����� burst �ð� ���� ��������  
				int time = sleep_queue[i]->burst_arr[sleep_queue[i]->burst_arr_index];
            	if (time == 0) {
            	
            		// ��� ���� �� ���  
            		// cycle index �� ������ ���� burst time �� Ž���Ѵ�.  
              	  sleep_queue[i]->burst_arr_index += 1;
                
               		// queue �� �켱������ ���� �����Ѵ�.					    
					// ���� 0 ready queue �� �ִ� ��� �״�� �д�.  
					// ���� 3 ready queue �� �ִ� ��쵵 �״�� �д�.  
                	int queue = sleep_queue[i]->queue;
                	if( queue != 3) queue = queue > 0 ? queue - 1 : 0;
                	
                
                	// ���ŵ� queue ���� sleep_queue �� �� process �� ���� 
					sleep_queue[i]->queue = queue;
                	push_queue(sleep_queue[i]); 
                	sleep_queue[i] = NULL;
            	}
        	}
    	}
		
		
        // 3. ���డ���� ���μ����� Ȯ��   
        // current_process �� ���� ���� ��� �����ٸ��� �����Ѵ�.  
        if (current_process == NULL) {  
		        
			Process * tmp; 
			
			// 4���� ť�� �켱������� ���鼭 
			// �����ٸ� �� �� �ִ� ť�� ã�Ƽ� fcfs  �� �����Ѵ�. 
			// queue 0, 1, 2 �� ��� time quantum �� �����ϴ� RR ����̸�.  
			// queue 3 �� ��� time FCFS ����̴�.  
			for(k=0; k<4; k++){
				tmp = fcfs(k);
				if(tmp != NULL){
					if(k != 3) time_quantum = 2*(k+1);
					else time_quantum = -1;
					
					current_process = tmp;
					break;
				}
			}            
            
            // �������� �õ��������� ������ ��� 
			 
            // Fail to schedule
            if (current_process == NULL) {
                
				// break �� ��Ȳ���� üũ�Ѵ�. 
				// break : 1 / not break : 0 �̴�.  
				break_check = 0;                
                for(k =0; k<process_num; k++){
                	if(sleep_queue[k] != NULL) break_check =1;
				}			
                
                
                // Finish if all processes are terminated
                // break ���� ���μ����� ����, 
				// ���� �������� ���� ���μ����� ���ٸ�, �����Ѵ�.  
                if (remain_process == 0 && break_check == 0) {
                    for(i=0; i<line; i++) {
                        printf("|      | \n");
                    }
                    
                    line = 0;
                    printf("|  %d   | \n", prev_process_id);
                    printf("+-------------- %d\n\n", global_time);
                    break;
                }
                
                
                // Wait if the I/O requesting process remains
                current_process_id = 0;
                current_queue = -1;
            } else {
                current_process_id = current_process->PID;
                current_queue = current_process->queue;
                
            }
        }
        
        
        // ���� ���� �������� ���μ����� �ִٸ�, (burst ���̴�.)
		// ���ο� ���μ�������, ������ ���μ����� ������ �����Ѵ�.    
        // 3-2. Run an existing process
        if(prev_process_id != current_process_id) {
            
            for(i=0; i<line; i++) {
                printf("|      | \n");
            }
            line = 0;
            if(prev_process_id == 0) {
                printf("| .... | \n");
            } 
            printf("+--------------clock--- %d\n", global_time);
            
            
            if(current_process_id == 0) {
                printf("| .... | \n");
            } 
			else {
				printf("|  %d   | \n", current_process_id);
            }                        
            prev_process_id = current_process_id;
            
        } else {
            line++;
        }
        
         
        // 4. Increase global time by 1
        // while �ݺ��� �ȿ��� �����ϸ�, global time �� 1 ����
		// �����ٸ� �ÿ� ���� time quantum ���� 1 �Ҹ� �Ѵ�. 
		 
        global_time += 1;
        time_quantum -= 1;
        for (i = 0; i < process_num; i++) {
            if (sleep_queue[i] != NULL) {
                int index = sleep_queue[i]->burst_arr_index;
                sleep_queue[i]->burst_arr[index] -= 1;
            }
        }
        
    
        // current_process �� NULL �� ���, �� ��� ���μ����� cpu burst �� �Ҹ�ǰ�
		// ���� io burst �� ���� ��Ȳ�� ���Ѵ�. 
		// �� ��� while �� ó������ ���ư���.  
        if( current_process == NULL) continue;
        
        // 5. Consume allocated time quantum 1        
        // �����ִ� cpu burst �� ����Ѵ�. 
		
		
		// �ش� index ��ġ��  burst_arr ���� burst time �� ��� �Ҹ��� ��� 
		// case 1. process burst time �� ��� �Ҹ��� ���  return -1
		// case 2. cpu burst �� �� �Ҹ��ؼ� io burst �� ��û�� ��� return 0  
		// case 3. ���� cpu burst time �� �����ִ� ��� return 1  
        int check = burst_check(current_process, current_queue, current_process_id);
        
        
        // case 1, 2
        if (check != 1) {
            current_process = NULL;
            current_process_id = 0;
            current_queue = -1;
        } 
        
        // case 3 
		else {
            if (time_quantum == 0) {
            	// 0,1,2 ready queue �� �ִ� ���, �켱������ �ϳ� ����߸���. 
                current_queue = current_queue < 3 ? current_queue + 1 : 3 ;
                current_process->queue = current_queue;
				                
                if (push_queue(current_process) == -1)
                    return (-1);
                    
                current_process = NULL;
                current_process_id = 0;
                current_queue = -1;
            }
        }
    }   
    
    fclose(file); 
    
	
	free(job_queue);
    free(ready_queue0);
    free(ready_queue1);
    free(ready_queue2);
    free(ready_queue3);
    free(sleep_queue);
    free(process_table);
    
    
    print_table();
    
    return (0);
}


// allocate memory for each process and initiate
Process* init_process(int _PID, int _queue, int _at, int _n_cycle) {
    int arr_size;

    Process* new_process = (Process*)malloc(sizeof(Process));
    if (new_process == NULL)
        return (new_process);
    new_process->PID = _PID;
    new_process->queue = _queue;
    new_process->at = _at;
    new_process->n_cycle = _n_cycle;
    new_process->burst_arr_index = 0;
    new_process->sum_burst_time = 0;
    arr_size = (_n_cycle * 2) - 1;
    new_process->burst_arr = (int*)malloc(sizeof(int) * arr_size);

    return (new_process);
}

void print_table(void) {
	int i;
    double AverageTT = 0;
    double AverageWT = 0;
    printf("process table:\n");
    printf("+-------+-------------------+----------------+\n");
    printf("|  PID  |  Turnaround Time  |  Waiting Time  |\n");
    printf("+-------+-------------------+----------------+\n");
    for(i=0; i<process_num; i++) {
        AverageTT += process_table[i][0];
        AverageWT += process_table[i][1];
        printf("| %5d | %17d | %14d |\n", i+1, process_table[i][0], process_table[i][1]);
    }
    printf("+-------+-------------------+----------------+\n");
    printf("\n");
    AverageTT /= process_num;
    AverageWT /= process_num;
    printf("Average Turnaround Time : %.2lf\nAverage Wating Time     : %.2lf\n", AverageTT, AverageWT);
}




// push the process proper ready queue
int push_queue(Process* process) {
    RQ* head;
    RQ* rq;
    int queue_num;
    int pid;

    queue_num = process->queue;
     
	pid = process->PID - 1;
    switch (queue_num) {
    case 0:
        head = ready_queue0;
        break;
    case 1:
        head = ready_queue1;
        break;
    case 2:
        head = ready_queue2;
        break;
    case 3:
        head = ready_queue3;
        break;
    default:
        break;
    }
    
    rq = (RQ*)malloc(sizeof(RQ));
    rq->data = process;
    rq->next = NULL;
    while (head->next != NULL) {
        head = head->next;
    }
    head->next = rq;
    return (0);
}

int arrival_check(void) {
    int result, i;

    result = 0;
    for (i = 0; i < process_num; i++) {
        if (job_queue[i] != NULL) {
            if (job_queue[i]->at == global_time) {
                push_queue(job_queue[i]);
                job_queue[i] = NULL;
            } else {
                result = 1;
            }
        }
    }
    return (result);
}

int burst_check(Process* process, int queue, int PID) {
    int result;
    int remain;
    int n_cycle;
    int index;
    int arr_size;
    
    remain = process->burst_arr[process->burst_arr_index] - 1 ;
    process->burst_arr[process->burst_arr_index] = remain;
    
    // �ش� index ��ġ��  burst_arr ���� burst time �� ��� �Ҹ��� ��� 
	// case 1. process burst time �� ��� �Ҹ��� ���  return -1
	// case 2. cpu burst �� �� �Ҹ��ؼ� io burst �� ��û�� ��� return 0  
    if (remain == 0) {
        n_cycle = process->n_cycle;
        index = process->burst_arr_index;
        arr_size = (n_cycle * 2) - 1;
        index += 1;
        process->burst_arr_index = index;
        if (arr_size == index) {
            delete_process(process, global_time);
            result = -1;
            return result;
        }
        result = 0;
        PID -= 1;
        sleep_queue[PID] = process;
        return result; 
    }
    
    // ���� burst time �� �����ִ� ���  
    result = 1;
    return result;
}

// schedule a process from receiving ready queue (= type)
// if there's nothing to schedule, then return (NULL)
// else, return scheduled process pointer

Process* fcfs(int type) {
    Process* result;
    RQ* head;
    RQ* remove;

    switch (type) {
    case 0:
        head = ready_queue0;        
        break;
    case 1:
        head = ready_queue1;
        break;
    case 2:
    	head = ready_queue2;
    	break;
    case 3:
        head = ready_queue3;
        break;
    default:
        break;
    }
        
    if (head == NULL || head->next == NULL) {
        return (NULL);
    } 
    
    remove = head->next;
    result = remove->data;
    head->next = remove->next;
    free(remove);
    return (result);
}


void delete_process(Process* process, int global_time) {
    int pid;
    int arrival_time;
    int total_burst_time;
    int TT;
	
    pid = process->PID - 1;
    arrival_time = process->at;
    total_burst_time = process->sum_burst_time;
    TT = global_time - arrival_time;
    
    process_table[pid][0] = TT;
    process_table[pid][1] = TT - total_burst_time;
    
	free(process->burst_arr);
    free(process);
    return;
}

