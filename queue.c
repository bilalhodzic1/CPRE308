#include "queue.h"

// A utility function to create a new linked list node.
struct QNode* newNode(request_t* request, char* request_string)
{
    struct QNode* temp
        = (struct QNode*)malloc(sizeof(struct QNode));
    char* copy = malloc(sizeof(char) * (strlen(request_string) + 1));
    strcpy(copy, request_string);
    request->request_string = copy;
    temp->request = request;
    temp->next = NULL;
    return temp;
}
 
// A utility function to create an empty queue
struct Queue* createQueue()
{
    struct Queue* q
        = (struct Queue*)malloc(sizeof(struct Queue));
    q->front = q->rear = NULL;
    q->size = 0;
    return q;
}
 
// The function to add a key k to q
void enQueue(struct Queue* q, request_t* newreq, char* request_string)
{
    // Create a new LL node
    struct QNode* temp = newNode(newreq, request_string);
 
    // If queue is empty, then new node is front and rear
    // both
    if (q->rear == NULL) {
        q->front = temp;
        q->rear = temp;
        q->size++;
        return;
    }
 
    // Add the new node at the end of queue and change rear
    q->rear->next = temp;
    q->rear = temp;
    q->size++;
}
 
// Function to remove a key from given queue q
struct QNode deQueue(struct Queue* q)
{
    struct QNode adjuster; 
    if (q->size == 0){
        request_t new_bogus;
        new_bogus.num_trans = -1;
        adjuster.request = &new_bogus;
       return adjuster;
    }
 
    // Store previous front and move front one node ahead
    struct QNode* temp = q->front;
    adjuster  = *temp;
    q->front = q->front->next;
    q->size--;
 
    // If front becomes NULL, then change rear also as NULL
    if (q->front == NULL)
        q->rear = NULL;
    free(temp);
    return adjuster;
}
