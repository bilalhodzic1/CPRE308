#include "project2.h"
#include "queue.h"
#include "Bank.h"


int id = 0;

int num_threads;
int num_accounts;

pthread_mutex_t* account_locks;
pthread_t* worker_threads;

queue_t* q; 

int main(int argc, char* argv[]){
	num_threads = atoi(argv[1]);
	num_accounts = atoi(argv[2]);
	char* filename  = argv[3];

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


	char transaction[100];
	while(1){
		fgets(transaction, 100, stdin);
		enQueue(q, transaction);
		if(q->size == 5){
			while(q->size != 0){
				qNode_t test = deQueue(q);
				printf("%s\n", test.transaction);
			}
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
	char* token = strtok(transaction, " ");
	if(strcmp(token, "ID") == 0){
		printf("Transaction after 1 strtok: %s\n", transaction);
		token = strtok(NULL, " ");
		new_request.check_acc_id = atoi(token);
	}
	printf("New request check id: %d\n", new_request.check_acc_id);
	return new_request;
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
