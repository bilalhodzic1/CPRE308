#include "project2.h"
#include "queue.h"
#include "Bank.h"


int external_id = 1;
int internal_id = 1;

int num_threads;
int num_accounts;

pthread_mutex_t* account_locks;
pthread_t* worker_threads;
pthread_mutex_t id_lock;

queue_t* q; 

int main(int argc, char* argv[]){
	num_threads = atoi(argv[1]);
	num_accounts = atoi(argv[2]);
	char* filename  = argv[3];

	worker_threads = malloc(num_threads * sizeof(pthread_t));
	account_locks = malloc(num_accounts * sizeof(pthread_mutex_t));
	pthread_mutex_init(&id_lock, NULL);
	int i;
	initialize_accounts(num_accounts);
	q = createQueue();
	for(i = 0; i < num_threads; i++){
		pthread_create(&worker_threads[i], NULL, work, NULL);
	}
	for(i = 0; i < num_accounts; i++){
		pthread_mutex_init(&account_locks[i], NULL);
	}


	char transaction[100];
	while(1){
		fgets(transaction, 100, stdin);
		int valid = check_valid(transaction);
		if(valid){
			printf("ID: %d\n", external_id);
			external_id++;
			enQueue(q, transaction);
		}
	}

	//Threads should pop the queue. Get them damn locks. Gottem!


	return 0;
}

void* work(){
	while(1){
		if(q->size != 0){
			qNode_t gottem = deQueue(q);
			request_t test = parse_transaction(gottem.transaction);
		}
	}
}

request_t parse_transaction(char* transaction){
	request_t new_request;
	char copytrans[100];
	strcpy(copytrans, transaction);
	char* token = strtok(copytrans, " ");
	int valid = 0;
	if(strcmp(token, "CHECK") == 0){
		pthread_mutex_lock(&id_lock);
		token = strtok(NULL, " ");
		new_request.check_acc_id = atoi(token);
		new_request.request_id = internal_id;
		internal_id++;
		pthread_mutex_unlock(&id_lock);
	}else if(strcmp(token, "TRANS") == 0){

	}else{

	}
	return new_request;
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
