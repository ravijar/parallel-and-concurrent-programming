#ifndef LINKED_LIST_RWLOCK_H
#define LINKED_LIST_RWLOCK_H

#include <pthread.h>
#include "linked_list_node.h"

// Global read-write lock for synchronization
extern pthread_rwlock_t rwlock;

// Function declarations for read-write lock-based linked list
int Member(int value, list_node_t* head_p);
int Insert(int value, list_node_t** head_pp);
int Delete(int value, list_node_t** head_pp);

#endif  // LINKED_LIST_RWLOCK_H
