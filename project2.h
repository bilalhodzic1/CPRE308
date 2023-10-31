#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <wait.h>
#include <sys/time.h>

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
}request_t;

void* work();

request_t parse_transaction(char* transaction);

