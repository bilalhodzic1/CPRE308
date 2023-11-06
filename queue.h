#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct trans{
    int acc_id;
    int amount;
}trans_t;

typedef struct request{
    int request_id;
    int check_acc_id;
    trans_t* transactions;
    int num_trans;
    struct timeval start_time, end_time;
    char* request_string;
}request_t;

typedef struct QNode {
    request_t* request;
    struct QNode* next;
} qNode_t;
 
// The queue, front stores the front node of LL and rear
// stores the last node of LL
typedef struct Queue {
    struct QNode *front, *rear;
    int size;
}queue_t;
 
struct QNode* newNode(request_t* request, char* request_string);
struct Queue* createQueue();
void enQueue(struct Queue* q, request_t* newreq, char* request_string);
struct QNode deQueue(struct Queue* q);

