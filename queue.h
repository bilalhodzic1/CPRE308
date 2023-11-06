#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct QNode {
    char* transaction;
    struct QNode* next;
} qNode_t;
 
// The queue, front stores the front node of LL and rear
// stores the last node of LL
typedef struct Queue {
    struct QNode *front, *rear;
    int size;
}queue_t;
 
struct QNode* newNode(char* transaction);
struct Queue* createQueue();
void enQueue(struct Queue* q, char* transaction);
struct QNode deQueue(struct Queue* q);

