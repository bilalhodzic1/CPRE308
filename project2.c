#include "project2.h"
#include "queue.h"
#include "Bank.h"

int id = 0;



int main(int argc, char* argv[]){
	// int num_threads = atoi(argv[1]);
	// int num_accounts = atoi(argv[2]);
	// char* filename  = argv[3];

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

	initialize_accounts(2);


	queue_t* q = createQueue();
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
