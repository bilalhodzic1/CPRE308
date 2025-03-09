#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <wait.h>
#include <sys/time.h>
#include "queue.h"
#include <time.h>

void* work();

void parse_transaction(request_t* request);

int check_valid(char* transaction);

void process_transaction(request_t* request);
int compare( const void* a, const void* b);
