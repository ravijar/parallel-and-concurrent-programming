#ifndef LINKED_LIST_MUTEX_H
#define LINKED_LIST_MUTEX_H

#include <pthread.h>
#include "linked_list_node.h"

// Global mutex for synchronization
extern pthread_mutex_t mutex;

// Function declarations for mutex-based linked list
int Member(int value, list_node_t* head_p);
int Insert(int value, list_node_t** head_pp);
int Delete(int value, list_node_t** head_pp);

#endif  // LINKED_LIST_MUTEX_H
