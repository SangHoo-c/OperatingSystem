#include<stdio.h>
#include<stdlib.h>



int main(){
	int i, j, np, nr, count_blocked, dead_lock_check;
	int dl_check_num;
	int **alloc;
	int **req;
	int *res_init;
	int *res_avail;
	int row, col;
	int check_error;	
	
	FILE *file;	
	
	file = fopen("input.txt", "r");
	if(file == NULL){
		printf("error 0 : check your file path. \n");
		return 0;
	}
	
	fscanf(file, "%d %d", &np, &nr);
	
	// �Ҵ��Ϸ��� �迭�� ��, �� ����  
	row = np + 1;
	col = nr + 1;	
	
	alloc = (int**) malloc(sizeof(int*) * row);
	if(alloc == NULL){
		printf("error -1 : check your memory. \n");
		return -1;
	}
	for(i=0; i<row; i++) alloc[i] = (int*) malloc(sizeof(int) * col);

	req = (int**) malloc(sizeof(int*) * row);
	if(req == NULL){
		printf("error -1 : check your memory. \n");
		return -1;
	}
	for(i=0; i<row; i++) req[i] = (int*) malloc(sizeof(int) * col);
	
	res_init = (int*) malloc(sizeof(int) * col);
	res_avail = (int*) malloc(sizeof(int) * col);
	
	if(res_init == NULL){
		printf("error -1 : check your memory. \n");
		return -1;
	}
	if(res_avail == NULL){
		printf("error -1 : check your memory. \n");
		return -1;
	}
	 
	check_error = 0;
	// res ���ֿ� ���� ���� �Է�  
	for(i=0; i<nr; i++){
		fscanf(file, "%d", &res_init[i]);
		check_error ++;
	}
	
	 
	for(i=0; i<np; i++){
		for(j=0; j<nr; j++){
			if(fscanf(file, "%d", &alloc[i][j]) == -1){
				printf("error 1 : check your input format. there must be missing values. \n");
				return 1;
			}
			check_error ++;
		}
	}
	
	for(i=0; i<np; i++){
		for(j=0; j<nr; j++){
			if(fscanf(file, "%d", &req[i][j]) == -1){
				printf("error 1 : check your input format. there must be missing values. \n");
				return 1;
			}
			check_error ++;
		}
	}
	
	int chk; 
	if(fscanf(file, "%d", &chk) == 1){
		printf("error 2 : check your input format. there must be overloaded values. \n");
		return 2;
	}
	
	
	while(1){
		
		// ����Ǿ� �ִ� p ���� ���ϱ� 
		// ���Ŀ� dl check �� ����� ���̴�.  
		dl_check_num = 0;
		int tmp; 
		for(i=0; i<np; i++){
			tmp = 0;
			for(j=0; j<nr; j++){
				if(alloc[i][j] == 0 && req[i][j] == 0) tmp ++;
			}
			if(tmp != 0) dl_check_num ++;
		}
			
		// res_avail �迭 �ʱ�ȭ 
		for(j=0; j<nr; j++ ) res_avail[j] = 0;
		
		// avail �迭�� �Ҵ�  
		for(j=0; j<nr; j++){
			res_avail[j] = res_init[j];
			for(i=0; i<np; i++){
				res_avail[j] -= alloc[i][j];
			}
		}
		
		// ����� ���� �ڵ�  
		dead_lock_check = 0;
		// process ���� ���鼭 checking 
		for(i=0; i<np; i++){
			count_blocked = 0;
			for(j=0; j<nr; j++){
				if(req[i][j] <= res_avail[j]){
					continue;
				}
				else{
					count_blocked ++;
				}
			}
		
			// i ��° process �� ��� edge ���� unblocked ���¶��
			// ��� ������ �����Ѵ�.  
			if(count_blocked == 0){
				
				// i ��° request method ��� 0  
				for(j=0; j<nr; j++){
					req[i][j] = 0;
				} 
				
				// i ��° allocation ���� ��� �ݳ��Ѵ�. 
				// res_avail �� ���� �÷��ش�. 
				for(j=0; j<nr; j++){
					if(alloc[i][j] != 0){
						res_avail[j] += alloc[i][j];
						alloc[i][j] = 0;
					}
				} 
			}
			
			// i ��° process �� �ϳ��� blocked edge �� �ִٸ�
			// dead_lock_check ++ �Ѵ�.  
			else{
				dead_lock_check ++;
			}		
		}
		
		
		// loop Ż�� ����
		// dead lock �� ����Ǹ� �����Ѵ�.  
		if(dead_lock_check == dl_check_num) break;
		
		// loop Ż�� ���� 2. 
		// ��� graph redcution �� ��ġ��
		// ��� edge �� ���� 0 �̸� �����Ѵ�. 
		int check_2 = 0;
		for(i=0; i<np; i++){
			for(j=0; j<nr; j++){
				check_2 += alloc[i][j];
			}
		}
		if(check_2 == 0) break;
	}
	
	// blocked ������ process �� ������ ��Ÿ���� dead_lock_check �� ��ü ���μ����� ������ ���ٸ�, 
	// dead lock �� ����� ���̴�.  
	// dead lock �� ����Ǵ� ���  
	
	
	if(dead_lock_check == dl_check_num) printf("\n\n deadlock detected! \n\n");
	else printf("\n\n all edges are removed. no deadlock detection! \n\n");
	
	
	printf(" --- deadlocked process list --- \n");
	
	int cnt; 
	for(i=0; i<np; i++){
		cnt = 0;
		for(j=0; j<nr; j++){
			cnt += alloc[i][j];
		}
		if(cnt != 0 ) printf(" p%d ", i+1);
	}
	printf("\n");
	printf(" ------------------------------- \n");
	
	printf(" \n current allocated state \n");
	for(i=0; i<np; i++){
		for(j=0; j<nr; j++){
			printf(" %d ", alloc[i][j]);
		}
		printf("\n");
	}
	
	printf("\n current req state \n");
	for(i=0; i<np; i++){
		for(j=0; j<nr; j++){
			printf(" %d ", req[i][j]);
		}
		printf("\n");
	}
	
	
	printf("\n current avail state \n");
	for(j=0; j<nr; j++) printf(" %d ", res_avail[j]);
	
	
	for(i=0; i<row; i++){
		free(alloc[i]);
		free(req[i]);
	}
	free(alloc);
	free(req);
	free(res_init);
	free(res_avail);
	
	fclose(file); 
	
	return 0;
}
