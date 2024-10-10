#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "linked_list_node.h"

// Function declarations for serial linked list
int Member(int value, list_node_t* head_p);
int Insert(int value, list_node_t** head_pp);
int Delete(int value, list_node_t** head_pp);

#endif  // LINKED_LIST_H
