#include "project2.h"
#include "Bank.h"


int external_id = 1;

int num_threads;
int num_accounts;
FILE* output;

pthread_mutex_t* account_locks;
pthread_mutex_t q_lock;
pthread_t* worker_threads;



queue_t* q; 

int main(int argc, char* argv[]){
	num_threads = atoi(argv[1]);
	num_accounts = atoi(argv[2]);
	char* filename  = argv[3];

	output = fopen(filename, "w");
	worker_threads = malloc(num_threads * sizeof(pthread_t));
	account_locks = malloc(num_accounts * sizeof(pthread_mutex_t));
	int i;
	initialize_accounts(num_accounts);
	q = createQueue();
	for(i = 0; i < num_threads; i++){
		pthread_create(&worker_threads[i], NULL, work, NULL);
	}
	for(i = 0; i < num_accounts; i++){
		pthread_mutex_init(&account_locks[i], NULL);
	}
	pthread_mutex_init(&q_lock, NULL);
	write_account(0, 1000);
	write_account(2, 3000);
	write_account(4, 7000);
	write_account(5, 8000);
	write_account(8, 45500);
	write_account(1, 1022300);
	char request_string[100];
	while(1){
		fgets(request_string, 100, stdin);
		int valid = check_valid(request_string);
		if(valid){
			printf("ID: %d\n", external_id);
			request_t new_req;
			new_req.request_id = external_id;
			external_id++;
			gettimeofday(&new_req.start_time, NULL);
			enQueue(q, &new_req, request_string);
		}else{
			fclose(output);
		}
	}

	//Threads should pop the queue. Get them damn locks. Gottem!


	return 0;
}

void* work(){
	while(1){
		pthread_mutex_lock(&q_lock);
		qNode_t gottem = deQueue(q);
		pthread_mutex_unlock(&q_lock);
		if(gottem.request->num_trans != -1){
			parse_transaction(gottem.request);
			process_transaction(gottem.request);
		}
			//printf("ID: %d CHECK ACC NUM: %d STARTTIM: %ld.%06.ld\n", gottem.request->request_id, gottem.request->check_acc_id, gottem.request->start_time.tv_sec, gottem.request->start_time.tv_usec);
		
		
	}
}

void parse_transaction(request_t* request){
	request_t new_request;
	char copytrans[100];
	strcpy(copytrans, request->request_string);
	char* token = strtok(copytrans, " ");
	if(strcmp(token, "CHECK") == 0){
		token = strtok(NULL, " ");
		request->check_acc_id = atoi(token);
		request->num_trans = 0; 
	}else if(strcmp(token, "TRANS") == 0){
		token = strtok(NULL, " ");
		int tran_count = 0;
		int flop = 0;
		while(token != NULL){
			if(flop == 0){
				tran_count++;
				flop = 1;
			}else{
				flop = 0;
			}
			token = strtok(NULL, " ");
		}
		request->transactions = malloc(sizeof(trans_t) * tran_count);
		request->num_trans = tran_count;
		char copy2[100];
		strcpy(copy2, request->request_string);
		int i;
		token = strtok(copy2, " ");
		token = strtok(NULL, " ");
		for(i = 0; i < tran_count; i++){
			request->transactions[i].acc_id = atoi(token);
			token = strtok(NULL, " ");
			request->transactions[i].amount = atoi(token);
			token = strtok(NULL, " ");
		}
	}else{

	}
}

int check_valid(char* transaction){
	char copytrans[100];
	strcpy(copytrans, transaction);
	char* token = strtok(copytrans, " ");
	if(strcmp(token, "CHECK") == 0 || strcmp(token, "TRANS") == 0){
		return 1;
	}
	return 0;
}

void process_transaction(request_t* request){
	if(request->num_trans == 0){
		pthread_mutex_lock(&account_locks[request->check_acc_id]);
		int balance_result = read_account(request->check_acc_id);
		//sleep(2);
		pthread_mutex_unlock(&account_locks[request->check_acc_id]);
		gettimeofday(&request->end_time, NULL);
		printf("BAL %d TIME %ld.%06.ld %ld.%06.ld\n", balance_result, request->start_time.tv_sec, request->start_time.tv_usec,request->end_time.tv_sec, request->end_time.tv_usec);
	}else{
		int locks_needed[request->num_trans];
		int i;
		for(i = 0; i < request->num_trans; i++){
			locks_needed[i] = request->transactions[i].acc_id;
		}
	}
}

//TO make transactions first get old value. Adjust. Write the new value. Write does not do any math.

	//For fine grained code we should obtain a lock for each and every account. For coarse grained code lock the whole damn bank.

	//Mutexes need to be gained in order??
		//How do we avoid deadlock

			//Reccomeneded to have some semantic rhym or reason to locking. Maybe we should get all locks then switch
				//Was mentuoned somewhere*(mayeb in lab document) that we should get lowest then continue. So always try to lock the lowest in the transaction
					//What does this even prevent. 
						//Imagine we are adjusting 1 and 2 in one trans
						//And in another we are adjuting 3 and 1.
							//Now we have a problem since 3 is gotten when 1 cant be which leads to many problems. Or maybe it doesnt idk
