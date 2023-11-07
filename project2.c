#include "project2.h"
#include "Bank.h"


int external_id = 1;

int num_threads;
int num_accounts;
int master_while = 1;
FILE* output;

pthread_mutex_t* account_locks;
pthread_mutex_t q_lock;
pthread_t* worker_threads;



queue_t* q; 

int main(int argc, char* argv[]){
	//GEt inital arguements to initialize a lock for each accoutn and the threads to work function
	num_threads = atoi(argv[1]);
	num_accounts = atoi(argv[2]);
	char* filename  = argv[3];
	//Open the file and trunc content
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
	char request_string[1024];
	//Start getting input strings
	while(1){
		//Check if its actually a command
		fgets(request_string, 1024, stdin);
		int valid = check_valid(request_string);
		if(valid == 1){
			//If it is print ID and add to Q
			printf("ID: %d\n", external_id);
			request_t* new_req = malloc(sizeof(request_t));
			new_req->request_id = external_id;
			external_id++;
			gettimeofday(&new_req->start_time, NULL);
			enQueue(q, new_req, request_string);
		}else if(valid == -3){
			//IF end then we kill the threads
			master_while = 0;
			for(i = 0; i < num_threads; i++){
				pthread_join(worker_threads[i], NULL);
			}
			fclose(output);
			break;
		}
	}

	//Threads should pop the queue. Get them damn locks. Gottem!


	return 0;
}

void* work(){
	//Read the queue and decipher the results
	while(master_while){
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
	//Make a copy for strtok get the command name
	char copytrans[1024];
	strcpy(copytrans, request->request_string);
	char* token = strtok(copytrans, " ");
	if(strcmp(token, "CHECK") == 0){
		//If check next tok will be account ID setup the request object with this
		token = strtok(NULL, " ");
		request->check_acc_id = atoi(token);
		request->num_trans = 0; 
	}else if(strcmp(token, "TRANS") == 0){
		//IF trans we tok through it to get the count
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
		//Initialize and set variables based on thsi count
		request->transactions = malloc(sizeof(trans_t) * tran_count);
		request->num_trans = tran_count;
		//Another copy to get actual stuff
		char copy2[1024];
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
	}
}

int check_valid(char* transaction){
	//Checks if first strtok on a copy is a valid command
	char copytrans[1024];
	strcpy(copytrans, transaction);
	char* token = strtok(copytrans, " ");
	if(strcmp(token, "CHECK") == 0 || strcmp(token, "TRANS") == 0){
		return 1;
	}else if (strcmp(token, "END")){
		return -3;
	}
	return 0;
}

void process_transaction(request_t* request){
	//If we dont have 0 trans we are a check so do a check
	if(request->num_trans == 0){
		//Get the lock for the account read and unlock then output to file
		pthread_mutex_lock(&account_locks[request->check_acc_id]);
		int balance_result = read_account(request->check_acc_id);
		pthread_mutex_unlock(&account_locks[request->check_acc_id]);
		gettimeofday(&request->end_time, NULL);
		fprintf(output, "%d BAL %d TIME %ld.%06ld %ld.%06ld\n",request->request_id ,balance_result, request->start_time.tv_sec, request->start_time.tv_usec,request->end_time.tv_sec, request->end_time.tv_usec);

	}else{
		//Get the locks needed then sort them
		int locks_needed[request->num_trans];
		int original_states[request->num_trans];
		int i;
		for(i = 0; i < request->num_trans; i++){
			locks_needed[i] = request->transactions[i].acc_id;
		}
		qsort(locks_needed, request->num_trans, sizeof(int), compare);
		int transaction_index = 0;
		//Lock in order
		for(i = 0; i < request->num_trans; i++){
			pthread_mutex_lock(&account_locks[locks_needed[i]]);
		}
		int failure = 0;
		//Get initial value to rollback if needed on ISF
		for(i = 0; i < request->num_trans; i++){
			original_states[i] = read_account(request->transactions[i].acc_id);
		}
		//Get request and execute checking for potential ISF and outputting to file
		for(i = 0; i < request->num_trans; i++){
			int curr_bal = read_account(request->transactions[i].acc_id);
			int to_adjust = request->transactions[i].amount;
			if(to_adjust < 0){
				if(curr_bal < to_adjust * -1){
					failure = 1;
					gettimeofday(&request->end_time, NULL);
					fprintf(output,"%d ISF %d TIME %ld.%06ld %ld.%06ld\n",request->request_id,request->transactions[i].acc_id,request->start_time.tv_sec, request->start_time.tv_usec,request->end_time.tv_sec, request->end_time.tv_usec);
				}else{
					write_account(request->transactions[i].acc_id, curr_bal + to_adjust);
				}
			}else{
				write_account(request->transactions[i].acc_id, curr_bal + to_adjust);
			}
		}
		//Do the rollback if we failed
		if(failure){
			for(i = 0; i < request->num_trans; i++){
				write_account(request->transactions[i].acc_id, original_states[i]);
			}
			free(request->request_string);
			free(request->transactions);
			free(request);
		}else{
			//Otherwise priont OK
			gettimeofday(&request->end_time, NULL);
			fprintf(output,"%d OK TIME %ld.%06ld %ld.%06ld\n",request->request_id,request->start_time.tv_sec, request->start_time.tv_usec,request->end_time.tv_sec, request->end_time.tv_usec);
			free(request->request_string);
			free(request->transactions);
			free(request);
		}
		//Unlock the mutexs
		for(i = 0; i < request->num_trans; i++){
			pthread_mutex_unlock(&account_locks[locks_needed[i]]);
		}

	}
}
//Compare for qsort
int compare( const void* a, const void* b)
{
     int int_a = * ( (int*) a );
     int int_b = * ( (int*) b );

     if ( int_a == int_b ) return 0;
     else if ( int_a < int_b ) return -1;
     else return 1;
}
