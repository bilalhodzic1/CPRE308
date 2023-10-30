#include "project2.h"
#include "queue.h"

int id = 0;



int main(int argc, char* argv[]){
	// int num_threads = atoi(argv[1]);
	// int num_accounts = atoi(argv[2]);
	// char* filename  = argv[3];

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
	return 0;
}
